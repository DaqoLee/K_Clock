/**
 ******************************************************************************
 * @file           Button.c
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
#include "button.h"
#include <driver/gpio.h>

/* Defines -------------------------------------------------------------------*/
#define BUTTON_LONGPRESS_TIME       (1000)

/* Private types -------------------------------------------------------------*/
typedef gpio_num_t GPIO_config_t;
/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static const GPIO_config_t Button_IOConfig[BUTTON_PINLIST_MAX] = 
{
  BUTTON_USERBTN_PIN,
  BUTTON_USERBTN_PIN2
  //BUTTON_USERBTN_PIN3
};

Button_ItemData_t Button_ItemDataList[BUTTON_ITEM_MAX];

uint16_t Button_ProcessTimeMs = 10;
uint16_t Button_LongPressCount = 150;

/* Private macro -------------------------------------------------------------*/
#define BUTTON_GETSTATUS(pConfig)   gpio_get_level(*pConfig)
/* Import function prototypes ------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void GPIO_QuickConfigList(GPIO_config_t * IOConfig, uint8_t listsize)
{
  // gpio_config_t pGPIOConfig;
  for(int i=0; i < listsize ;i++)
  {
    // pGPIOConfig.intr_type = GPIO_INTR_DISABLE;
    // pGPIOConfig.mode = GPIO_MODE_DEF_INPUT;
    // pGPIOConfig.pin_bit_mask = IOConfig[i];
    // pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    // pGPIOConfig.pull_up_en = GPIO_PULLUP_ENABLE;

    // gpio_config(&pGPIOConfig);

    gpio_set_pull_mode(IOConfig[i], GPIO_PULLUP_ONLY);
    gpio_set_direction(IOConfig[i], GPIO_MODE_DEF_INPUT);
  }
     
}

void Button_init(uint16_t processTimeMs)
{		
   GPIO_QuickConfigList((GPIO_config_t*)Button_IOConfig, BUTTON_PINLIST_MAX);
  if(processTimeMs != 0)
  {
    Button_ProcessTimeMs = processTimeMs;
  }
  Button_LongPressCount = BUTTON_LONGPRESS_TIME / Button_ProcessTimeMs;
}

void Button_process(void)
{
  Button_ItemData_t *pItemData = Button_ItemDataList;
	for(int i = 0; i < BUTTON_ITEM_MAX; i++)
  {
		pItemData[i].releaseJump = 0;
		pItemData[i].pressedJump = 0;
    pItemData[i].longPressedJump = 0;
    pItemData[i].longReleaseJump = 0;
		
		pItemData[i].status = !BUTTON_GETSTATUS(&Button_IOConfig[i]);
		
		if (pItemData[i].status != pItemData[i].lastStatus)
    {
			if(pItemData[i].status)
      {
				pItemData[i].pressedJump = pItemData[i].status;
			}
			else
      {
        if(pItemData[i].longPressed)
        {
          pItemData[i].longReleaseJump = pItemData[i].lastStatus;
        }
        else
        {
          pItemData[i].releaseJump = pItemData[i].lastStatus;
        }
				pItemData[i].longPressed = 0;
				pItemData[i].timer = 0;
			}
		}
    else
    {
      if(pItemData[i].status && (pItemData[i].longPressed == 0))
      {
        pItemData[i].timer++;
        if(pItemData[i].timer > Button_LongPressCount)
        {
          pItemData[i].longPressed = pItemData[i].status;
          pItemData[i].longPressedJump = pItemData[i].status;
          pItemData[i].timer = 0;
        }
      }
    }
		
		pItemData[i].lastStatus = pItemData[i].status;
	}
}

Button_ItemData_t* Button_getItemData(Button_Item_t item)
{
  if(item < BUTTON_ITEM_MAX && item >= 0)
  {
    return &Button_ItemDataList[item];
  }
  else
  {
    return &Button_ItemDataList[0];
  }
}

/* Private functions ---------------------------------------------------------*/



