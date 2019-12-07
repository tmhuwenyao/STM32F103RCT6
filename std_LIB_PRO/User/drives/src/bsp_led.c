#include "bsp_led.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_LED | RCC_DOOR_LED |RCC_APB2Periph_GPIOA, ENABLE);
	/*初始化电源指示灯*/
	GPIO_InitStructure.GPIO_Pin =  LED_PIN_GREEN | LED_PIN_RED ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
	/*初始化门禁指示灯*/
	GPIO_InitStructure.GPIO_Pin =  DOOR_LED_PIN |GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DOOR_LED_PORT, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
}
/**
  * @name  LED_SetStatus
  * @param  灯的型号，状态
  * @retval 成功/失败
  * @brief  LED状态设置
  */
uint8_t LED_SetStatus(LED_TYPE LED,uint8_t Status)
{
	uint8_t ret = FAIL;
	switch(LED)
	{
		case GREEN:
			if(Status == LED_OFF)
				GPIO_SetBits(LED_PORT, LED_PIN_GREEN);  //设为高电平，关闭LED灯
			else if(Status == LED_ON)
				GPIO_ResetBits(LED_PORT, LED_PIN_GREEN);
			else if(Status == LED_TOGGLE)
				LED_PORT->ODR ^= LED_PIN_GREEN;    //电平翻转 
			ret = TRUE;
			break;
		case RED:
			if(Status == LED_OFF)
				GPIO_SetBits(LED_PORT, LED_PIN_RED);  //设为高电平，关闭LED灯
			else if(Status == LED_ON)
				GPIO_ResetBits(LED_PORT, LED_PIN_RED);
			else if(Status == LED_TOGGLE)
				LED_PORT->ODR ^= LED_PIN_RED;    //电平翻转 
			ret = TRUE;
			break;
		case YELLOW:
			if(Status == LED_OFF)
				GPIO_SetBits(DOOR_LED_PORT, DOOR_LED_PIN);  //设为高电平，关闭LED灯
			else if(Status == LED_ON)
				GPIO_ResetBits(DOOR_LED_PORT, DOOR_LED_PIN);
			else if(Status == LED_TOGGLE)
				DOOR_LED_PORT->ODR ^= DOOR_LED_PIN;    //电平翻转 
			ret = TRUE;
			break;
		default :
			break;
	}
	return ret;
}

//PWM输出实验

