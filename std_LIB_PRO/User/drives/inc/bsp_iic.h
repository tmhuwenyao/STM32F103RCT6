#ifndef __BSP_IIC_H
#define __BSP_IIC_H

#include "stm32f10x.h"


#define GPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define I2C_SCL_PIN		GPIO_Pin_6			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN		GPIO_Pin_7			/* 连接到SDA数据线的GPIO */

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */


void I2C_GPIOInitConfig(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendACK(u8 ack);
u8 I2C_RecvACK(void);

void I2C_SendByte(u8 dat);
u8 I2C_RecvByte(void);


uint8_t I2C_CheckDevice(uint8_t _Address);

#endif

