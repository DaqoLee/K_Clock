/**
 ******************************************************************************
 * @file           user.c
 * @description:   
 * @author:        Hare
 * @date:          2022-10-16
 * @version:       v0.1
 * @history: 
 *    [version]   [date]       [author]       [description]
 *    v0.1   	    2022-10-16   Hare           create
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "K_Clock.h"

/* Defines -------------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
static const char *CLOCK_TAG = "K_Clock";
extern EventGroupHandle_t clock_event_group;

/* Private variables ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Import function prototypes ------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
K_clock::K_clock(/* args */)
{
}

K_clock::~K_clock()
{
}

void K_clock::config(Clock_config_t *config)
{
    memcpy(&conf, config, sizeof(conf));
}



void K_clock::init(void)
{
    powerON();
    wifiConnect();
    Button_init(10);
    motorInit();
    zeroing();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    sntpInit();
   // ESP_LOGI(CLOCK_TAG,"Hour %d , weatherCode %d\n",getTimeHour(), getWeatherCode());

}


void K_clock::wifiConnect(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(esp_event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    // wifi_config_t sta_config = {
    //     .sta = {
    //          CONFIG_ESP_WIFI_SSID,
    //          CONFIG_ESP_WIFI_PASSWORD,
    //         .bssid_set = false,
    //     }
    // };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &conf.wifiConfig));
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect());


    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    
	ESP_LOGI(CLOCK_TAG, "wifi_init_sta finished.");

}

void K_clock::motorInit(void)
{
    // DendoStepper_config_t step1_cfg = {
    // .stepPin = 27,
    // .dirPin = 26,
    // .enPin = 25,
    // .ms1Pin = 14,
    // .ms2Pin = 12,
    // .timer_group = TIMER_GROUP_0,
    // .timer_idx = TIMER_1,
    // .miStep = MICROSTEP_16,
    // .stepAngle = 0.086
    // };


    step1.config(&conf.stepperConfig);


    step1.init();

    step1.setSpeed(conf.speed, 100, 100);
    
}



esp_err_t K_clock::sntpInit(void)
{
	uint8_t retry=0;

	/*初始化SNTP并校时*/
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, conf.ntpUrl);
    sntp_set_time_sync_notification_cb(sntpcallback);
    sntp_set_sync_interval(conf.ntpInterval*60000);
	sntp_init();

	/*设置时区并获取系统时间*/
	setenv("TZ", "CST-8", 1);
	tzset();
	while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED && retry < 10) {
		retry++;;
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	sntp_set_sync_status(SNTP_SYNC_STATUS_RESET);		//复位同步结果

    

	/*输出校时结果*/
	if(retry == 10)
	{
        ESP_LOGI(CLOCK_TAG,"SNTP Sync Time Out" );
		return ESP_ERR_TIMEOUT;
	}
	else
	{
        ESP_LOGI(CLOCK_TAG, "SNTP Sync Successful(Retry %d Times)\n" , retry);
		
		return ESP_OK;
	}
    
}


//https://seniverse.yuque.com/hyper_data/api_v3/yev2c3
uint8_t K_clock::weatherCodeSwitch(uint8_t weatherCode)
{
    switch (weatherCode)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        return SUNNY;
        break;

    case 4:
    case 5:
    case 6:
    case 7:
    case 8:   
        return CLOUDY;
        break;

    case 9:       
        return OVERCAST;
        break;

    case 10:     
        return SHOWER;
        break;

    case 11:
        return THUNDERSHOWER;
        break;

    case 13:
        return LIGHT_RAIN;
        break;

    case 14:
        return MODERATE_RAIN;
        break;

    case 15:
        return HEAVY_RAIN;
        break;

    case 16:
        return STORM;
        break;  

    case 20:
        return SLEET;
        break;

    case 22:
        return LIGHT_SNOW;
        break;

    case 23:
        return MODERATE_SNOW;
        break;

    case 24:
        return HEAVY_SNOW;
        break;

    case 25:
        return SNOWSTORM;
        break;

    case 30:
        return FOGGY;
        break;

    case 34:
        return HURRICANE;
        break;

    case 12:
        return HAIL;
        break;

    case 99:
        return FROST;
        break;       

    case 28:
        return DUSTSTORM;

    case 18:
        return SEVERE_STORM;

    case 33:
        return BLUSTERY;

    case 35:
        return THOUNDER;

    case 36:
        return TORNADO;
        break;

    default:
        return FROST;
        break;
    }
}

