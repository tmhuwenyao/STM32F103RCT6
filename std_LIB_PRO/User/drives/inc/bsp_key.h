#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "stm32f10x.h"

/*外部中断EXIT相关宏定义*/
#define             RCC_KEY_EXTI                        (RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO)     //引脚作为中断，要开启复用功能
#define             KEY_PORT                      	 	GPIOB  
#define             KEY1_PIN                      		GPIO_Pin_5   	//KEY1 PB5
#define             KEY2_PIN                        	GPIO_Pin_4		//KEY2 PB4

#define             KEY_SOURCE_PORT                     GPIO_PortSourceGPIOB
//KEY1-EXTI
#define             KEY1_SOURCE_PIN                     GPIO_PinSource5
#define             KEY1_EXTI_LINE                      EXTI_Line5

//KEY2-EXTI
#define             KEY2_SOURCE_PIN                     GPIO_PinSource4
#define             KEY2_EXTI_LINE                      EXTI_Line4
//KEY中断处理
#define             KEY1_EXTI_IRQ                        EXTI9_5_IRQn
#define             KEY1_EXTI_FUNCTION                   EXTI9_5_IRQHandler

#define             KEY2_EXTI_IRQ                        EXTI4_IRQn
#define             KEY2_EXTI_FUNCTION                   EXTI4_IRQHandler

extern void KEY_EXTI_Config(void);

#endif

