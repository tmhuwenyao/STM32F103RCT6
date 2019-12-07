/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  HU WENYAO
  * @version V1.0
  * @date    2019-04-21
  * @brief   串口1接收函数
  ******************************************************************************
  */ 
#include "bsp_usart.h"
#include "externalcomm.h"
/* 定义每个串口结构体变量 */
uint16_t modbus_recv_gap_flag = 0; //大于3.5个字符的时间间隔当做两帧数据，2ms左右
UART_T g_tUart1;
static uint8_t g_TxBuf1[TX_BUF_SIZE];		/* 发送缓冲区 */
static uint8_t g_RxBuf1[RX_BUF_SIZE];		/* 接收缓冲区 */
/*
*********************************************************************************************************
*	函 数 名: UartVarInit
*	功能说明: 初始化串口相关的变量
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void UartVarInit(void)
{
	g_tUart1.uart = USART1;						/* STM32 串口设备 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* 发送缓冲区指针 */
	g_tUart1.pRxBuf = g_RxBuf1;					/* 接收缓冲区指针 */
	g_tUart1.usTxBufSize = TX_BUF_SIZE;			/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = RX_BUF_SIZE;			/* 接收缓冲区大小 */
	g_tUart1.usTxWrite = 0;						/* 发送FIFO写索引 */
	g_tUart1.usTxRead = 0;						/* 发送FIFO读索引 */
	g_tUart1.usRxWrite = 0;						/* 接收FIFO写索引 */
	g_tUart1.usRxRead = 0;						/* 接收FIFO读索引 */
	g_tUart1.usRxCount = 0;						/* 接收到的新数据个数 */
	g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart1.ReciveNew = MODS_ReceieveNew;		/* 接收到新数据后的回调函数 */
}
/*串口终端函数配置*/
void NVIC_UsartConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* 使能串口1中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
 /**
  * @brief  USARTx GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
  
void USART1_Init_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	macUSART_APBxClock_FUN(macUSART_CLK, ENABLE);
	macUSART_GPIO_APBxClock_FUN(macUSART_GPIO_CLK, ENABLE);
		
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  macUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macUSART_TX_PORT, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = macUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macUSART_RX_PORT, &GPIO_InitStructure);	
			
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = macUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   //中断函数接收使能
	USART_Cmd(USART1, ENABLE);
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
	//使能串口终端
	NVIC_UsartConfig(); 
	/*串口相关变量初始化*/
	UartVarInit();
}


/// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{		
	#if	USART_PRINTF_ENABLE
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	#endif
		return (ch);
}

/// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

//输出一个字符
static void USART1_Putchar(UART_T *_pUart,uint8_t c)
{
	USART_SendData(_pUart->uart, c);
	while(USART_GetFlagStatus(_pUart->uart, USART_FLAG_TC)!=SET);
}
//输出一个字符串
void USART1_Senddata(UART_T *_pUart,uint8_t * str,uint16_t str_len)
{
	uint8_t i = 0;
	for(i = 0;i < str_len ;i++)
	{
		USART1_Putchar(_pUart,*(str + i));
	}
}
void MODS_SendData(UART_T *_pUart,uint8_t * str,uint16_t str_len)
{
	USART1_Senddata( _pUart, str, str_len);
}

/*串口接收中断函数*/
void UartIRQ(UART_T *_pUart)
{
	uint8_t ch = 0;
	if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(_pUart->uart,USART_IT_RXNE);
		ch = USART_ReceiveData(_pUart->uart);
		_pUart->usRxWrite =_pUart->usRxWrite % RX_BUF_SIZE;
		_pUart->pRxBuf[(_pUart->usRxWrite ++)] = ch;
		_pUart->usRxCount ++;		//接收字符的长度
		modbus_recv_gap_flag = 0;   //该标志位1，表示一帧数据接收完成
	//	_pUart->ReciveNew(data);		//该函数执行有问题，不知道为什么
	}
}


void MODS_ReceieveNewData(UART_T *_pUart)
{
	uint8_t i,frame_start = 0;
	if((_pUart->usRxCount < 8) ||(modbus_recv_gap_flag == 0))  //接收到字符数据长度小于8,或者未接受到一个完整帧
	{
		return ;
	}
	
	frame_start = (_pUart->usRxWrite - _pUart->usRxCount + RX_BUF_SIZE) % RX_BUF_SIZE;
	for( i= 0;i < _pUart->usRxCount;i++)
	{
		g_tModS.RxBuf[g_tModS.RxCount++] = _pUart->pRxBuf[(frame_start + i) % RX_BUF_SIZE] ;
	}
	_pUart->usRxCount =0 ;//接收完一帧数据，将接收数据长度清零。
	
}