void K_clock::weatherCodeParse(char* str ,  Weather_t* weather)
{
	
	cJSON *root,*arrayItem,*subArray;
	cJSON *arr_item;
	cJSON *JsonCode;
	root = cJSON_Parse(str);
	if(root!=NULL)
	{
		arrayItem = cJSON_GetObjectItem(root,"results");
		//ESP_LOGE(HTTP_TAG, "Data IS json");
		int arr_size = cJSON_GetArraySize(arrayItem);
		//ESP_LOGI(CLOCK_TAG, "root_arr_size: %d \n", arr_size);
		arr_item = arrayItem->child;
		for(int i = 0; i < arr_size; i++)
		{
			subArray = cJSON_GetObjectItem(arr_item, "now");
			int sub_array_size = cJSON_GetArraySize(subArray);
			//ESP_LOGI(CLOCK_TAG, "sub_arr_size: %d \n", sub_array_size);
			//for(int j = 0; j < sub_array_size; j ++)
			{
				if(subArray->type == cJSON_Object)
				{
					JsonCode =  cJSON_GetObjectItem(subArray, "code");
					if(cJSON_IsString(JsonCode))
					{
						weather->code = atoi(JsonCode->valuestring);//((JsonCode->valuestring[0]-'0')*10 + (JsonCode->valuestring[1]-'0'));
						//ESP_LOGI(CLOCK_TAG, "code: %d \n", weather->code);

					}					
				}
			}
			//ESP_LOGI(CLOCK_TAG, "Finish");
		}
		cJSON_Delete(root);
	}
}


uint8_t K_clock::getWeatherCode(void)
{
    Weather_t Weather;
    const uint16_t maxHttpBuff = 1300;
    char output_buffer[maxHttpBuff] = {0};   // Buffer to store response of http request
    int content_length = 0;
    uint8_t errCode = 100;
    char URL[200] = {0}; //="http://api.seniverse.com/v3/weather/now.json?""key="UserKey"&location="Location"&language="Language"&unit=c";
    
    sprintf(URL,"http://api.seniverse.com/v3/weather/now.json?key=%s&location=%s&language=en&unit=c",conf.userKey, conf.location);
    esp_http_client_config_t config = {
        .url = URL,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET Request
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) 
	{
        ESP_LOGI(CLOCK_TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        esp_http_client_close(client);	
        return errCode;
    } 
	else 
	{
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) 
		{
            ESP_LOGI(CLOCK_TAG, "HTTP client fetch headers failed");
            esp_http_client_close(client);	
            return errCode;
        } 
		else
		{
            int data_read = esp_http_client_read_response(client, output_buffer, maxHttpBuff);
            if (data_read >= 0) 
			{
                // ESP_LOGI(CLOCK_TAG, "HTTP GET Status = %d, content_length = %d",
                // esp_http_client_get_status_code(client),
                // esp_http_client_get_content_length(client));
                ESP_LOGI(CLOCK_TAG,"data:%s", output_buffer);
                esp_http_client_close(client);	
                weatherCodeParse(output_buffer, &Weather);
                ESP_LOGI(CLOCK_TAG, "Weather code: %d \n", weatherCodeSwitch(Weather.code));
				return weatherCodeSwitch(Weather.code);
            }
			else 
			{
                ESP_LOGI(CLOCK_TAG, "Failed to read response");
                esp_http_client_close(client);	
                return errCode;
            }
        }
    }
    
}

uint8_t K_clock::getState(void)
{
    
    return step1.getState();
}



uint8_t K_clock::getTimeHour(void)
{
	time_t now;
	struct tm timeinfo;
	char strftime_buf[64];

	time(&now);						//获取系统时间s
	localtime_r(&now, &timeinfo);	//将获取到的系统时间s转换为带有格式的timeinfo信息
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(CLOCK_TAG,"System Time: %s\n" , strftime_buf);
    return timeinfo.tm_hour;
}

uint8_t K_clock::getTimeMin(void)
{
	time_t now;
	struct tm timeinfo;
	char strftime_buf[64];

	time(&now);						//获取系统时间s
	localtime_r(&now, &timeinfo);	//将获取到的系统时间s转换为带有格式的timeinfo信息
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(CLOCK_TAG,"System Time: %s\n" , strftime_buf);
    return timeinfo.tm_min;
}

uint8_t K_clock::getTimeSec(void)
{
	time_t now;
	struct tm timeinfo;
	char strftime_buf[64];

	time(&now);						//获取系统时间s
	localtime_r(&now, &timeinfo);	//将获取到的系统时间s转换为带有格式的timeinfo信息
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI(CLOCK_TAG,"System Time: %s\n" , strftime_buf);
    return timeinfo.tm_sec;
}

