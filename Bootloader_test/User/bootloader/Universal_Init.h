/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : Universal_Init.h
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UNIVERSAL_INIT_H
#define UNIVERSAL_INIT_H 

/* Includes ------------------------------------------------------------------*/
#include "at32f45x.h"
#include "bl_can.h"
#include "IAP.h"
#include "bsp_tim.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void TIM2_Configuration(confirm_state sate);

#endif 

/*******************************END OF FILE************************/
