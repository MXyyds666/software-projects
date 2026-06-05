/**
*     Copyright (c) 2023, Nations Technologies Inc.
*
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter
* referred to as NATIONS). This software, and the product of NATIONS described herein
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other
* intellectual property rights. Names and brands of third party may be mentioned or referred
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and
* improvements to this software at any time without notice. Please contact NATIONS and obtain
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes
* no responsibility for the accuracy and reliability of this software.
*
*     It is the responsibility of the user of this software to properly design, program, and test
* the functionality and safety of any application made of this information and any resulting product.
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations.
* User shall comply with any applicable export control laws and regulations promulgated and administered by
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/

/**
*\*\file main.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x.h"
#include "n32h47x_48x_shrtim.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_exti.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_gpio.h"
#include "delay.h"
#include "n32h47x_48x_msp.h"
#include "test.h"

#define SHRTIM_TIMERINDEX_TIMER_A (uint32_t)0
#define SHRTIM_TIMERINDEX_TIMER_B (uint32_t)1
#define SHRTIM_TIMERINDEX_TIMER_C (uint32_t)2
#define SHRTIM_TIMERINDEX_TIMER_D (uint32_t)3
#define SHRTIM_TIMERINDEX_TIMER_E (uint32_t)4
#define SHRTIM_TIMERINDEX_TIMER_F (uint32_t)5

#define SHRTIM_INPUT_CLOCK 250000000
#define TIMD_PWM_FREQ 200000
#define DEADTIME_IN_NS 50

/* For LED1~LED3, the specific port and specific pin depend on the board used. */
#define LED1_PIN GPIO_PIN_2
#define LED1_PORT GPIOH
#define LED1_ON()         LED1_PORT->PBSC = LED1_PIN; 
#define LED1_OFF()        LED1_PORT->PBC = LED1_PIN; 
#define LED1_TOGGLE()     LED1_PORT->POD ^= LED1_PIN;
#define LED1_CLOCK_ENABLE()  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);

#define LED2_PIN GPIO_PIN_3
#define LED2_PORT GPIOH
#define LED2_ON()         LED2_PORT->PBSC = LED2_PIN; 
#define LED2_OFF()        LED2_PORT->PBC = LED2_PIN; 
#define LED2_TOGGLE()     LED2_PORT->POD ^= LED2_PIN;
#define LED2_CLOCK_ENABLE()  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);

#define LED3_PIN GPIO_PIN_5
#define LED3_PORT GPIOH
#define LED3_ON()         LED3_PORT->PBSC = LED3_PIN; 
#define LED3_OFF()        LED3_PORT->PBC = LED3_PIN; 
#define LED3_TOGGLE()     LED3_PORT->POD ^= LED3_PIN;
#define LED3_CLOCK_ENABLE()  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);

#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /* 0 bit  for pre-emption priority, 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /* 1 bit  for pre-emption priority, 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /* 2 bits for pre-emption priority, 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /* 3 bits for pre-emption priority, 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /* 4 bits for pre-emption priority, 0 bit  for subpriority */
#endif

#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) * 32) / TIMD_PWM_FREQ))

#define DEADTIME ((uint16_t)(((uint64_t)SHRTIM_INPUT_CLOCK * 32 * DEADTIME_IN_NS) / 1000000000))

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
