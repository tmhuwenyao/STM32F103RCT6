#ifndef __SMOKE_DETECT_H
#define __SMOKE_DETECT_H

#include "stm32f10x.h"
//烟感告警
//#define SMOKE_PORT 	GPIOA
//#define SMOKE_PIN 	GPIO_Pin_15
//#define RCC_SMOKE	RCC_APB2Periph_GPIOA
//蜂鸣器RCC_APB2Periph_GPIOA
#define BEEP_PORT 	GPIOA
#define BEEP_PIN 	GPIO_Pin_15
#define RCC_BEEP	RCC_APB2Periph_GPIOA

/*检测引脚输出低电平*/
//#define SMOKE_DETECT_ALARM	(GPIO_ReadInputDataBit(SMOKE_PORT, SMOKE_PIN) == 0)		

/*检测引脚输出低电平*/
//#define SMOKE_DETECT_CLEAR	(GPIO_ReadInputDataBit(SMOKE_PORT, SMOKE_PIN) == 1)

#define BEEP_ON				(GPIO_SetBits(BEEP_PORT, BEEP_PIN))

#define BEEP_OFF			(GPIO_ResetBits(BEEP_PORT, BEEP_PIN))


extern uint8_t smoke_alarm_flag;
extern void SMOKE_Alarm_Init(void);
void SMOKE_Alarm_Detect(void); //烟感告警处理方式
#endif

