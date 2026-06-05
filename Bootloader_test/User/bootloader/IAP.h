/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : IAP.h
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IAP_H
#define IAP_H 

/* Includes ------------------------------------------------------------------*/
#include "at32f45x.h"
#include "flash.h"
#include "variable.h"
#include "bl_can.h"
#include "System_Config.h"

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

//IAP状态宏定义
#define IAP_OK			    0
#define IAPERROR_SIZE		1 	 //固件大小超范围
#define IAPERROR_ERASE		2 	 //擦除Flash失败
#define IAPERROR_WRITEFLASH	3	 //写入Flash失败
#define IAPERROR_UNLOCK		4	 //没有处在锁车状态
#define IAPERROR_INDEX		5	 //索引错误
#define IAPERROR_BUSY		6	 //总线忙
#define IAPERROR_FORM		7 	 //数据格式错误
#define IAPERROR_CRC		8	 //校验错误
#define IAPERROR_OTHER	    9	 //其他错误
#define IAPERROR_JUMP		10		//跳转错误

/* Exported types ------------------------------------------------------------*/
#pragma pack(1)
typedef struct 
{
	volatile u32 IAP_HeadMark;				//头部标记
	volatile u32 IAP_CtrlData;				//控制数据
	volatile u32 IAP_FileSize;				//文件大小
	volatile u8 Be_CanBus_Slience;			//CAN总线静默状态
	volatile u8 Be_Rseset;					//复位状态
	volatile u8 Be_IAP;						//进入IAP标志位
	volatile u8 Rseset_CMD_SRCID;      //复位指令的源ID，记录谁下发的指令
//	volatile u8 Boot_SW_Version[32];   //BOOT软件版本
//	volatile u8 Boot_Compile_Date[8];  //BOOT编译日期
//	volatile u8 Boot_Compile_Time[8];  //BOOT编译时间
}IAP_CTRL_MAP_T;

typedef struct //2022-09-03
{
	volatile u8 Boot_SW_Version[32];   //BOOT软件版本
	volatile u8 Boot_Compile_Date[8];  //BOOT编译日期
	volatile u8 Boot_Compile_Time[8];  //BOOT编译时间
}BOOTLOADINFO;

typedef struct 		//设备ID信息
{
	volatile u32 Device_ID;
}ID_INFO;

typedef struct
{    
	u8 IAP_Busy ;						//总线状态
	s32 IAP_FileSize;				//IAP文件大小
	u16 IAPIndex;	  		    //IAP数据连续索引确认字节
	u16 IAPPageNb;						//IAP数据页数
	u16 IAPPackNb;						//一组数据包数量
	u16 IAP_GroupSize;			//一组数据的字节数
	s32 IAP_WritePos;				//IAP写位置
	s32 IAP_SavePos;				//IAP保存位置
	u32 IAP_CRC;						//IAP的校验位，这里是不是标准CRC，是用累加计算的
	u32 IAP_CRC_cnt;				//CRC累加次数
}IAP_STATE;

#pragma pack()


extern IAP_CTRL_MAP_T IAP_Ctrl_Map;
extern BOOTLOADINFO Bootload_Info;//2022-09-03

/* Exported functions ------------------------------------------------------- */
void IAP_Handle(CAN_EXT_ID extid ,u8* pdata);
void IAP_Init(void);
u8 Save_IAP_CtrlData(void);
u8 Save_BOOT_Info(void);//2022-09-03
u8 Save_ID_Info(uint32_t id);
s32 IAP_JumpTo(u32 ApplicationAddress);

#endif 

/*******************************END OF FILE************************/
