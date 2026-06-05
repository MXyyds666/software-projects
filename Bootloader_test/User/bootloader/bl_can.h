#ifndef __BL_CAN_H__
#define __BL_CAN_H__

#include "bsp_can.h"
#include "Parse_ComData.h"
#include "string.h"

#define CAN_TX_PIN      GPIO_Pin_9
#define CAN_TX_GPIO     GPIOB
#define CAN_RX_PIN      GPIO_Pin_8
#define CAN_RX_GPIO     GPIOB

#define CAN_MAX_FRM_LEN     8

/* Exported types ------------------------------------------------------------*/
struct CAN_EXTID  //CAN的扩展ID 29位
{
	uint32_t CAN_SD:8;       //源地址：发送该报文的设备地址  8位
	uint32_t CAN_PS:8;       //特定PDU：目标地址(DA)或组扩展(GE) 8位
	uint32_t CAN_PF:8;       //PDU格式：协议格式，决定是点对点还是广播 8位
	uint32_t CAN_DP:1;       //数据页：数据页  1位
	uint32_t CAN_EDP:1;      //保留位  1位
	uint32_t CAN_P:3;        //优先级：优先级  1位
};

//公用联合体
typedef union
{
	struct CAN_EXTID params;
	volatile uint32_t Cache;
}CAN_EXT_ID;	

typedef struct 
{
	uint8_t s_id;
	uint8_t ps;
	uint8_t pf;
	uint8_t dp;
	uint8_t p;
	uint8_t datalength;
	uint8_t pdata[8]; //最多8字节
}FORM_FRAME_DATA;

void User_CAN_Init(confirm_state sate);
void Can_Check_Send(void);
void CAN_PushFrame(FORM_FRAME_DATA frame_data);

#endif
