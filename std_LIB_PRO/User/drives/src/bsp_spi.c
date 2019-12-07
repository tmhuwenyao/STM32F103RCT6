#include "bsp_spi.h"
#include "bsp_usart.h"

/*
**************************SPI1���Ŷ���*************************
**SPI_NSS:			PA4
**SPI_CLK:			PA5
**SPI_MISO:			PA6
**SPI_MOSI:			PA7
***********************************************************/

/*
**************************SPI2���Ŷ���*************************
**SPI_NSS:			PB12
**SPI_CLK:			PB13
**SPI_MISO:			PB14
**SPI_MOSI:			PB15
***********************************************************/

/******************************************************
**�������ܣ�SPI��ʼ��
*******************************************************/
void SPI1_InitConfig(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
	// PA5 ʱ��  PA7 ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
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
/*���ÿ⺯������д����*/
#if 1
void SPI1_WriterByte(u8 dat)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	// ͨ�� SPI2����һ�ֽ����� 
	SPI_I2S_SendData(SPI1, dat);	
}

#else
/*�����ÿ⺯������д����*/
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
  * @brief  SPI2��ʼ��
  */
void SPI2_Init(void)
{

	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_RC522|RCC_RC522_RST|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //ʹ��SPI2ʱ��
	// PB12 Ƭѡ PB13 ʱ��  PB14 MOSI  PB15 MISO
	GPIO_InitStructure.GPIO_Pin = RC522_PIN_NSS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_Init(RC522_PORT, &GPIO_InitStructure);  
		
	GPIO_InitStructure.GPIO_Pin = RC522_PIN_CLK |RC522_PIN_MOSI|RC522_PIN_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   //�����������
	GPIO_Init(RC522_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RC522_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //�������
	GPIO_Init(RC522_RST_PORT, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ȫ˫��ģʽ
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 			//���豸ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 		//8�ֽ�����ͨ��
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 			//ʱ�ӿ���ʱΪ�͵�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;				//ʱ���������ز���
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		  			//�������NSSλ
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;			//ͨ�������趨��������Ԥ��Ƶֵ
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  			//���ݴ����MSB��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;						//CRC�������ʽ
	SPI_Init(SPI2, &SPI_InitStructure);								//��ʼ��
	
	SPI_Cmd(SPI2, ENABLE);	//ʹ��SPI2


/*
	GPIOB->CRH&=0X000FFFFF;		 //PB.13 PB.14 PB.15
	GPIOB->CRH|=0XB8B00000;

	RCC->APB1ENR|=1<<14;

	SPI2->CR1|=0<<10;//ȫ˫��ģʽ	
	SPI2->CR1|=1<<9; //���nss����
	SPI2->CR1|=1<<8;  
	
	SPI2->CR1|=1<<2; //SPI����
	SPI2->CR1|=0<<11;//8bit���ݸ�ʽ	
	SPI2->CR1&=~0x02;//����ģʽ��SCKΪ0 CPOL=0
	SPI2->CR1&=~0x01;//���ݲ����ӵ�һ��ʱ����ؿ�ʼ,CPHA=0  
	SPI2->CR1|=7<<3; //Fsck=Fcpu/4
	SPI2->CR1|=0<<7; //MSBfirst   
	SPI2->CR1|=1<<6; //SPI�豸ʹ��
	
*/	

}
/**
  * @name  SPI2WriteByte
  * @param  д�������
  * @retval �ɹ�/ʧ��
  * @brief  д���� 1byte
  */
uint8_t SPI2WriteByte(uint8_t Byte)
{
	/*
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	// ͨ�� SPI2����һ�ֽ����� 
	SPI_I2S_SendData(SPI2, Byte);
	return Byte;  //�����յ�������		
*/	
	while((SPI2->SR&0X02)==0);		//�ȴ���������	  
	SPI2->DR=Byte;	 							//����һ��byte   
	while((SPI2->SR&0X01)==0);		//�ȴ�������һ��byte  
	return SPI2->DR;          	  //�����յ�������
	
}


