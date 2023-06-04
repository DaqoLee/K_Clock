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
    static int i = 0;
    Button_init(10);
    while (1)
    {

        Button_process();
        /* code */
       // printf("SendTask: i=%d\n",i++);
        if(Button_getItemData(BUTTON_USERBTN)->pressedJump)
        {
            xEventGroupSetBits(clock_event_group, BIT1);
            printf("BUTTON_USERBTN pressedJump\n");
        }
       // printf("ButtonTask\n");
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}

void ClockTask(void *arg)
{
    EventBits_t uxBits;
    uint8_t count = 0;

   Clock1.init();
    while (1)
    {


        uxBits = xEventGroupWaitBits(
                       clock_event_group,
                       BIT0|BIT1,
                       pdTRUE,
                       pdFALSE,
                       portMAX_DELAY );

        printf("getupdateFlag \n");


        // if(Clock1.getupdateFlag())
        // {
        //    // Clock1.getTimeHour();
        //    // Clock1.run(24, Clock1.getTimeHour());
        //     //printf("ClockTask Hour %d \n",Clock1.getWeatherCode());
        //     Clock1.setupdateFlag(0);
        // }
       
      if(uxBits & BIT0)
      {
        Clock1.run(24, count++);
       // xEventGroupClearBits(wifi_event_group, BIT0);
        printf("BIT0\n");
        
      }
      else if (uxBits & BIT1)
      {
        printf("BIT1\n");
        printf("getWeatherCode %d \n",Clock1.getWeatherCode());
      }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    clock_event_group = xEventGroupCreate();
    
    if (clock_event_group != NULL)
    {
      xTaskCreate(ButtonTask, "ButtonTask", 2048, NULL, 1, NULL);
      xTaskCreate(ClockTask, "ClockTask", 4096, NULL, 1, NULL);
     // vTaskStartScheduler();
    }
    else
    {
        
    }
}

