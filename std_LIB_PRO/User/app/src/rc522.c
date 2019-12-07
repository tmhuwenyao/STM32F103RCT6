#include "rc522.h"
#include "bsp_systick.h"
#include "bsp_spi.h"
#include "bsp_usart.h"
#define MAXRLEN 18 
void RC522_Init(void)
{
	 SPI2_Init();
//	 CmdValid=0; 
     PcdReset();
     PcdAntennaOff(); 
     PcdAntennaOn();  
	 M500PcdConfigISOType( 'A' );
 	 delay_ms(100);	
 	 delay_ms(100);
}
/**
  * @name  PcdReset
  * @param  none
  * @retval 成功/失败
  * @brief  复位卡片
  */
char PcdReset(void)
{
	   RC522_RST_H();
    delay_us(1);
   RC522_RST_L();
    delay_us(1);
   RC522_RST_H();
     delay_us(1);
    WriteRawRC(CommandReg,PCD_RESETPHASE);				 //启动卡操作
    delay_us(1);
    
    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);           
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);			
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);     
    return MI_OK;
}
/**
  * @name  PcdAntennaOn
  * @param  none
  * @retval 成功/失败
  * @brief  开启天线
  */
void PcdAntennaOn(void)
{
	unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}
/**
  * @name  PcdAntennaOff
  * @param  none
  * @retval 成功/失败
  * @brief  关闭天线
  */
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

/**
  * @name  PcdRequest
  * @param  req_code:寻卡方式  0x52 = 寻感应区内所有符合14443A标准的卡，0x26 = 寻未进入休眠状态的卡
			pTagType：卡片类型代码	0x4400=Mifare_UltraLight,0x0400 = Mifare_One(S50),0x0200 = Mifare_One(S70),0x0800 = Mifare_Pro(X),0x4403 = Mifare_DESFire
  * @retval 成功/失败
  * @brief  寻卡
  */
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
	char status;  
	unsigned int  unLen;
	unsigned char ucComMF522Buf[MAXRLEN]; 
//  unsigned char xTest ;
	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07);

//  xTest = ReadRawRC(BitFramingReg);
//  if(xTest == 0x07 )
 //   { LED_GREEN  =0 ;}
 // else {LED_GREEN =1 ;while(1){}}
	SetBitMask(TxControlReg,0x03);
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
//     if(status  == MI_OK )
//   { LED_GREEN  =0 ;}
//   else {LED_GREEN =1 ;}
	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   
		status = MI_ERR;   
	}
   
	return status;
}
/**
  * @name  PcdAnticoll
  * @param  pSnr[OUT]:卡片序列号，4字节
  * @retval 成功/失败
  * @brief  防冲撞
  */
char PcdAnticoll(unsigned char *pSnr)
{
	    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}
/**
  * @name  PcdSelect
  * @param  pSnr[IN]:卡片序列号，4字节
  * @retval 成功/失败
  * @brief  选定卡片
  */
char PcdSelect(unsigned char *pSnr)
{
	   char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}	
/**
  * @name  PcdAuthState
  * @param  auth_mode: 0x60 = 验证A密钥,0x61 = 验证B密钥 
			addr:块地址
			pKey:密码
			pSnr:卡片序列号，4字节
  * @retval 成功/失败
  * @brief  验证卡片密码
  */
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
	char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
 //   memcpy(&ucComMF522Buf[2], pKey, 6); 
 //   memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    	status=0x00;
    return status;
}    
/**
  * @name  PcdRead
  * @param  addr:块地址
			pData:读出的数据，16字节
  * @retval 成功/失败
  * @brief  读取M1卡一块数据
  */
char PcdRead(unsigned char addr,unsigned char *pData)
{
	char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}
/**
  * @name  PcdRead
  * @param  addr:块地址
			pData:写入的数据，16字节
  * @retval 成功/失败
  * @brief  写一块数据到M1卡
  */
char PcdWrite(unsigned char addr,unsigned char *pData)
{
	char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}	

char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr);   
/**
  * @name  PcdHalt
  * @param  none
  * @retval 成功/失败
  * @brief  命令卡片进入休眠模式
  */
char PcdHalt(void)
{
	 char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return status;
}
/**
  * @name  PcdComMF522
* @param  Command:RC522命令字
		  pInData:通过RC522发送到卡片的数据
		  InLenByte:发送数据的字节长度
		  pOutData:接收到的卡片返回数据
		  pOutLenBit:返回数据的位长度
  * @retval 成功/失败
  * @brief  命令卡片进入休眠模式
  */
