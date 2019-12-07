#include "bsp.h"

void bsp_init(void)
{
	SYSTICK_InitConfig(72);  //ϵͳ�δ�ʱ�ӳ�ʼ��,ϵͳʱ��Ϊ72M,���72M��ô���ģ� system_stm32f10x.c�ļ��� #define SYSCLK_FREQ_72MHz  72000000
	USART1_Init_Config();		//����1��ʼ��
	I2C_GPIOInitConfig();	//��ʼ��IIC
	
	LED_Init();				//LED�Ƴ�ʼ��
	KEY_EXTI_Config();		//�ⲿ������ʼ��
	TIM2_InitConfig();		//��ʱ��2��ʼ��
		
	OLED_Init();		  	 //OLED����ʼ��
	DHT11_InitConfig();    	//��ʪ�ȴ�������ʼ��

	RC522_Init();			//��ʼ��SPI2������RC522ͨ��
}

