#include "bsp_led.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_LED | RCC_DOOR_LED |RCC_APB2Periph_GPIOA, ENABLE);
	/*��ʼ����Դָʾ��*/
	GPIO_InitStructure.GPIO_Pin =  LED_PIN_GREEN | LED_PIN_RED ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
	/*��ʼ���Ž�ָʾ��*/
	GPIO_InitStructure.GPIO_Pin =  DOOR_LED_PIN |GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DOOR_LED_PORT, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
}
/**
  * @name  LED_SetStatus
  * @param  �Ƶ��ͺţ�״̬
  * @retval �ɹ�/ʧ��
  * @brief  LED״̬����
  */
uint8_t LED_SetStatus(LED_TYPE LED,uint8_t Status)
{
	uint8_t ret = FAIL;
	switch(LED)
	{
		case GREEN:
			if(Status == LED_OFF)
				GPIO_SetBits(LED_PORT, LED_PIN_GREEN);  //��Ϊ�ߵ�ƽ���ر�LED��
			else if(Status == LED_ON)
				GPIO_ResetBits(LED_PORT, LED_PIN_GREEN);
			else if(Status == LED_TOGGLE)
				LED_PORT->ODR ^= LED_PIN_GREEN;    //��ƽ��ת 
			ret = TRUE;
			break;
		case RED:
			if(Status == LED_OFF)
				GPIO_SetBits(LED_PORT, LED_PIN_RED);  //��Ϊ�ߵ�ƽ���ر�LED��
			else if(Status == LED_ON)
				GPIO_ResetBits(LED_PORT, LED_PIN_RED);
			else if(Status == LED_TOGGLE)
				LED_PORT->ODR ^= LED_PIN_RED;    //��ƽ��ת 
			ret = TRUE;
			break;
		case YELLOW:
			if(Status == LED_OFF)
				GPIO_SetBits(DOOR_LED_PORT, DOOR_LED_PIN);  //��Ϊ�ߵ�ƽ���ر�LED��
			else if(Status == LED_ON)
				GPIO_ResetBits(DOOR_LED_PORT, DOOR_LED_PIN);
			else if(Status == LED_TOGGLE)
				DOOR_LED_PORT->ODR ^= DOOR_LED_PIN;    //��ƽ��ת 
			ret = TRUE;
			break;
		default :
			break;
	}
	return ret;
}

//PWM���ʵ��

