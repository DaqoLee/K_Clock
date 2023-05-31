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


void ButtonTask(void *arg)
{
    static int i = 0;
    while (1)
    {
        /* code */
       // printf("SendTask: i=%d\n",i++);
        if(i<100)
        {

        }
        printf("ButtonTask\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    
}

void ClockTask(void *arg)
{
   
    while (1)
    {
        /* code */
        printf("ClockTask\n");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    
}


extern "C" void app_main(void)
{
    Clock1.init();
    

    xTaskCreate(ButtonTask, "ButtonTask", 2048, NULL, 1, NULL);
    xTaskCreate(ClockTask, "ClockTask", 2048, NULL, 1, NULL);
    while (true)
	 {

        //Clock1.getWeatherCode();
        vTaskDelay(3000 / portTICK_PERIOD_MS);

    }
}

