#ifndef _BSP_LED_H_
#define _BSP_LED_H_

#include "stm32f10x.h"

#include <stdio.h>

#define LED_PORT 	GPIOB
#define LED_PIN_GREEN 	GPIO_Pin_8
#define LED_PIN_RED		GPIO_Pin_9
#define RCC_LED		RCC_APB2Periph_GPIOB

/*门禁指示灯*/
#define DOOR_LED_PORT 	GPIOA
#define DOOR_LED_PIN 	GPIO_Pin_8
#define RCC_DOOR_LED	RCC_APB2Periph_GPIOA

typedef enum LED{
	GREEN = 0,
	RED,
	YELLOW,
}LED_TYPE;

enum{
	LED_OFF = 0,
	LED_ON,
	LED_TOGGLE,  //翻转
};

enum{
	FAIL = 0,
	TRUE = 1,
};

enum{
	OFF = 0,
	ON,
};
extern uint8_t LED_SetStatus(LED_TYPE LED,uint8_t Status);
extern void LED_Init(void);

extern void DO_InitConfig(void);
extern void DO_Output(uint8_t status);
	

#endif

