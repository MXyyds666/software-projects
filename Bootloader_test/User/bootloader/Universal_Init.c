/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : Universal_Init.c
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Universal_Init.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

void TIM2_Configuration(confirm_state sate)
{
	TMR2_Configuration(sate);
}

void TMR2_GLOBAL_IRQHandler(void)
{
	if(tmr_interrupt_flag_get(TMR2, TMR_OVF_FLAG) != RESET)
	{
		g_Variable.Be_5ms = 1;
		tmr_flag_clear(TMR2, TMR_OVF_FLAG);
	}
}

/*******************************END OF FILE************************/