char PcdComMF522(unsigned char Command, 
                 unsigned char *pInData, 
                 unsigned char InLenByte,			  
                 unsigned char *pOutData, 
                 unsigned int  *pOutLenBit)
{
	char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);
   
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }
    
//    i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;
    do 
    {
         n = ReadRawRC(ComIrqReg);
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);
	      
    if (i!=0)
    {    
         if(!(ReadRawRC(ErrorReg)&0x1B))
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)
             {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
         }
         else
         {  
		 	 status = MI_ERR;  
		 
		  }
        
   }
   

   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE); 
   return status;
}
/**
  * @name  CalulateCRC
  * @param  
  * @retval 成功/失败
  * @brief  计算卡片数据的CRC校验
  */				 
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
	unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}
/**
  * @name  WriteRawRC
  * @param  Address:写入地址
		    value:写入的值
  * @retval 成功/失败
  * @brief 写寄存器
  */
void WriteRawRC(unsigned char Address,unsigned char value)
{
	    u8   ucAddr;
//	u8 tmp;

	RC522_CS_L();
    ucAddr = ((Address<<1)&0x7E);

	SPI2WriteByte(ucAddr);
	SPI2WriteByte(value);
	RC522_CS_H();
}
/**
  * @name  ReadRawRC
  * @param  Address:寄存器地址
  * @retval 成功/失败
  * @brief 读的值
  */
unsigned char ReadRawRC(unsigned char Address)
{
	uint8_t   ucAddr;
    uint8_t   ucResult=0;
	RC522_CS_L();
    ucAddr = ((Address<<1)&0x7E)|0x80;
	
	SPI2WriteByte(ucAddr);
	ucResult=SPI2ReadByte();
	RC522_CS_H();
   return ucResult;
}
/**
  * @name  SetBitMask
  * @param  Address:写入地址
		    mask:置位值
  * @retval 成功/失败
  * @brief 写寄存器
  */
void SetBitMask(unsigned char reg,unsigned char mask)
{
	char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}
/**
  * @name  ClearBitMask
  * @param  Address:写入地址
		    mask:清位值
  * @retval 成功/失败
  * @brief 写寄存器
  */
void ClearBitMask(unsigned char reg,unsigned char mask)
{
	char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}
/**
  * @name  M500PcdConfigISOType
  * @param  
  * @retval 成功/失败
  * @brief  设置工作方式
  */
char M500PcdConfigISOType(unsigned char type)
{
	if (type == 'A')                     //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);

 /*    WriteRawRC(CommandReg,0x20);    //as default   
       WriteRawRC(ComIEnReg,0x80);     //as default
       WriteRawRC(DivlEnReg,0x0);      //as default
	   WriteRawRC(ComIrqReg,0x04);     //as default
	   WriteRawRC(DivIrqReg,0x0);      //as default
	   WriteRawRC(Status2Reg,0x0);//80    //trun off temperature sensor
	   WriteRawRC(WaterLevelReg,0x08); //as default
       WriteRawRC(ControlReg,0x20);    //as default
	   WriteRawRC(CollReg,0x80);    //as default
*/
       WriteRawRC(ModeReg,0x3D);//3F
/*	   WriteRawRC(TxModeReg,0x0);      //as default???
	   WriteRawRC(RxModeReg,0x0);      //as default???
	   WriteRawRC(TxControlReg,0x80);  //as default???

	   WriteRawRC(TxSelReg,0x10);      //as default???
   */
       WriteRawRC(RxSelReg,0x86);//84
 //    WriteRawRC(RxThresholdReg,0x84);//as default
 //    WriteRawRC(DemodReg,0x4D);      //as default

 //    WriteRawRC(ModWidthReg,0x13);//26
       WriteRawRC(RFCfgReg,0x7F);   //4F
	/* WriteRawRC(GsNReg,0x88);        //as default???
	   WriteRawRC(CWGsCfgReg,0x20);    //as default???
       WriteRawRC(ModGsCfgReg,0x20);   //as default???
*/
   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
	   
  //   PcdSetTmo(106);
	   delay_ms(10);
       PcdAntennaOn();
   }
   else{ return -1; }
   
   return MI_OK;
}

