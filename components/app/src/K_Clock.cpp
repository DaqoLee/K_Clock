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
#define WIFI_SSID "HUAWEI-DFP"
#define WIFI_PASSWORD "dianfengpai"
#define NTP_URL		"ntp.aliyun.com"	//NTP服务器地址

#define MAX_STEPS (68880)//(360/0.35)*16 68880  16400
#define SNTP_INTERVAL (1) //SNTP同步周期设定:分钟


#define MAX_HTTP_OUTPUT_BUFFER 1300
#define HOST "api.seniverse.com"
#define UserKey "Sq4f2lh5b3lLPTCWr"
#define Location "guangzhou"
#define Language "en"
/* Private types -------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
static const char *CLOCK_TAG = "K_Clock";
 DendoStepper step1;
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


/* Private functions ---------------------------------------------------------*/

void K_clock::wifi_event_connect_handle(void* event_handler_arg,esp_event_base_t event_base,int32_t event_id,void* event_data)
{
    	/*获取事件数据*/
	wifi_event_sta_connected_t* data = (wifi_event_sta_connected_t*)event_data;

	printf("Connected to the Wifi Successful & Tiggle Special Event\n");
	printf("Event_base:%s\n",event_base);
	printf("Event Id:%d\n",event_id);
	printf("Wifi SSID:%.13s\n" , data->ssid);
	printf("Wifi Channel:%d\n" , data->channel);

}

void K_clock::wifi_event_disconnect_handle(void* event_handler_arg,esp_event_base_t event_base,int32_t event_id,void* event_data)
{
	wifi_event_sta_disconnected_t* eventdata = (wifi_event_sta_disconnected_t*)event_data;
	if(eventdata->reason == WIFI_REASON_NO_AP_FOUND)
	{
		printf("Can't Found The AP\n");
		vTaskDelay(5000/portTICK_PERIOD_MS);
	}
	esp_wifi_connect();
}

esp_err_t K_clock::event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


//    uint8_t ssid[32];      /**< SSID of target AP. */
//    uint8_t password[64];  /**< Password of target AP. */
void K_clock::wifiConnect(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    wifi_config_t sta_config = {
        .sta = {
             CONFIG_ESP_WIFI_SSID,
             CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false,
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect());


    /*注册WIFI连接和断开连接事件处理函数*/
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, wifi_event_connect_handle, NULL, NULL);
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, wifi_event_disconnect_handle, NULL, NULL);

	vTaskDelay(1000 / portTICK_PERIOD_MS);

}

void K_clock::motorInit(void)
{
    DendoStepper_config_t step1_cfg = {
    .stepPin = 27,
    .dirPin = 26,
    .enPin = 25,
    .ms1Pin = 14,
    .ms2Pin = 12,
    .timer_group = TIMER_GROUP_0,
    .timer_idx = TIMER_1,
    .miStep = MICROSTEP_32,
    .stepAngle = 0.086};


    step1.config(&step1_cfg);


    step1.init();

    step1.setSpeed(1000, 100, 100);
    
}


void K_clock::sntpcallback(struct timeval *tv)
{
	if(sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
	{
		//printf("Sntp Auto Sync Finish\n");
        //printf("Hour %d , weatherCode %d\n",getTimeHour(), getWeatherCode());
        printf("Hour %d \n",getTimeHour());
        run(24, getTimeHour());
		//getTimeHour();
		
	}
	else
	{
		printf("Sntp Auto Sync Fail\n");
	}
}


esp_err_t K_clock::sntpInit(void)
{
	uint8_t retry=0;

	/*初始化SNTP并校时*/
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, NTP_URL);
    sntp_set_time_sync_notification_cb(sntpcallback);
    sntp_set_sync_interval(SNTP_INTERVAL*60000);
	sntp_init();

	/*设置时区并获取系统时间*/
	setenv("TZ", "CST-8", 1);
	tzset();
	while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED && retry < 50) {
		retry++;;
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	sntp_set_sync_status(SNTP_SYNC_STATUS_RESET);		//复位同步结果

    

	/*输出校时结果*/
	if(retry == 50)
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

esp_err_t K_clock::returnToZero(void)
{
    step1.setSpeed(3000, 100, 100);
    step1.resetAbsolute();
   // step2.runPos(MAX_STEPS);
    ESP_LOGI(CLOCK_TAG, "Return to zero");
    Button_process();
    while (!Button_getItemData(BUTTON_USERBTN2)->status && (step1.getPosition()< MAX_STEPS))
    {
        Button_process();
        step1.runPos(100);  
        vTaskDelay(10 / portTICK_PERIOD_MS);
        //ESP_LOGI(CLOCK_TAG, "Position:%lld\n",step2.getPosition());

    } 

    // ESP_LOGI(CLOCK_TAG, "Position:%lld\n",step2.getPosition());

    step1.stop();
    step1.disableMotor();
    step1.setSpeed(1000, 100, 100);
    
    if(step1.getPosition() < MAX_STEPS)
    {
        step1.resetAbsolute();
        ESP_LOGI(CLOCK_TAG, "Return to zero");
        return ESP_OK;
    }
    else
    {        
        ESP_LOGI(CLOCK_TAG, "Cannot return to zero!!");
        return ESP_ERR_TIMEOUT;
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
		ESP_LOGI(CLOCK_TAG, "root_arr_size: %d \n", arr_size);
		arr_item = arrayItem->child;
		for(int i = 0; i < arr_size; i++)
		{
			subArray = cJSON_GetObjectItem(arr_item, "now");
			int sub_array_size = cJSON_GetArraySize(subArray);
			ESP_LOGI(CLOCK_TAG, "sub_arr_size: %d \n", sub_array_size);
			//for(int j = 0; j < sub_array_size; j ++)
			{
				if(subArray->type == cJSON_Object)
				{
					JsonCode =  cJSON_GetObjectItem(subArray, "code");
					if(cJSON_IsString(JsonCode))
					{
						weather->code = JsonCode->valueint;
						ESP_LOGI(CLOCK_TAG, "code: %d \n", weather->code);

					}					
				}
			}
			ESP_LOGI(CLOCK_TAG, "Finish");
		}
		cJSON_Delete(root);
	}
}


uint8_t K_clock::getWeatherCode(void)
{
    Weather_t Weather;
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};   // Buffer to store response of http request
    int content_length = 0;
    uint8_t errCode = 100;
    static const char *URL ="http://api.seniverse.com/v3/weather/now.json?""key="UserKey"&location="Location"&language="Language"&unit=c";
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
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) 
			{
                ESP_LOGI(CLOCK_TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                printf("data:%s", output_buffer);
                esp_http_client_close(client);	
                weatherCodeParse(output_buffer, &Weather);
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


uint8_t K_clock::getTimeHour(void)
{
	time_t now;
	struct tm timeinfo;
	char strftime_buf[64];

	time(&now);						//获取系统时间s
	localtime_r(&now, &timeinfo);	//将获取到的系统时间s转换为带有格式的timeinfo信息
	// strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	// printf("System Time: %s\n" , strftime_buf);
    return timeinfo.tm_hour;
}

void K_clock::run(uint8_t num,int32_t clockSteps)
{

    step1.runPos((MAX_STEPS/num)*clockSteps - step1.getPosition());  

}

void K_clock::init(void)
{
    // gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    // gpio_set_level(GPIO_NUM_4,1);

    wifiConnect();
    Button_init(10);
    motorInit();
    returnToZero();
    sntpInit();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Hour %d , weatherCode %d\n",getTimeHour(), getWeatherCode());
    // getWeatherCode();
}