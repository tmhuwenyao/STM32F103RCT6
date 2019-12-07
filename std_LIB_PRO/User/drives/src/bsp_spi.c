#include "bsp_spi.h"
#include "bsp_usart.h"

/*
**************************SPI1引脚定义*************************
**SPI_NSS:			PA4
**SPI_CLK:			PA5
**SPI_MISO:			PA6
**SPI_MOSI:			PA7
***********************************************************/

/*
**************************SPI2引脚定义*************************
**SPI_NSS:			PB12
**SPI_CLK:			PB13
**SPI_MISO:			PB14
**SPI_MOSI:			PB15
***********************************************************/

/******************************************************
**函数功能：SPI初始化
*******************************************************/
void SPI1_InitConfig(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
	// PA5 时钟  PA7 数据
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	

	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 			
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 	
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		 	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		  
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  			
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	// Enable SPI1  
	SPI_Cmd(SPI1, ENABLE);
	

	
}
/*调用库函数进项写操作*/
#if 1
void SPI1_WriterByte(u8 dat)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	// 通过 SPI2发送一字节数据 
	SPI_I2S_SendData(SPI1, dat);	
}

#else
/*不调用库函数进项写操作*/
void SPI1_WriterByte(uint8_t data)
{	
	uint8_t i;			  
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_L();
		if(data&0x80)
		{
			OLED_SDIN_H();
		}
		else
			OLED_SDIN_L();
		OLED_SCLK_H();
		data <<= 1;   
	}
}
#endif

/**
  * @name  SPI2_Init
  * @param  none
  * @retval none
  * @brief  SPI2初始化
  */
void SPI2_Init(void)
{

	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_RC522|RCC_RC522_RST|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //使能SPI2时钟
	// PB12 片选 PB13 时钟  PB14 MOSI  PB15 MISO
	GPIO_InitStructure.GPIO_Pin = RC522_PIN_NSS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_Init(RC522_PORT, &GPIO_InitStructure);  
		
	GPIO_InitStructure.GPIO_Pin = RC522_PIN_CLK |RC522_PIN_MOSI|RC522_PIN_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //复用推挽输出
	GPIO_Init(RC522_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RC522_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_Init(RC522_RST_PORT, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //全双工模式
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 			//主设备模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 		//8字节数据通信
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 			//时钟空闲时为低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;				//时钟奇数边沿采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		  			//软件控制NSS位
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;			//通信速率设定，波特率预分频值
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  			//数据传输从MSB开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;						//CRC计算多项式
	SPI_Init(SPI2, &SPI_InitStructure);								//初始化
	
	SPI_Cmd(SPI2, ENABLE);	//使能SPI2


/*
	GPIOB->CRH&=0X000FFFFF;		 //PB.13 PB.14 PB.15
	GPIOB->CRH|=0XB8B00000;

	RCC->APB1ENR|=1<<14;

	SPI2->CR1|=0<<10;//全双工模式	
	SPI2->CR1|=1<<9; //软件nss管理
	SPI2->CR1|=1<<8;  
	
	SPI2->CR1|=1<<2; //SPI主机
	SPI2->CR1|=0<<11;//8bit数据格式	
	SPI2->CR1&=~0x02;//空闲模式下SCK为0 CPOL=0
	SPI2->CR1&=~0x01;//数据采样从第一个时间边沿开始,CPHA=0  
	SPI2->CR1|=7<<3; //Fsck=Fcpu/4
	SPI2->CR1|=0<<7; //MSBfirst   
	SPI2->CR1|=1<<6; //SPI设备使能
	
*/	

}
/**
  * @name  SPI2WriteByte
  * @param  写入的数据
  * @retval 成功/失败
  * @brief  写数据 1byte
  */
uint8_t SPI2WriteByte(uint8_t Byte)
{
	/*
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	// 通过 SPI2发送一字节数据 
	SPI_I2S_SendData(SPI2, Byte);
	return Byte;  //返回收到的数据		
*/	
	while((SPI2->SR&0X02)==0);		//等待发送区空	  
	SPI2->DR=Byte;	 							//发送一个byte   
	while((SPI2->SR&0X01)==0);		//等待接收完一个byte  
	return SPI2->DR;          	  //返回收到的数据
	
}


