#include "dht11.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
/*接收温湿度*/
uint8_t humi_i,humi_f;
uint8_t temp_i,temp_f;

/*若在dht11_rst函数中加入uart1_print函数，则不能正常打印数据
*不要在时序函数中随便使用打印函数
*/
void DHT11_InitConfig()
{
	RCC_APB2PeriphClockCmd(DHT11_CLK, ENABLE); //开启GPIOB时钟
	DHT11_Out();
	GPIO_SetBits(DHT11_GPIO,DHT11_PIN);
}
/*配置引脚为输入模式*/
void DHT11_In()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN ;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING;   //浮空输入
	GPIO_Init(DHT11_GPIO,&GPIO_InitStructure);
}
/*配置引脚为输出模式*/
void DHT11_Out()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11_PIN ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;   //推挽输出
	GPIO_Init(DHT11_GPIO,&GPIO_InitStructure);
}

/*复位DHT11,发送起始信号*/
void dht11_rst(void)
{
	DHT11_Out();
	GPIO_ResetBits(DHT11_GPIO,DHT11_PIN);  //将引脚电平至低，持续20ms，至少拉低18ms
	delay_ms(20);
	GPIO_SetBits(DHT11_GPIO,DHT11_PIN);
	
	delay_us(30);   //将引脚电平至高，持续30u
	DHT11_In();
//	uart1_print(" reset ok....\r\n");    //错误，不能使用此函数
}
/*检查DHT11是否正确回应*/
char dht11_check(void)
{
	uint8_t count;
	while((GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)==0)&&count<80)
	{
	    count++;
		delay_us(1);
	}
	if(count>=100)
		return 1;
	count=0;
	while((GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)==1)&&count<80)
	{
		count++;
		delay_us(1);
	}
	if(count>=100)
		return 1;
//	uart1_print("the dht is working perfectly...");        //错误，不能使用此函数
	return 0;
}

/*读取一个bit的数据*/
char dht11_read_bit(void)
{
	uint8_t count=0;
	
	/*等待管脚变为低电平*/
	while((GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)==1)&&count<80) 
	{
		count++;
		delay_us(1);
	}
	count=0;
	/*等待管脚变为高电平*/
	while((GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)==0)&&count<80) 
	{
		count++;
		delay_us(1);
	}
	delay_us(40);
	if(GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)==0)  //读出电平为低电平，则输出0
		return 0;
	else 
		return 1;
}

/*读取一个字节的数据*/
char dht11_read_byte(void)
{
	char data=0;
	int i;
	for(i=0;i<8;i++)
	{
		data<<=1;
		data |=dht11_read_bit();
	}
	return data;
}

/*读取一次完整的数据，40bit数据*/
char dht11_read_data(uint8_t* humi_i,uint8_t* humi_f,uint8_t* temp_i,uint8_t* temp_f)
{
	uint8_t a[5];
	uint8_t i=0;
	dht11_rst();
	if(dht11_check()==1)
	{
		printf("check failed\r\n");
	}
	
	
	for(i=0;i<5;i++)
	{
		a[i]=dht11_read_byte();
		//uart1_print("hello\r\n");    //此处不能打印数据，否则读取数据失败
	}
	if((a[0]+a[1]+a[2]+a[3])==a[4])
	{
		*humi_i=a[0];     //湿度整型数据
		*humi_f=a[1];     //湿度整型数据
		*temp_i=a[2];     //温度整型数据	
		*temp_f=a[3];     //温度整型数据
		 return 0;
	}
	return 1;
}



