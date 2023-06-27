#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_sleep.h"


#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_sntp.h"
#include "button.h"

#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include <driver/gpio.h>

#include "cJSON.h"

#include "DendoStepper.h"
#include "K_Clock.h"
#include "button.h"




K_clock Clock1;

EventGroupHandle_t clock_event_group;

void ButtonTask(void *arg)
{
    uint16_t timeInterval = 0;
    Button_init(10);
    while (1)
    {
        Button_process();
        if(Button_getItemData(BUTTON_USERBTN)->longPressedJump)
        {
            xEventGroupSetBits(clock_event_group, BTNLONG_BIT);
            printf("BUTTON_USERBTN longPressedJump\n");
        }
        else if(Button_getItemData(BUTTON_USERBTN)->releaseJump)
        {
            xEventGroupSetBits(clock_event_group, BTNJUMP_BIT);
            printf("BUTTON_USERBTN releaseJump\n");
        }
  
       // printf("ButtonTask\n");
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}

void ClockTask(void *arg)
{
    EventBits_t uxBits;
    Clock_config_t clockConfig = {
        .wifiConfig = {
            .sta = {
                "ssid", //ssid
                "password", // password
                .bssid_set = false,
            }
        },
        .ntpUrl = "ntp.aliyun.com",
        .userKey = "userKey", //心之天气userKey
        .location = "guangzhou",
        .powerPin = GPIO_NUM_4,
        .pages = 24, //叶片数
        .ntpInterval = 30,//网络校准时间间隔/分钟
        .speed = 2000, // 电机速度
        .compensation = 1000, //步进数误差补偿

        .stepperConfig = {
            .stepPin = 27, 
            .dirPin = 26, 
            .enPin = 25,
            .ms1Pin = 14, // 细分控制
            .ms2Pin = 12, //
            .timer_group = TIMER_GROUP_0,
            .timer_idx = TIMER_1,
            .miStep = MICROSTEP_16,  //电机驱动细分
            .stepAngle = 0.086//0.086 0.12 步进电机减速后的步进角
        }

    };

    Clock1.config(&clockConfig);
    Clock1.init();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    //Clock1.runPages(Clock1.getTimeHour());
    Clock1.runPages(Clock1.getWeatherCode());
    vTaskDelay(5000/portTICK_PERIOD_MS);
   // vTaskDelay((60-Clock1.getTimeSec())*1000/portTICK_PERIOD_MS);
    while (1)
    {
        uxBits = xEventGroupWaitBits(
                       clock_event_group,
                       SNTP_BIT | BTNJUMP_BIT | BTNLONG_BIT,
                       pdTRUE,
                       pdFALSE,
                       60000/portTICK_PERIOD_MS );//60000/portTICK_PERIOD_MS portMAX_DELAY

        if(uxBits & SNTP_BIT)//时间校准事件
        {
            Clock1.runPages(Clock1.getWeatherCode());
            //Clock1.runPages(Clock1.getTimeHour());
        }
        else if (uxBits & BTNJUMP_BIT)//按键短按跳变/松开
        {
            Clock1.runInf(200);
        }
        else if (uxBits & BTNLONG_BIT)//按键长按
        {    
            Clock1.powerOFF();
        }
        else //最大等待时间
        {     
           // Clock1.runPages(Clock1.getTimeHour());
        }
    }
    
}

extern "C" void app_main(void)
{
   // ESP_ERROR_CHECK(nvs_flash_init());

    clock_event_group = xEventGroupCreate();
    if (clock_event_group != NULL)
    {    
      xTaskCreate(ClockTask, "ClockTask", 4096, NULL, 1, NULL);
      xTaskCreate(ButtonTask, "ButtonTask", 2048, NULL, 1, NULL);
     // vTaskStartScheduler();
    }
    else
    {
        
    }


}





