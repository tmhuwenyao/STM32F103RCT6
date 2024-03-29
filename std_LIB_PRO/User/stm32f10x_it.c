/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */
  
/*记得 stm32f10x_conf.h 里面的用的配置的头文件要打开*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_timer.h"
extern __IO uint32_t ret;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}
/*串口1接收中断函数*/
void USART1_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}
/*
 * 函数名：USB_LP_CAN1_RX0_IRQHandler
 * 描述  ：USB中断和CAN接收中断服务程序，USB跟CAN公用I/O，这里只用到CAN的中断。
 * 输入  ：无
 * 输出  : 无	 
 * 调用  ：无
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	
	RxMessage.StdId=0x00;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.DLC=0;
	RxMessage.FMI=0;
	RxMessage.Data[0]=0x00;
	RxMessage.Data[1]=0x00;
	
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	if((RxMessage.ExtId==0x1234)  && 
	   (RxMessage.IDE==CAN_ID_EXT)&& 
	   (RxMessage.DLC==2)         && 
	   ((RxMessage.Data[1]|RxMessage.Data[0]<<8)==0xDECA))
	{
		ret = 1; 
	}
	else
	{
	ret = 0; 
	}
}
/*按键外部中断函数处理*/
void KEY_EXTI_FUNCTION()
{
	if(EXTI_GetITStatus(KEY1_EXTI_LINE) != RESET)
	{
		LED_SetStatus(RED,LED_TOGGLE);
		printf("key1 is pressed\n\r");
		EXTI_ClearITPendingBit(KEY1_EXTI_LINE);
	}
	else if(EXTI_GetITStatus(KEY2_EXTI_LINE) != RESET)
	{
		LED_SetStatus(RED,LED_TOGGLE);
		printf("key2 is pressed\n\r");
		EXTI_ClearITPendingBit(KEY2_EXTI_LINE);
	}
	
}
/*定时器TIMER2执行*/
void TIM2_IRQHandler(void)
{
	static uint16_t tim2_tick_count = 0;
	static uint16_t modbus_recv_count = 0;
	//判断是否为TIM_IT_Update事件产生
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		/*1S定时标志位*/
		if(tim2_tick_count ++ == 1000)
		{
			tim2_1s_flag = 1 ;		//每隔1s将该标志至为1
			tim2_tick_count = 0;
		}
		
		//帧间隔判断
		if(modbus_recv_count ++ >= 10)
		{
			modbus_recv_gap_flag = 1;
			modbus_recv_count = 10;
		}

		//清空中断挂起位 TIM_IT_Update
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
