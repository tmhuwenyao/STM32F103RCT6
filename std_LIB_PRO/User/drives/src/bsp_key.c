#include "bsp_key.h"

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQ ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void KEY_EXTI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;    //声明要放在函数的开头
	EXTI_InitTypeDef EXTI_InitStructure;	//声明要放在函数的开头
	//开启按键的时钟和复用功能
	RCC_APB2PeriphClockCmd(RCC_KEY_EXTI,ENABLE);
	//按键引脚配置，配置引脚为上拉输入
	
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);
	
	//按键中断配置
	
	//配置按键1
	GPIO_EXTILineConfig(KEY_SOURCE_PORT, KEY1_SOURCE_PIN); 
	EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	//配置按键2
	GPIO_EXTILineConfig(KEY_SOURCE_PORT, KEY2_SOURCE_PIN); 
	EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_Configuration();
}

