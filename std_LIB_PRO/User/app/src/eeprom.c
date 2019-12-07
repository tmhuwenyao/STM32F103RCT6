#include "eeprom.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "bsp_iic.h"

/*
*********************************************************************************************************
*	函 数 名: ee_CheckOk
*	功能说明: 判断串行EERPOM是否正常
*	形    参：无
*	返 回 值: 1 表示正常， 0 表示不正常
*********************************************************************************************************
*/
uint8_t eeprom_CheckOk(void)
{
	if (I2C_CheckDevice(EE_DEV_ADDR) == 0)
	{
		return 1;
	}
	else
	{
		/* 失败后，切记发送I2C总线停止信号 */
		I2C_Stop();		
		return 0;
	}
}
/******************************************************************
*	函 数 名: eeprom_ReadBytes
*	功能说明: 从串行EEPROM指定地址处开始读取若干数据
*	形    参：_usAddress : 起始地址
*			 _usSize : 数据长度，单位为字节
*			 _pReadBuf : 存放读到的数据的缓冲区指针
*	返 回 值: 0 表示失败，1表示成功
********************************************************************/
uint8_t eeprom_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i;
	
	/* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */
	
	/* 第1步：发起I2C总线启动信号 */
	I2C_Start();
	
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
//	I2C_SendByte(EE_DEV_ADDR | I2C_WR);	/* 此处是写指令 */
	I2C_SendByte((0XA0|((_usAddress/256)<<1))|I2C_WR);
//	I2C_SendByte((0XA0|(((_usAddress>>8)&0x01)<<1))|I2C_WR);
	
	/* 第3步：发送ACK */
	if (I2C_RecvACK() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
//	I2C_SendByte((uint8_t)_usAddress);
	I2C_SendByte((uint8_t)_usAddress%256);
//	I2C_SendByte((uint8_t)_usAddress&0xff);
	
	/* 第5步：发送ACK */
	if (I2C_RecvACK() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}
	
	/* 第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */
	I2C_Start();
	
	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
//	I2C_SendByte(EE_DEV_ADDR | I2C_RD);	/* 此处是读指令 */
	I2C_SendByte((0XA0|((_usAddress/256)<<1))|I2C_RD);
	/* 第8步：发送ACK */
	if (I2C_RecvACK() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}	
	
	/* 第9步：循环读取数据 */
	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = I2C_RecvByte();	/* 读1个字节 */
		
		/* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
		if (i != _usSize - 1)
		{
			I2C_SendACK(0);	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
		}
		else
		{
			I2C_SendACK(1);	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
		}
	}
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	printf("Read fild!\r\n");
	return 0;
}
/**********************************************************************************
*	函 数 名: eeprom_WriteBytes
*	功能说明: 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
*	形    参：_usAddress : 起始地址
*			 _usSize : 数据长度，单位为字节
*			 _pWriteBuf : 存放读到的数据的缓冲区指针
*	返 回 值: 0 表示失败，1表示成功
**********************************************************************************/
uint8_t eeprom_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
	uint16_t i,m;
	uint16_t usAddr;
	
	/* 
		写串行EEPROM不像读操作可以连续读取很多字节，每次写操作只能在同一个page。
		对于24xx02，page size = 8
		简单的处理方法为：按字节写操作模式，没写1个字节，都发送地址
		为了提高连续写的效率: 本函数采用page wirte操作。
	*/

	usAddr = _usAddress;	
	for (i = 0; i < _usSize; i++)
	{
		/* 当发送第1个字节或是页面首地址时，需要重新发起启动信号和地址 */
		if ((i == 0) || (usAddr & (EE_PAGE_SIZE - 1)) == 0)
		{
			/*　第０步：发停止信号，启动内部写操作　*/
			I2C_Stop();
			
			/* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms 			
				CLK频率为200KHz时，查询次数为30次左右
			*/
			for (m = 0; m < 1000; m++)
			{				
				/* 第1步：发起I2C总线启动信号 */
				I2C_Start();
				
				/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
				//i2c_SendByte(macEE_DEV_ADDR | macI2C_WR);	/* 此处是写指令 */
				I2C_SendByte(0XA0|I2C_WR);
				
				/* 第3步：发送一个时钟，判断器件是否正确应答 */
				if (I2C_RecvACK() == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail;	/* EEPROM器件写超时 */
			}
		
			/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
			I2C_SendByte((uint8_t)usAddr);
			
			/* 第5步：等待ACK */
			if (I2C_RecvACK() != 0)
			{
				goto cmd_fail;	/* EEPROM器件无应答 */
			}
		}
	
		/* 第6步：开始写入数据 */
		I2C_SendByte(_pWriteBuf[i]);
	
		/* 第7步：发送ACK */
		if (I2C_RecvACK() != 0)
		{
			goto cmd_fail;	/* EEPROM器件无应答 */
		}

		usAddr++;	/* 地址增1 */		
	}
	
	/* 命令执行成功，发送I2C总线停止信号 */
	I2C_Stop();
	return 1;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 0;
}
/**********************************************************************************
*	函 数 名: eeprom_Erase
*	功能说明: eeprom擦除
*	形    参：无
*	返 回 值: 无
**********************************************************************************/
void eeprom_Erase(void)
{
	uint16_t i;
	uint8_t buf[EE_SIZE];
	/* 填充缓冲区 */
	for (i = 0; i < EE_SIZE; i++)
	{
		buf[i] = 0xff;
	}
	/* 写EEPROM, 起始地址 = 0，数据长度为 512 */
	if (eeprom_WriteBytes(buf, 0, EE_SIZE) == 0)
	{
		printf("Erase eeprom error\r\n");
		return;
	}
	else
	{
		printf("Erase eeprom Success\r\n");
	}
}

uint8_t write_buf[EE_SIZE];  //读缓冲
uint8_t read_buf[EE_SIZE];   //写缓冲
/**********************************************************************************
*	函 数 名: eeprom_Test
*	功能说明: eeprom读写测试
*	形    参：无
*	返 回 值: 无
**********************************************************************************/
void eeprom_Test(void)
{
		uint16_t i;
	uint8_t buff[20] = {0XAA,0X3B,0X2B,0X4C,0X45,0X18,0X97,0XF1,0XE5,0X69,
						0X88,0X9D,0X3A,0X62,0XDE,0XCF,0X38,0X29,0X1A,0XBB};
	
	
	if(eeprom_CheckOk() == 0)
	{
		/* 没有检测到EEPROM */
		printf("Not check EEPROM!\r\n");

		while (1);	/* 停机 */
	}
  
	/* 填充测试缓冲区 */
	for (i = 0; i < EE_SIZE; i++)
	{		
		write_buf[i] = 1;
	}
	
  
	if (eeprom_WriteBytes(write_buf, 0, EE_SIZE) == 0)
	{
		printf("Write eeprom error\r\n");
		return;
	}
	else
	{		
		printf("Write eeprom Success！\r\n");
	}
	
//	eeprom_WriteBytes(buff, 0, 20);

//	eeprom_Erase();

	/*写完之后需要适当的延时再去读，不然会出错*/
	delay_ms(20);
	/*-----------------------------------------------------------------------------------*/
	if (eeprom_ReadBytes(read_buf, 0, EE_SIZE) == 0)
	{
		printf("Read eeprom error\r\n");
		return;
	}
	else
	{
		printf("Read  eeprom Success\r\n");
	}
	//打印EEPROM里的数据
	for (i = 0; i < EE_SIZE; i++)
	{
		printf("%X ", read_buf[i]);

		if ((i & 15) == 15)
		{
			printf("\r\n");	
		}
	}

}
