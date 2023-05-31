/**
 ******************************************************************************
 * @file           user.h
 * @description:   
 * @author:        
 * @date:          2022-10-16
 * @version:       v0.1
 * @history: 
 *    [version]   [date]       [author]       [description]
 *    v0.1   	    2022-10-16              create
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __K_CLOCK_H__
#define __K_CLOCK_H__



#ifdef __cplusplus
extern "C" {
#endif


#include "stdint.h"
#include "stdio.h"
#include <cstring>
#include "driver/timer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "math.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_http_client.h"

#include "nvs_flash.h"
#include "esp_sntp.h"
#include "button.h"

#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "DendoStepper.h"
#include "cJSON.h"
/* Includes ------------------------------------------------------------------*/

/* Defines -------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/*


{
  "results": [
    {
      "location": {
        "id": "C23NB62W20TF",
        "name": "西雅图",
        "country": "US",
        "path": "西雅图,华盛顿州,美国",
        "timezone": "America/Los_Angeles",
        "timezone_offset": "-07:00"
      },
      "now": {
        "text": "多云", //天气现象文字
        "code": "4", //天气现象代码
        "temperature": "14", //温度，单位为c摄氏度或f华氏度
        "feels_like": "14", //体感温度，单位为c摄氏度或f华氏度
        "pressure": "1018", //气压，单位为mb百帕或in英寸
        "humidity": "76", //相对湿度，0~100，单位为百分比
        "visibility": "16.09", //能见度，单位为km公里或mi英里
        "wind_direction": "西北", //风向文字
        "wind_direction_degree": "340", //风向角度，范围0~360，0为正北，90为正东，180为正南，270为正西
        "wind_speed": "8.05", //风速，单位为km/h公里每小时或mph英里每小时
        "wind_scale": "2", //风力等级，请参考：http://baike.baidu.com/view/465076.htm
        "clouds": "90", //云量，单位%，范围0~100，天空被云覆盖的百分比 #目前不支持中国城市#
        "dew_point": "-12" //露点温度，请参考：http://baike.baidu.com/view/118348.htm #目前不支持中国城市#
      },
      "last_update": "2015-09-25T22:45:00-07:00" //数据更新时间（该城市的本地时间）
    }
  ]
}

*/

enum weatherCode_t
{
    SUNNY = 0,
    CLOUDY,
    OVERCAST,
    SHOWER,
    THUNDERSHOWER,
    LIGHT_RAIN,
    MODERATE_RAIN,
    HEAVY_RAIN,
    STORM,
    SLEET,
    LIGHT_SNOW,
    MODERATE_SNOW,
    HEAVY_SNOW,
    SNOWSTORM,
    FOGGY,
    HURRICANE,
    HAIL,
    FROST,
    DUSTSTORM,
    SEVERE_STORM,
    BLUSTERY,
    THOUNDER,
    TORNADO
};

typedef struct
{
  char name[20];
  char text[20];
  uint8_t code;
  int8_t temperature;
  uint8_t humidity;

} Weather_t;



class K_clock
{
private:
  /* data */

  void wifiConnect(void);
  void wifiConnect(uint8_t* ssid, uint8_t* password);



  static void sntpcallback(struct timeval *tv);
  static esp_err_t event_handler(void *ctx, system_event_t *event);
  static esp_err_t sntpInit(void);
  static void wifi_event_connect_handle(void* event_handler_arg,esp_event_base_t event_base,int32_t event_id,void* event_data);
  static void wifi_event_disconnect_handle(void* event_handler_arg,esp_event_base_t event_base,int32_t event_id,void* event_data);
  static uint8_t weatherCodeSwitch(uint8_t weatherCode);

  static uint8_t getTimeHour(void);
  static uint8_t getWeatherCode(void);
  static void weatherCodeParse(char* str, Weather_t* weather);

public:
  K_clock();
  ~K_clock();

  esp_err_t returnToZero(void);
  void init(void);
  void motorInit(void);
 
  

  static void run(uint8_t num,int32_t clockSteps);
};


/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /*  */
