#include "bsp_spi.h"
#include "oleddisplay.h"
#include "oledfont.h"
#include "bsp_systick.h"

#define OLED_WR_CMD  0	//写命令
#define OLED_WR_DATA  1	//写数据
#define OLED_MODE 0
void OLED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	/*初始化*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 |GPIO_Pin_5| GPIO_Pin_6| GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//OLED控制函数
/******************************************************************************
 * 描  述 : 向OLED写入1字节数据
 * 入  参 : dat:数据；mode：=0：写入命令，=1：写入数据
 * 返回值 : 无
 ******************************************************************************/
void OLED_WRByte(uint8_t dat,uint8_t mode)     
{
    uint8_t i=8, temp=0;
    
    if(mode == OLED_WR_CMD)
	{
		OLED_DC_L();
	}
    else
	{
		OLED_DC_H();
	}		
      
    for(i=0; i<8; i++) //发送一个八位数据 
    {
        OLED_SCLK_L();  
        
        temp = dat&0x80;
        if (temp == 0)
        {
            OLED_SDIN_L();
        }
        else
        {
            OLED_SDIN_H();
        }
        dat <<= 1;
		OLED_SCLK_H();             
    }
}
 
/******************************************************************************
 * 描  述 : 设置坐标
 * 入  参 : x：x坐标；y：y坐标
 * 返回值 : 无
 ******************************************************************************/
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
    OLED_WRByte((0xb0+y),OLED_WR_CMD);
    OLED_WRByte(((x&0xf0)>>4)|0x10,OLED_WR_CMD);
    OLED_WRByte((x&0x0f)|0x01,OLED_WR_CMD); 
} 
/******************************************************************************
 * 描  述 : LCD初始化
 * 入  参 : 无
 * 返回值 : 无
 ******************************************************************************/
void OLED_Fill(uint8_t dat) 
{
    uint8_t y,x;
    for(y=0;y<8;y++)
    {
        OLED_WRByte(0xb0+y,OLED_WR_CMD);//设置页地址（0~7）
        OLED_WRByte(0x02,OLED_WR_CMD); //设置显示位置—列低地址
        OLED_WRByte(0x10,OLED_WR_CMD); //设置显示位置—列高地址
        for(x=0; x<X_WIDTH; x++)
        OLED_WRByte(dat,OLED_WR_DATA);
    }
}

/******************************************************************************
 * 描  述 : 指定位置显示一个字符
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{      	
  uint8_t c=0,i=0;	
  
  c = chr-' ';//得到偏移后的值			
  if(x > Max_Column-1)
  {
    x = 0;
    y = y+2;
  }

  OLED_Set_Pos(x,y);	
  for(i=0; i<8; i++)OLED_WRByte(F8X16[c*16+i],OLED_WR_DATA); 
  OLED_Set_Pos(x,y+1);
  for(i=0;i<8;i++)OLED_WRByte(F8X16[c*16+i+8],OLED_WR_DATA);
}

/******************************************************************************
 * 描  述 : 显示8*16一组标准ASCII字符串
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void LCD_P8x16Str(uint8_t x, uint8_t y,uint8_t ch[])
{
  uint8_t c=0,i=0,j=0;
    
  while (ch[j] != '\0')
  {    
    c = ch[j]-32;
    if(x>120){x=0;y++;}
    OLED_Set_Pos(x,y);    
    for(i=0; i<8; i++)OLED_WRByte(F8X16[c*16+i],OLED_WR_DATA);          
    OLED_Set_Pos(x,y+1);    
    for(i=0;i<8;i++) OLED_WRByte(F8X16[c*16+i+8],OLED_WR_DATA);        
    x += 8;
    j++;
  }
}
/******************************************************************************
 * 描  述 : 显示汉字
 * 入  参 : x:列0~127；y:页地址0~7；
 * 返回值 : 无
 ******************************************************************************/
void LCD_P16x16Ch(uint8_t x, uint8_t y, uint8_t N)
{
    uint8_t wm=0;
    unsigned int adder=32*N;        
    OLED_Set_Pos(x , y);
    for(wm = 0;wm < 16;wm++)               
    {
        OLED_WRByte(F16x16[adder],OLED_WR_DATA);    
        adder += 1;
    }      
    OLED_Set_Pos(x,y + 1); 
    for(wm = 0;wm < 16;wm++)         
    {
        OLED_WRByte(F16x16[adder],OLED_WR_DATA);
        adder += 1;
    }           
}
/******************************************************************************
 * 描  述 : 显示BMP图片128×64
 * 入  参 : 起始点坐标(x,y),x的范围0～127，y为页的范围0～7
 * 返回值 : 无
 ******************************************************************************/
