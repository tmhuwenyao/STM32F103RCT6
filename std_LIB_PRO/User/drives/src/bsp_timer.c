#include "bsp_timer.h"
uint8_t tim2_1s_flag = 0;

/*1ms 产生一次中断*/
static void TIM2_Config()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7199; //预分频系数，系统默认的频率为72M，计数频率72MHz/(7199+1) = 10Khz  
	TIM_TimeBaseInitStruct.TIM_Period = 9;  //定时1ms的计数,每1ms产生一次中断
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //开启TIM2中断
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);//情况事件更新标志位
	TIM_Cmd(TIM2,ENABLE);
	
}

static void TIM2_NVIC_InitConfig()
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}
void TIM2_InitConfig(void)
{
	TIM2_Config();
	TIM2_NVIC_InitConfig();
}

