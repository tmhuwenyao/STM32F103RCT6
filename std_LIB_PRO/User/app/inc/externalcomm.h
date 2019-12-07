#ifndef __EXTERNAL_COMM_H
#define __EXTERNAL_COMM_H

#include "stm32f10x.h"
#include "bsp_usart.h"

#define FC_READ_REGISTER			0X03
#define FC_WRITE_SIGLE_REGISTER		0X05

#define SLAVE_ADDRESS		1
#define S_RX_BUF_SIZE		255
#define S_TX_BUF_SIZE		255

/* RTU 应答代码 */
#define RSP_OK				0		/* 成功 */
#define RSP_ERR_CMD			0x01	/* 不支持的功能码 */
#define RSP_ERR_REG_ADDR	0x02	/* 寄存器地址错误 */
#define RSP_ERR_VALUE		0x03	/* 数据值域错误 */
#define RSP_ERR_WRITE		0x04	/* 写入失败 */

#define ERR_DATA			0Xffff //错误数据

typedef struct
{
	uint8_t RxBuf[S_RX_BUF_SIZE];
	uint8_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;

	uint8_t RspCode;

	uint8_t TxBuf[S_TX_BUF_SIZE];
	uint8_t TxCount;
}MODS_T;


typedef enum  PARAMETER{
	REG1 = 0,
	REG2,
	REG3,
	REG4,
	REG5,
	REG6,
	REG7,
	REG8,
	REG9,
	REG10 ,
	REG_NUM,
}MODS_PARA;

extern MODS_T g_tModS;
extern void MODS_ReceieveNew(uint8_t ch);
extern void Monitor_CommProcess(void);

#endif



