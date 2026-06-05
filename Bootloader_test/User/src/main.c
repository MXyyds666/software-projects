/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *
  * Copyright (c) 2025, Artery Technology, All rights reserved.
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f45x_board.h"
#include "at32f45x_clock.h"
#include "bsp_can.h"
#include "System_Config.h"
#include "Parse_ComData.h"
#include "string.h"

#if defined(__CC_ARM)   /* MDK AC5 */
volatile uint32_t g_JumpInit __attribute__((at(0x20000000), zero_init)); 	//程序是否跳转标志位
volatile uint32_t g_DEVICE_ID __attribute__((at(ID_INFO_Addr), zero_init));	//ID存储位置
#endif

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
	FORM_FRAME_DATA s_frame_data = {g_DEVICE_ID, 0x20, PF_RESPONSE, 0, 5, 8};//初始化一个自定义的 CAN 数据帧结构体 (用于回复响应)
	
	//Bootloader启动后先检查设备ID是否正确
	if(g_DEVICE_ID == 0 || g_DEVICE_ID >=0x7FF)
	{
		Flash_Prepare(ID_INFO_Addr, 4);
		if(g_DEVICE_ID == 0xFFFFFFFF)
		{
			Save_ID_Info(0x01);
		}
	}
	
	//变量操作，是否进行跳转
//	if (g_JumpInit == 0xAA553344)	        /* 软件复位后再进入APP，提供一个干净的CPU环境给APP */
//	{
////			CAN_PushFrame(s_frame_data);
////			Can_Check_Send();

//		IAP_JumpTo(ADD_FORMAL);	                    /* 去执行APP程序 */
//	}
	switch(g_JumpInit)
	{
		case 0xAA553344:	//跳转APP
			IAP_JumpTo(ADD_FORMAL);	                    /* 去执行APP程序 */
			break;
		case 0x55AA4433:	//跳转BootB
//			IAP_JumpTo();
			break;
	}
	
	system_clock_config();
	delay_init();
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0);	//设置中断向量表基地址在 Flash 的 0 偏移处 (即 0x08000000)
	__disable_irq();  														//关闭全局中断，确保初始化期间不会被中断打断
	static uint16_t s_cnt = 0;	//定义局部静态变量用于开机延时计数
//	FORM_FRAME_DATA s_frame_data = {g_DEVICE_ID, 0x20, PF_RESPONSE, 0, 5, 8};//初始化一个自定义的 CAN 数据帧结构体 (用于回复响应)
	memset(s_frame_data.pdata, 0xFF, 8); 

	Init_Variables();
	InitAllPeripherals(TRUE);	
	__enable_irq();		//2022-08-25  开放总中断
	
	while(1)
	{
		if(g_Variable.Be_5ms == 1)
		{
			//程序跳转  
			if(++s_cnt>=400) {                       //2s开机等待
				if(!IAP_Ctrl_Map.Be_IAP) {            //没有在进行程序更新
					IAP_Ctrl_Map.Be_Rseset = 0;  //跳转走之前清标志位	
					Save_IAP_CtrlData();
					g_JumpInit = 0xAA553344;
//					CAN_PushFrame(s_frame_data);
//					Can_Check_Send();
					NVIC_SystemReset();
//					if(!IAP_JumpTo(ADD_FORMAL))	{ 	//跳转到正式程序区
//						g_Variable.Be_Reset = 1;	//跳转错误，程序复位
//					}
				}
				s_cnt = 400;
			}
			
			if(++g_Variable.IAP_Timeout_cnt >= 1000) { //5s
				g_Variable.Be_IAP = 0;    //超时清标志
				g_Variable.IAP_Timeout_cnt = 1000;
			}
			g_Variable.Be_5ms = 0;
		}

		//复位  程序更新过程中不进行复位
		if(g_Variable.Be_Reset == 1) {
			NVIC_SystemReset();
		}
		//测试CAN发送报文
//		CAN_PushFrame(s_frame_data);
		Can_Check_Send();
//		delay_ms(1000);
	}
}

void HardFault_Handler(void) {
	//如果发生硬件故障，置位复位标志，然后复位  为了防止Flash划分不一致，引起的产品变砖
	IAP_Ctrl_Map.Be_Rseset = 1;  //跳转走之前清标志位
	Save_IAP_CtrlData();
	NVIC_SystemReset(); 
}
