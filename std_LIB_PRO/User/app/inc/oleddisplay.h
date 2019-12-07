#ifndef __OLED_DISPLAY_H
#define __OLED_DISPLAY_H

#include "stm32f10x.h"

#define OLED_DC_Set()	
#define OLED_DC_L()		GPIO_ResetBits(GPIOA, GPIO_Pin_6)	//PA6-  DADA CMD 
#define OLED_DC_H()		GPIO_SetBits(GPIOA, GPIO_Pin_6)

#define OLED_CS_L()		GPIO_ResetBits(GPIOA, GPIO_Pin_4)	//PA4-  CS
#define OLED_CS_H()		GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define OLED_RST_L()	GPIO_ResetBits(GPIOC, GPIO_Pin_4)	//PC4-  RESET
#define OLED_RST_H()	GPIO_SetBits(GPIOC, GPIO_Pin_4)

#define OLED_SCLK_L()	GPIO_ResetBits(GPIOA, GPIO_Pin_5)	//PA5-CLK
#define OLED_SCLK_H()	GPIO_SetBits(GPIOA, GPIO_Pin_5)

#define OLED_SDIN_L()	GPIO_ResetBits(GPIOA, GPIO_Pin_7)	//PA7-MOSI
#define OLED_SDIN_H()	GPIO_SetBits(GPIOA, GPIO_Pin_7)

//OLED模式设置
//0:4线串行模式
//1:并行8080模式

#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

//OLED控制函数
extern  void DelayMS(uint32_t dly);
extern  void OLED_Init(void);
extern  void OLED_Fill(uint8_t bmp_dat);
extern  void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);
extern  void OLED_Set_Pos(uint8_t x, uint8_t y);
extern  void LCD_P8x16Str(uint8_t x, uint8_t y,uint8_t ch[]);
extern  void LCD_P16x16Ch(uint8_t x, uint8_t y, uint8_t N);
extern  void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[]);
extern  void OLED_Test(void);
#endif

