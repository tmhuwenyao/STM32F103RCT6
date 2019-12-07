#include "smokedetect.h"
#include "bsp_usart.h"
#include "bsp_led.h"
uint8_t smoke_alarm_flag = 0;

static void BEEP_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_BEEP , ENABLE);
	GPIO_InitStructure.GPIO_Pin =  BEEP_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
	
	BEEP_OFF;  //初始化关闭蜂鸣器
}

//烟感告警初始化
void SMOKE_Alarm_Init(void)
{
//	SmokeDetect_Config();
	BEEP_Config();
}
#define BEEP_ON_DELAY	5
//烟感告警检测,1S执行一次
void SMOKE_Alarm_Detect(void)
{
	static uint8_t alarm_count ;
	if(1)
	{
		if(alarm_count ++ >= BEEP_ON_DELAY)
		{
			smoke_alarm_flag = 1;
			BEEP_ON;//开报警器
			LED_SetStatus(RED,LED_ON); //打开报警灯
			alarm_count = BEEP_ON_DELAY;
		}
	}
	else
	{
		if(alarm_count > 0)
		{
			alarm_count --;
			
		}
		else if(alarm_count == 0)
		{
			smoke_alarm_flag = 0;   //无告警
			LED_SetStatus(RED,LED_OFF); //关闭报警灯
			BEEP_OFF;
		}

	}
//	printf("the alarm_count is %d\n\r",alarm_count);
}

