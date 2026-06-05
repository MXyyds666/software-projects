/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : variable.h
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef VARIABLE_H
#define VARIABLE_H 

/* Includes ------------------------------------------------------------------*/
#include "at32f45x.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//地址相关宏定义
#define ID_METER       0x10   //仪表
#define ID_PC          0x20   //上位机
#define ID_ECU         0x30   //控制器,已废弃，改为在FLASH中定义 g_DEVICE_ID
#define ID_BMS1        0x40   //BMS1
#define ID_BROADCAST   0xFF   //广播 
#define MYSELF_ID      ID_ECU //自身ID

/* Exported types ------------------------------------------------------------*/
#pragma pack(1)
typedef struct
{
	volatile u8 Be_5ms;
	volatile u8 Be_Reset;
	volatile u8 Be_IAP;
	volatile u8 Be_Wait;
	volatile u16 IAP_Timeout_cnt;
}GLOBAL_VARIABLE;

#pragma pack()
/* Exported variables ------------------------------------------------------- */
extern GLOBAL_VARIABLE g_Variable;

/* Exported functions ------------------------------------------------------- */


#endif 

/*******************************END OF FILE************************/
