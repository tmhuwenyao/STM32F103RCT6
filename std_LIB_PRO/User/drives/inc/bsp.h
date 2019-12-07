#ifndef __BSP_H
#define __BSP_H

#include "stm32f10x.h"

#include "bsp_usart.h"
#include "bsp_led.h"  
#include "bsp_can.h"
#include "bsp_systick.h"
#include "bsp_key.h"
#include "bsp_timer.h"
#include "bsp_spi.h"
#include "bsp_iic.h"

#include "dht11.h"
#include "rc522.h"
#include "oleddisplay.h"
#include "externalcomm.h"
#include "eeprom.h"

extern void bsp_init(void);

#endif

