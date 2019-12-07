#include "bsp_key.h"

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* �����ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_EXTI_IRQ ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void KEY_EXTI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;    //����Ҫ���ں����Ŀ�ͷ
	EXTI_InitTypeDef EXTI_InitStructure;	//����Ҫ���ں����Ŀ�ͷ
	//����������ʱ�Ӻ͸��ù���
	RCC_APB2PeriphClockCmd(RCC_KEY_EXTI,ENABLE);
	//�����������ã���������Ϊ��������
	
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);
	
	//�����ж�����
	
	//���ð���1
	GPIO_EXTILineConfig(KEY_SOURCE_PORT, KEY1_SOURCE_PIN); 
	EXTI_InitStructure.EXTI_Line = KEY1_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�½����ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	//���ð���2
	GPIO_EXTILineConfig(KEY_SOURCE_PORT, KEY2_SOURCE_PIN); 
	EXTI_InitStructure.EXTI_Line = KEY2_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�½����ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_Configuration();
}

