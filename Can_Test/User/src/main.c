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
#include "motor.h"
#include "string.h"
#include "flash.h"

uint32_t g_motor_id = 0x01;
extern CAN_TASK_DATA g_CAN_TASK_FLAG;	//任务函数运行切换标志位

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
	uint8_t data[8]={0x12,0x23,0x34,0x45,0x56,0x78,0x89,0x10};

	nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x8000);
	system_clock_config();
	delay_init();
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

	g_motor_id = *(volatile uint32_t*)ID_INFO_Addr;

	Bsp_Can1_Init();
//	while(Bsp_Can1_Init() != SUCCESS);
  
  //配置主机过滤器
  Bsp_Can_Filter_config(CAN1,0x7FF,0x0,CAN_FILTER_NUM_0);
  //配置本机过滤机，只接收发送给本机的报文
  Bsp_Can_Filter_config(CAN1,g_motor_id,0,CAN_FILTER_NUM_1);

  while(1)
  {
	  if(g_CAN_TASK_FLAG.can_task_flag == 1)
	  {
		  handle_zero_set(g_CAN_TASK_FLAG.data, g_CAN_TASK_FLAG.len);
		  memset(&g_CAN_TASK_FLAG, 0, sizeof(CAN_TASK_DATA));
	  }
	  else if(g_CAN_TASK_FLAG.can_task_flag == 2)
	  {
		  handle_id_set(g_CAN_TASK_FLAG.data, g_CAN_TASK_FLAG.len);
		  memset(&g_CAN_TASK_FLAG, 0, sizeof(CAN_TASK_DATA));
	  }
	  else if(g_CAN_TASK_FLAG.can_task_flag ==3)
	  {
		  handle_id_reset(g_CAN_TASK_FLAG.data, g_CAN_TASK_FLAG.len);
		  memset(&g_CAN_TASK_FLAG, 0, sizeof(CAN_TASK_DATA));
	  }
//	  Bsp_Can_Transmit_Classic_Standard(CAN1,0xFF,data,8);
//	  delay_ms(1000);
  }
}