void K_clock::runPages(int16_t value)
{
    static uint16_t lastValue = 0;
    uint16_t timeOut = 200;
    
    if(lastValue != value)
    {
        int32_t steps = (step1.getStepsPerRot()/conf.pages)*value - step1.getPosition();

        if(value == 0|| steps < 0)
        {
            ESP_LOGI(CLOCK_TAG,"steps: %d  ,value %d\n", steps, value);
            zeroing();

            steps = (step1.getStepsPerRot()/conf.pages)*value - step1.getPosition();
            step1.runPos(steps); 
        }
        else
        {
            step1.runPos(steps); 

            while (step1.getState() > IDLE && timeOut--)
            {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            // printf("timeOut: %d\n" , timeOut);
            step1.disableMotor();
        }
        lastValue = value;
    }
    else
    {

    }



}

void K_clock::runPos(int16_t value)
{
    static uint16_t lastValue = 0;
    uint16_t timeOut = 200;

    if(lastValue != value)
    {
        step1.runPos(value); 
        while (step1.getState() != IDLE && timeOut--)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    // printf("timeOut: %d\n" , timeOut);
        step1.disableMotor();
        lastValue = value;
    }
    else
    {

    }
   
}

void K_clock::runInf(int16_t value)
{
    uint16_t timeOut = 200;
    step1.runInf(1); 

    vTaskDelay(value / portTICK_PERIOD_MS);
    step1.stop();
    step1.disableMotor();
}



void K_clock::resetPos(void)
{
    step1.resetAbsolute();
}



esp_err_t K_clock::zeroing(void)//zeroing
{
     uint16_t timeOut = 60;
   
    step1.resetAbsolute();
    ESP_LOGI(CLOCK_TAG, "zeroing");
    Button_process();
    //step1.runPos((step1.getStepsPerRot()+ conf.compensation)); 
    while (!Button_getItemData(BUTTON_RESTBTN)->status && (step1.getPosition()<= (step1.getStepsPerRot()+ conf.compensation)))
    {
        Button_process();
        step1.runPos(200); 
        vTaskDelay(10 / portTICK_PERIOD_MS);
        //ESP_LOGI(CLOCK_TAG, "Position:%lld\n",step2.getPosition());

    } 

    // ESP_LOGI(CLOCK_TAG, "Position:%lld\n",step2.getPosition());
    step1.stop();

    if(step1.getPosition() <= (step1.getStepsPerRot()+ conf.compensation))
    {
        step1.runPos(conf.compensation); 

        while (step1.getState() > IDLE && timeOut--)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }     
        step1.disableMotor();   
        step1.resetAbsolute();

        ESP_LOGI(CLOCK_TAG, "end of zeroing");
        return ESP_OK;
    }
    else
    {        
        ESP_LOGI(CLOCK_TAG, "Cannot return to zero!!");
        return ESP_ERR_TIMEOUT;
    }

}


void K_clock::powerON(void)
{
    gpio_set_direction(conf.powerPin , GPIO_MODE_OUTPUT);
    gpio_set_level(conf.powerPin ,1);  
}

void K_clock::powerOFF(void)
{
    gpio_set_level(conf.powerPin ,0); 
}




/* Private functions ---------------------------------------------------------*/

void K_clock:: wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if(event_base == WIFI_EVENT)
    {
		wifi_event_sta_connected_t *wifi_sta_event = (wifi_event_sta_connected_t*) event_data;
		switch(event_id)
        {
			case WIFI_EVENT_STA_START:       
				ESP_LOGI(CLOCK_TAG, "connect to AP......");
				esp_wifi_connect();
				break;

			case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(CLOCK_TAG,"Connected to the Wifi Successful & Tiggle Special Event\n");
                ESP_LOGI(CLOCK_TAG,"Event_base:%s\n",event_base);
                ESP_LOGI(CLOCK_TAG,"Event Id:%d\n",event_id);
                ESP_LOGI(CLOCK_TAG,"Wifi SSID:%.13s\n" , wifi_sta_event->ssid);
                ESP_LOGI(CLOCK_TAG,"Wifi Channel:%d\n" , wifi_sta_event->channel);
				break;

			case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(CLOCK_TAG, "retry to connect to AP...... ");
                //esp_wifi_connect();
				break;

			default:
				break;           
		}
	}else if(event_base ==  IP_EVENT)
    {
		ip_event_got_ip_t* ip_event = (ip_event_got_ip_t*) event_data;
		if(event_id == IP_EVENT_STA_GOT_IP)
        {
			ESP_LOGI(CLOCK_TAG, "got ip:" IPSTR, IP2STR(&ip_event->ip_info.ip));

		}
	}
}

esp_err_t K_clock::esp_event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


void K_clock::sntpcallback(struct timeval *tv)
{
	if(sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
	{
		 xEventGroupSetBits(clock_event_group, SNTP_BIT);
	
	}
	else
	{
		printf("Sntp Auto Sync Fail\n");
	}
}