void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,const uint8_t BMP[])
{ 	
  uint16_t j=0;
  uint8_t x,y;
  
  if((y1 % 8) == 0) y = y1/8;      
  else y = y1/8+1;
  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0; x<x1; x++)
    {      
      OLED_WRByte(BMP[j++],OLED_WR_DATA);	    	
    }
  }
} 
void OLED_Init(void)
{
	OLED_GPIO_Config();
    OLED_RST_L();
	delay_ms(100); 
	OLED_RST_H();
    delay_ms(10);
	OLED_CS_H();
	delay_ms(10);
    OLED_CS_L();
    
    delay_ms(200);

    OLED_WRByte(0xae,OLED_WR_CMD);//--turn off oled panel
    OLED_WRByte(0x00,OLED_WR_CMD);//---set low column address
    OLED_WRByte(0x10,OLED_WR_CMD);//---set high column address
    OLED_WRByte(0x40,OLED_WR_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WRByte(0x81,OLED_WR_CMD);//--set contrast control register
    OLED_WRByte(0xcf,OLED_WR_CMD); // Set SEG Output Current Brightness
	
    OLED_WRByte(0xa1,OLED_WR_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WRByte(0xc8,OLED_WR_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WRByte(0xa6,OLED_WR_CMD);//--set normal display
    OLED_WRByte(0xa8,OLED_WR_CMD);//--set multiplex ratio(1 to 64)
    OLED_WRByte(0x3f,OLED_WR_CMD);//--1/64 duty
	
    OLED_WRByte(0xd3,OLED_WR_CMD);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WRByte(0x00,OLED_WR_CMD);//-not offset
		
    OLED_WRByte(0xd5,OLED_WR_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WRByte(0x80,OLED_WR_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WRByte(0xd9,OLED_WR_CMD);//--set pre-charge period
		
    OLED_WRByte(0xf1,OLED_WR_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
		//OLED_WRByte(0x22,OLED_WR_CMD);
    OLED_WRByte(0xda,OLED_WR_CMD);//--set com pins hardware configuration
    OLED_WRByte(0x12,OLED_WR_CMD);
    OLED_WRByte(0xdb,OLED_WR_CMD);//--set vcomh
		
    OLED_WRByte(0x40,OLED_WR_CMD);//Set VCOM Deselect Level
		
    OLED_WRByte(0x20,OLED_WR_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WRByte(0x02,OLED_WR_CMD);//
    OLED_WRByte(0x8d,OLED_WR_CMD);//--set Charge Pump enable/disable
    OLED_WRByte(0x14,OLED_WR_CMD);//--set(0x10) disable
    OLED_WRByte(0xa4,OLED_WR_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WRByte(0xa6,OLED_WR_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_WRByte(0xaf,OLED_WR_CMD);//--turn on oled panel
    OLED_Fill(0xff);  //初始清屏
    OLED_Set_Pos(0,0);  
}
/*OLED屏测试程序*/
void OLED_Test(void)
{
	uint8_t i = 0;
	OLED_Fill(0x00);//清屏
	/*显示符号，数字和字母*/
	for(i=0; i<16 ;i++)
    {
        OLED_ShowChar(i*8,0,' '+i);
        OLED_ShowChar(i*8,2,' '+i+16);  
        OLED_ShowChar(i*8,4,' '+i+32);
        OLED_ShowChar(i*8,6,' '+i+48);    
    }
	delay_ms(1000);  //延时，方便观察OLED显示
	delay_ms(1000);  //延时，方便观察OLED显示
	        //显示汉字
    OLED_Fill(0x00); //清屏
    for(i=0; i<3; i++)  //显示6个汉字
    {
        LCD_P16x16Ch(i*16+36,2,i+16 );    //从 i+16个字符开始
    }
    for(i=0; i<7; i++) //显示4个汉字
    {
        LCD_P16x16Ch(i*16+8,4,i+3); 
        
    }
	for(i=0;i<6;i++)
	{
		LCD_P16x16Ch(i*16+16,6,i+10);
	}
	delay_ms(1000);  //延时，方便观察OLED显示
	delay_ms(1000);  //延时，方便观察OLED显示
	

	/*
	//显示图形
    OLED_Fill(0x00);     //清屏
    OLED_DrawBMP(0,0,128,8,BMP1);//显示图形
    delay_ms(1000);  //延时，方便观察OLED显示
	delay_ms(1000);  //延时，方便观察OLED显示
        
    OLED_Fill(0x00); //清屏
    OLED_DrawBMP(0,0,128,8,BMP2);
    delay_ms(1000); 
	*/
}