void iccardcode(void);
/**
  * @name  PcdBakValue
  * @param  sourceaddr：源地址
			goaladdr:目标地址
  * @retval 成功/失败
  * @brief  备份钱包
  */
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
	char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    
	 return MI_ERR;  
	 
	}
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   
		status = MI_ERR;  
	 
	 }

    return status;
}
/**
  * @name  PcdBakValue
  * @param  dd_mode：0xC0 = 扣款，0xC1 = 充值
			addr:扣款地址
			pValue:4字节增(减)值，低位在前
  * @retval 成功/失败
  * @brief  扣款和充值
  */
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
	    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
 //       for (i=0; i<16; i++)
 //       {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {  
			 status = MI_ERR;   
		
		}
    }
    return status;
}
/*
void CALL_isr_UART(void)
{
	    unsigned char len, i;

	    len=RevBuffer[0];
		for(i=0;i<len+1;i++)
		{	 while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
			printf("%c",RevBuffer[i]);	
		}
}

*/
unsigned char g_ucTempbuf[20]; 
unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
unsigned char  da1[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
unsigned char  da2[4]  = {0x00,0,0,0x01};

/*RC522 Test Programme*/
void RC522_Test(void)
{
	uint8_t status = 0;
	uint8_t i,temp = 0;
	while(1)
	{
	status = PcdRequest(PICC_REQALL,g_ucTempbuf);/*扫描卡*/
		 if(status!=0)
		 {	
		 	continue;
		 }
		 printf("卡的类型:");
	     for(i=0;i<2;i++)
	     {	
		  	temp=g_ucTempbuf[i];
			printf("%X",temp);				//打印卡的信息	
			printf("hello->%d\n\r",i);
		 }
		 status = PcdAnticoll(g_ucTempbuf);/*防冲撞*/ 
		 if(status!=0)
		 { 
		 	return;
		 }
		 printf("卡的序号");
		 for(i=0;i<4;i++)
		 {
	    	temp=g_ucTempbuf[i];
			printf("%X",temp);
							
		 }
		 delay_ms(1000);
		 status = PcdSelect(g_ucTempbuf);//选定卡片
		 if(status==0)
		 {
			printf("卡选定成功\n");
		 } 
		 status = PcdAuthState(PICC_AUTHENT1A, 1, DefaultKey, g_ucTempbuf);//验证卡片密码    
		 if (status == MI_OK)
	     {    
		 	printf("校验密码成功\n");
		 }
		 status = PcdWrite(1, da1);
		 if(status==0)
		 {
		 	printf("写卡成功\n");
		 }
		 else
		 {
		 	printf("操作失败");
		 }
		 while(1)
		 {
			 status = PcdRequest(PICC_REQALL, g_ucTempbuf);//寻卡
	         if (status != MI_OK)
	         {    
				  continue;
	         }
			 status = PcdAnticoll(g_ucTempbuf);//防冲撞
	         if (status != MI_OK)
	         {    
				 continue;   
			 }
			 status = PcdSelect(g_ucTempbuf);//选定卡片
	         if (status != MI_OK)
	         {    
			 	continue;
			 }
	         status = PcdAuthState(PICC_AUTHENT1A, 0x01, DefaultKey, g_ucTempbuf);//验证卡片密码
	         if (status != MI_OK)
	         {    
			 	continue;  		 
			 }
	         status = PcdValue(PICC_DECREMENT,0x01,da2);//扣款
	         if (status != MI_OK)
	         {   
			 	 continue;   
			 }
			 delay_ms(1000);
			 printf("\n");
			 status = PcdRead(0x01, g_ucTempbuf);//读块
			 if(status==0)
			 {
			 	printf("读块1成功");
				printf("\n");
			    for(i=0;i<16;i++)
			 	{
				 	temp=g_ucTempbuf[i];
					printf("%X",temp);
			 	}	
			 }
        	 status = PcdBakValue(0x01, 0x02);//块备份
	         if (status != MI_OK)
	         {   
			 	 continue;   
			 }
	         status = PcdRead(0x02, g_ucTempbuf);//读块
	         if (status != MI_OK)
	         {   
			 	 continue;    
			 }
	         printf("读块02：");	//超级终端显示,
	         for(i=0;i<16;i++)
			 {
			 	temp=g_ucTempbuf[i];
				printf("%X",temp);				
			 }
			 printf("\n");			 
			 delay_ms(1000);	         
			 PcdHalt();
		}
	}
}
