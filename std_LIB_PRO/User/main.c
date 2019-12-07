/*—ßœ∞ø™∑¢≥Ã–Ú STM32F103RCT6
	HOO~~
*/
/*****************∞Ê±æ∫≈£¨º«µ√–ﬁ∏ƒ±∏∑*************************/
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
//	OLED_Test();	//œ‘ æ∆¡≤‚ ‘≥Ã–Ú
	eeprom_Test();  //eeprom≤‚ ‘≥Ã–Ú
	LED_SetStatus(GREEN,LED_OFF);
	while (1)
	{
		Monitor_CommProcess();	//∫ÛÃ®º‡øÿ≤Ÿ◊˜
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
		if(tim2_1s_flag == 1)   //∂® ±∆˜1s
		{
			tim2_1s_flag = 0; //1s∂® ±±Í÷æ«Â0
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

