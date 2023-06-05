#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

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
    Button_init(10);
    while (1)
    {

        Button_process();
        /* code */
       // printf("SendTask: i=%d\n",i++);
        if(Button_getItemData(BUTTON_USERBTN)->longPressedJump)
        {
            xEventGroupSetBits(clock_event_group, BIT2);
            printf("BUTTON_USERBTN longPressedJump\n");
        }
        else if(Button_getItemData(BUTTON_USERBTN)->releaseJump)
        {
            xEventGroupSetBits(clock_event_group, BIT1);
            printf("BUTTON_USERBTN releaseJump\n");
        }

        if(Button_getItemData(BUTTON_RESTBTN)->pressedJump)
        {
            xEventGroupSetBits(clock_event_group, BIT3);
            printf("BUTTON_RESTBTN pressedJump\n");
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
                "HUAWEI-DFP",
                "dianfengpai",
                .bssid_set = false,
            }
        },

        .ntpUrl = "ntp.aliyun.com",

        .userKey = "Sq4f2lh5b3lLPTCWr",
        .location = "guangzhou",

        .powerPin = GPIO_NUM_4,
        .pages = 24,
        .ntpInterval = 30,

        .stepperConfig = {
            .stepPin = 27,
            .dirPin = 26,
            .enPin = 25,
            .ms1Pin = 14,
            .ms2Pin = 12,
            .timer_group = TIMER_GROUP_0,
            .timer_idx = TIMER_1,
            .miStep = MICROSTEP_16,
            .stepAngle = 0.086
        }

    };

    Clock1.config(&clockConfig);
    Clock1.init();

    while (1)
    {
        uxBits = xEventGroupWaitBits(
                       clock_event_group,
                       BIT0 | BIT1 | BIT2,
                       pdTRUE,
                       pdFALSE,
                       portMAX_DELAY );

      if(uxBits & BIT0)
      {
        Clock1.runPages(Clock1.getTimeHour());
        printf("BIT0\n");
        
      }
      else if (uxBits & BIT1)
      {
        printf("BIT1\n");
        Clock1.runPos(200);
       // printf("getWeatherCode %d \n",Clock1.getWeatherCode());
      }
      else if (uxBits & BIT2)
      {    
        printf("BIT2 powerOFF \n");
        Clock1.powerOFF();
      }
      else if (uxBits & BIT3)
      {
        
        // printf("BIT3 resetPos \n");
        // Clock1.resetPos();
      }
        vTaskDelay(10/portTICK_PERIOD_MS);
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

