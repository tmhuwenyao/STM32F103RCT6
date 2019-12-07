#include "bsp_systick.h"
static u8 fac_us=0;  //微秒延时倍乘数
static u16 fac_ms=0;  //毫秒延时倍乘数
/*初始化滴答定时器
*SYSCLK 固定为HCLK的1/8
*SYSCLK为系统时钟
*/
void SYSTICK_InitConfig(uint8_t SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2清空，选择外部时钟， HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
	
}

/*延时毫秒
*SYSTICK->LOAD 为24位的定时器，故最大的延时数mtime<oxffffff*8*1000/SYSCLK
*SYSCLK单位为hz,mtime单位为ms
*对168M条件下,nms<=798ms 
*/
void delay_ms(uint16_t mtime)
{
	u32 temp;		   
	SysTick->LOAD=(u32)mtime*fac_ms;//时间加载
	SysTick->VAL =0x00;           //清空倒计时
	SysTick->CTRL=0x01 ;          //开始计时
	do
	{
		temp=SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
}
/*延时纳秒*/
void delay_us(uint32_t utime)
{
	u32 temp;		   
	SysTick->LOAD=(u32)utime*fac_us;//时间加载
	SysTick->VAL =0x00;           //清空倒计时
	SysTick->CTRL=0x01 ;          //开始计时
	do
	{
		temp=SysTick->CTRL;
	}while(temp&0x01&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器
	
}

