#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f10x.h"


#define SPI1_BUFF_MAX	512

#define SPI2ReadByte()	SPI2WriteByte(0)

/*SPI1*/
extern uint8_t SPI1_Send_Data;
extern uint8_t SPI1_Read_Data;
extern void SPI1_InitConfig(void);
extern uint8_t SPI1_RW(uint8_t dat);
extern void SPI_Device_Init(void);
extern void SPI1_WriterByte(uint8_t dat);
/*SPI2 用于RC522数据交换*/
#define RC522_PORT 		GPIOB
#define RC522_PIN_NSS 	GPIO_Pin_12
#define RC522_PIN_CLK 	GPIO_Pin_13
#define RC522_PIN_MOSI 	GPIO_Pin_14
#define RC522_PIN_MISO 	GPIO_Pin_15
#define RCC_RC522		RCC_APB2Periph_GPIOB	

#define RC522_RST_PORT	GPIOC
#define RC522_RST_PIN	GPIO_Pin_6
#define RCC_RC522_RST	RCC_APB2Periph_GPIOC
#define RC522_RST_L()	GPIO_ResetBits(RC522_RST_PORT, RC522_RST_PIN)		//PC6- RC522 RESET
#define RC522_RST_H()	GPIO_SetBits(RC522_RST_PORT, RC522_RST_PIN)

#define RC522_CS_L()	GPIO_ResetBits(RC522_PORT, RC522_PIN_NSS)	//PB12-  CS NSS
#define RC522_CS_H()	GPIO_SetBits(RC522_PORT, RC522_PIN_NSS)

#define RC522_SCLK_L()	GPIO_ResetBits(RC522_PORT, RC522_PIN_CLK)	//PB13-CLK
#define RC522_SCLK_H()	GPIO_SetBits(RC522_PORT, RC522_PIN_CLK)

#define RC522_MOSI_L()	GPIO_ResetBits(RC522_PORT, RC522_PIN_MOSI)	//PB14-MOSI
#define RC522_MOSI_H()	GPIO_SetBits(RC522_PORT, RC522_PIN_MOSI)

#define RC522_MISO_L()	GPIO_ResetBits(RC522_PORT, RC522_PIN_MISO)	//PB15- MISO
#define RC522_MISO_H()	GPIO_SetBits(RC522_PORT, RC522_PIN_MISO)

extern void SPI2_Init(void);
extern uint8_t SPI2WriteByte(u8 Byte);

/*********************************************************/




#endif

