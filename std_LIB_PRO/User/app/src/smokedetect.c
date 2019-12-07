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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
	
	BEEP_OFF;  //��ʼ���رշ�����
}

//�̸и澯��ʼ��
void SMOKE_Alarm_Init(void)
{
//	SmokeDetect_Config();
	BEEP_Config();
}
#define BEEP_ON_DELAY	5
//�̸и澯���,1Sִ��һ��
void SMOKE_Alarm_Detect(void)
{
	static uint8_t alarm_count ;
	if(1)
	{
		if(alarm_count ++ >= BEEP_ON_DELAY)
		{
			smoke_alarm_flag = 1;
			BEEP_ON;//��������
			LED_SetStatus(RED,LED_ON); //�򿪱�����
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
			smoke_alarm_flag = 0;   //�޸澯
			LED_SetStatus(RED,LED_OFF); //�رձ�����
			BEEP_OFF;
		}

	}
//	printf("the alarm_count is %d\n\r",alarm_count);
}

