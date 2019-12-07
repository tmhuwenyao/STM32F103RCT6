#ifndef __BSP_IIC_H
#define __BSP_IIC_H

#include "stm32f10x.h"


#define GPIO_PORT_I2C	GPIOB			/* GPIO�˿� */
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define I2C_SCL_PIN		GPIO_Pin_6			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		GPIO_Pin_7			/* ���ӵ�SDA�����ߵ�GPIO */

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */


void I2C_GPIOInitConfig(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendACK(u8 ack);
u8 I2C_RecvACK(void);

void I2C_SendByte(u8 dat);
u8 I2C_RecvByte(void);


uint8_t I2C_CheckDevice(uint8_t _Address);

#endif

