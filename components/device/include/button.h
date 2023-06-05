/**
 ******************************************************************************
 * @file           Button.h
 * @description:   
 * @author:        Hare
 * @date:          2022-10-16
 * @version:       v0.1
 * @history: 
 *    [version]   [date]       [author]       [description]
 *    v0.1   	    2022-10-16   Hare           create
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Defines -------------------------------------------------------------------*/
#define BUTTON_USERBTN_PIN           		22
#define BUTTON_USERBTN_PIN2           	19

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BUTTON_PIN_USERBTN = 0,
	BUTTON_PIN_RESTBTN,
  BUTTON_PINLIST_MAX
} Button_PinList_t;

typedef enum
{
  BUTTON_USERBTN = 0,
	BUTTON_RESTBTN, 
  BUTTON_ITEM_MAX,
} Button_Item_t;

typedef struct{
	uint8_t status : 1;
	uint8_t lastStatus : 1;
	uint8_t pressedJump : 1;
	uint8_t releaseJump : 1;
	uint8_t longPressed : 1;
  uint8_t longPressedJump : 1;
  uint8_t longReleaseJump : 1;
	uint8_t reserve : 1;
	uint16_t timer;
} Button_ItemData_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void Button_init(uint16_t processTimeMs);
void Button_process(void);
Button_ItemData_t* Button_getItemData(Button_Item_t item);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */
