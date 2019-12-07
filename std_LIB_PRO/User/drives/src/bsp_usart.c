/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  HU WENYAO
  * @version V1.0
  * @date    2019-04-21
  * @brief   ����1���պ���
  ******************************************************************************
  */ 
#include "bsp_usart.h"
#include "externalcomm.h"
/* ����ÿ�����ڽṹ����� */
uint16_t modbus_recv_gap_flag = 0; //����3.5���ַ���ʱ����������֡���ݣ�2ms����
UART_T g_tUart1;
static uint8_t g_TxBuf1[TX_BUF_SIZE];		/* ���ͻ����� */
static uint8_t g_RxBuf1[RX_BUF_SIZE];		/* ���ջ����� */
/*
*********************************************************************************************************
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartVarInit(void)
{
	g_tUart1.uart = USART1;						/* STM32 �����豸 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = TX_BUF_SIZE;			/* ���ͻ�������С */
	g_tUart1.usRxBufSize = RX_BUF_SIZE;			/* ���ջ�������С */
	g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart1.ReciveNew = MODS_ReceieveNew;		/* ���յ������ݺ�Ļص����� */
}
/*�����ն˺�������*/
void NVIC_UsartConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
 /**
  * @brief  USARTx GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
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
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   //�жϺ�������ʹ��
	USART_Cmd(USART1, ENABLE);
	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
	�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
	//ʹ�ܴ����ն�
	NVIC_UsartConfig(); 
	/*������ر�����ʼ��*/
	UartVarInit();
}


/// �ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{		
	#if	USART_PRINTF_ENABLE
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	#endif
		return (ch);
}

/// �ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

//���һ���ַ�
static void USART1_Putchar(UART_T *_pUart,uint8_t c)
{
	USART_SendData(_pUart->uart, c);
	while(USART_GetFlagStatus(_pUart->uart, USART_FLAG_TC)!=SET);
}
//���һ���ַ���
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

/*���ڽ����жϺ���*/
void UartIRQ(UART_T *_pUart)
{
	uint8_t ch = 0;
	if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(_pUart->uart,USART_IT_RXNE);
		ch = USART_ReceiveData(_pUart->uart);
		_pUart->usRxWrite =_pUart->usRxWrite % RX_BUF_SIZE;
		_pUart->pRxBuf[(_pUart->usRxWrite ++)] = ch;
		_pUart->usRxCount ++;		//�����ַ��ĳ���
		modbus_recv_gap_flag = 0;   //�ñ�־λ1����ʾһ֡���ݽ������
	//	_pUart->ReciveNew(data);		//�ú���ִ�������⣬��֪��Ϊʲô
	}
}


void MODS_ReceieveNewData(UART_T *_pUart)
{
	uint8_t i,frame_start = 0;
	if((_pUart->usRxCount < 8) ||(modbus_recv_gap_flag == 0))  //���յ��ַ����ݳ���С��8,����δ���ܵ�һ������֡
	{
		return ;
	}
	
	frame_start = (_pUart->usRxWrite - _pUart->usRxCount + RX_BUF_SIZE) % RX_BUF_SIZE;
	for( i= 0;i < _pUart->usRxCount;i++)
	{
		g_tModS.RxBuf[g_tModS.RxCount++] = _pUart->pRxBuf[(frame_start + i) % RX_BUF_SIZE] ;
	}
	_pUart->usRxCount =0 ;//������һ֡���ݣ����������ݳ������㡣
	
}






