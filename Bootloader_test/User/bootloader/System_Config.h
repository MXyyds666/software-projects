/********************************************************************
* COPYRIGHT(C) 	: 2022 QiTeng
* File Name     : System_Config.h
* Author        : wenjun.wang
* Version       : V1.0.0
* Date          : 2022-5-19
* Description   : 
**********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H 

/* Includes ------------------------------------------------------------------*/
#include "at32f45x.h"
#include "flash.h"
#include "IAP.h"
#include "Universal_Init.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */
void InitAllPeripherals(confirm_state sate);
void Init_Variables(void);

#endif 

/*******************************END OF FILE************************/
