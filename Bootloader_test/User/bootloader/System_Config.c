/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : System_Config.c
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "System_Config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/*****************************************************************
* Function Name : Peripherals_Clock_Init
* Description   : 外设Clock初始化
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void Peripherals_Clock_Init(confirm_state sate)
{
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, sate);	//GPIOA
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, sate);	//GPIOB
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, sate);	//GPIOC
	crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, sate);	//GPIOD
	crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, sate);		//定时器2
	crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, sate);		//CAN时钟
}

/*****************************************************************
* Function Name : NVIC_Configuration
* Description   : 中断初始化
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void NVIC_Configuration(confirm_state sate)
{
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	if(sate)
	{
		nvic_irq_enable(CAN1_RX_IRQn,1, 0);
		nvic_irq_enable(TMR2_GLOBAL_IRQn, 2, 0);
	}
	else
	{
		nvic_irq_disable(CAN1_RX_IRQn);			//关闭CAN中断
		nvic_irq_disable(TMR2_GLOBAL_IRQn);	//关闭定时器2中断
	}
}

/*****************************************************************
* Function Name : InitAllPeripherals
* Description   : 外设初始化
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void InitAllPeripherals(confirm_state sate)
{
	//中断向量表初始化
	NVIC_Configuration(sate);
	//外设时钟初始化	
	Peripherals_Clock_Init(sate);
	
	//2022-08-25 __set_PRIMASK(!sate);
	
	User_CAN_Init(sate);
 	TIM2_Configuration(sate);
}

/*****************************************************************
* Function Name : InitAllPeripherals
* Description   : 外设初始化
* Input         : None
* Output        : 
* Notes         :
******************************************************************/
void Init_Variables(void)
{
	IAP_Init();
		
//	memset(&g_Variable,0,sizeof(g_Variable)); //全局变量初始化
}

/*******************************END OF FILE************************/
