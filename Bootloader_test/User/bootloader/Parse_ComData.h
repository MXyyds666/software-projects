/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : Parse_ComData.h
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PARSE_COMDATA_H
#define PARSE_COMDATA_H 

/* Includes ------------------------------------------------------------------*/
#include "at32f45x.h"
#include "variable.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//PF相关宏定义
#define PF_BOOT_CMD        224  //BOOT升级控制命令
#define PF_BUS_CMD         225  //总线控制静默
#define PF_MULTI_PGN_RW    226  //多PGN读写 对特定目的地多个PGN进行读或写
#define PF_OBD_ERRCODE_TX  227  //OBD故障码传输 1000ms
#define PF_BOOT_JUMP_B		 228	//跳转到B区 2026.4.15
#define PF_BOOT_UPDATA_B	 229	//升级B区 2026.4.15
#define PF_HEARTBEAT       231  //心跳 设备循环发出心跳报文，间隔1000ms，心跳循环数据循环累加
#define PF_RESPONSE        232  //确认 对特定命令，请求的普通广播或ACK或NACK响应
#define PF_SEND_DATA       233  //发送数据 向特定目的地特定PGN发送数据
#define PF_ONE_PGN_REQUEST 234  //请求 单PGN请求
#define PF_GE_DATA         235  //群扩展 多包传输时用于传输数据 数据报文
#define PF_GE_PROTOCOL     236  //群扩展 用于一组特殊功能(如专用功能，网络管理功能，多组传输功能等) 协议报文

//BOOT控制命令相关宏定义
#define IAP_RESET         0x01
#define IAP_BEGIN         0x02
#define IAP_CHECK         0x03

/* Exported types ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
void Parse_CAN_Frame(can_rxbuf_type RxMessage);

#endif 

/*******************************END OF FILE************************/
