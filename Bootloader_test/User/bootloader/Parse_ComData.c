/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : Parse_ComData.c
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Parse_ComData.h"
#include "IAP.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
extern uint32_t g_DEVICE_ID;
extern uint32_t g_JumpInit;
extern uint8_t g_UpData_BootB;

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/


/*****************************************************************
* Function Name : Parse_CAN_Frame
* Description   : CAN数据解析
* Input         : RxMessage:接收到的一帧数据
* Output        : NONE
* Notes         :
******************************************************************/
void Parse_CAN_Frame(can_rxbuf_type RxMessage)
{
	CAN_EXT_ID can_extid;
	can_extid.Cache = RxMessage.id;		//获取ID

	switch(can_extid.params.CAN_PF)		//读取ID指定位，判断CAN的命令
	{
		case PF_BUS_CMD:   //225 0xE1总线控制静默命令
			IAP_Ctrl_Map.Be_CanBus_Slience = RxMessage.data[0];
			if(RxMessage.data[0] == 2)  //2:恢复总线
			{
				IAP_Ctrl_Map.Be_IAP = 0;
				g_Variable.Be_IAP = 0;
			}
			Save_IAP_CtrlData();
			g_Variable.IAP_Timeout_cnt = 0;
			break;
		case PF_BOOT_CMD:			//224 0xE0升级控制命令
		case PF_BOOT_UPDATA_B:	//229 升级BOOTB 2026.4.16
		case PF_GE_DATA:			//235
		case PF_GE_PROTOCOL:		//236
			if(PF_BOOT_UPDATA_B == can_extid.params.CAN_PF)	//升级BOOTB标志位
				g_UpData_BootB = 1;
			g_Variable.IAP_Timeout_cnt = 0;
			if(can_extid.params.CAN_PS == g_DEVICE_ID) //发给自己的数据
				IAP_Handle(can_extid , RxMessage.data);
			break;
		case PF_BOOT_JUMP_B:	//228跳转到 2026.4.15
		{
			g_JumpInit = 0x55AA4433;
			NVIC_SystemReset();
		}
			break;
		default:
			break;
	}
}

/*******************************END OF FILE************************/
