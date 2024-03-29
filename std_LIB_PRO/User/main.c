/*学习开发程序 STM32F103RCT6
	HOO~~
*/
/*****************版本号，记得修改备�*************************/
#define VERSION_MAIN	0
#define VERSION_DELV	3
/*************************************************************/
#include "bsp.h"

int main(void)
{	
	bsp_init();
	printf("this is  JIE.CAI -->> STM32 test program....\n\r");
	printf("DEVELOP VERSION IS :V.%d.%d\n\r",VERSION_MAIN,VERSION_DELV);

//	RC522_Test();
//	OLED_Test();	//显示屏测试程序
	eeprom_Test();  //eeprom测试程序
	LED_SetStatus(GREEN,LED_OFF);
	while (1)
	{
		Monitor_CommProcess();	//后台监控操作
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
		if(tim2_1s_flag == 1)   //定时器1s
		{
			tim2_1s_flag = 0; //1s定时标志清0
			LED_SetStatus(GREEN,LED_TOGGLE);
			LED_SetStatus(RED,LED_TOGGLE);
			dht11_read_data(&humi_i,&humi_f,&temp_i,&temp_f);
		//	printf("the humility is %d.%d,the temperature is %d.%d\n\r",humi_i,humi_f,temp_i,temp_f);
		//	SMOKE_Alarm_Detect();
		}
	//	GPIO_ResetBits(GPIOA, GPIO_Pin_All);
		GPIO_SetBits(GPIOA, GPIO_Pin_All);
		LED_SetStatus(YELLOW,LED_ON);
	}
}

