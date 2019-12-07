#include "bsp.h"

void bsp_init(void)
{
	SYSTICK_InitConfig(72);  //系统滴答时钟初始化,系统时钟为72M,这个72M怎么来的？ system_stm32f10x.c文件中 #define SYSCLK_FREQ_72MHz  72000000
	USART1_Init_Config();		//串口1初始化
	I2C_GPIOInitConfig();	//初始化IIC
	
	LED_Init();				//LED灯初始化
	KEY_EXTI_Config();		//外部按键初始化
	TIM2_InitConfig();		//定时器2初始化
		
	OLED_Init();		  	 //OLED屏初始化
	DHT11_InitConfig();    	//温湿度传感器初始化

	RC522_Init();			//初始化SPI2，用于RC522通信
}

