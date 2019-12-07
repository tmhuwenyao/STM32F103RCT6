#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include "stm32f10x.h"
void SYSTICK_InitConfig(uint8_t SYSCLK);
void delay_ms(uint16_t mtime);
void delay_us(uint32_t utime);


#endif

