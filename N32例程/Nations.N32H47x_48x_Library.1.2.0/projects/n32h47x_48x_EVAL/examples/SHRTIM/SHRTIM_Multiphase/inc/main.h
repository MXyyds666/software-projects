/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
*\*\file main.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
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
#include "n32h47x_48x_adc.h"
#include "delay.h"
#include "n32h47x_48x_msp.h"
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /* 0 bit  for pre-emption priority, 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /* 1 bit  for pre-emption priority, 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /* 2 bits for pre-emption priority, 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /* 3 bits for pre-emption priority, 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /* 4 bits for pre-emption priority, 0 bit  for subpriority */
#endif

/* For LED1~LED3, the specific port and specific pin depend on the board used. */
#define LED1_PIN GPIO_PIN_3
#define LED1_PORT GPIOA
#define LED1_ON()         LED1_PORT->PBSC = LED1_PIN; 
#define LED1_OFF()        LED1_PORT->PBC = LED1_PIN; 
#define LED1_TOGGLE()     LED1_PORT->POD ^= LED1_PIN;
#define LED1_CLOCK_ENABLE()  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);

#define LED2_PIN GPIO_PIN_8
#define LED2_PORT GPIOA
#define LED2_ON()         LED2_PORT->PBSC = LED2_PIN; 
#define LED2_OFF()        LED2_PORT->PBC = LED2_PIN; 
#define LED2_TOGGLE()     LED2_PORT->POD ^= LED2_PIN;
#define LED2_CLOCK_ENABLE()  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);

#define LED3_PIN GPIO_PIN_4
#define LED3_PORT GPIOB
#define LED3_ON()         LED3_PORT->PBSC = LED3_PIN; 
#define LED3_OFF()        LED3_PORT->PBC = LED3_PIN; 
#define LED3_TOGGLE()     LED3_PORT->POD ^= LED3_PIN;
#define LED3_CLOCK_ENABLE()  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);

#define BUTTION_PIN GPIO_PIN_4
#define BUTTION_PORT GPIOA
#define BUTTON_PRESSED() ((BUTTION_PORT->PID & (uint32_t)BUTTION_PIN) == 0)
#define BUTTON_CLOCK_ENABLE()  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);

#define SHRTIM_TIMERINDEX_TIMER_A (uint32_t)0
#define SHRTIM_TIMERINDEX_TIMER_B (uint32_t)1
#define SHRTIM_TIMERINDEX_TIMER_C (uint32_t)2
#define SHRTIM_TIMERINDEX_TIMER_D (uint32_t)3
#define SHRTIM_TIMERINDEX_TIMER_E (uint32_t)4
#define SHRTIM_TIMERINDEX_TIMER_F (uint32_t)5

#define MULTIPHASE_PERIOD ((uint16_t)0xA000) // 
#define PHASE_SHIFT_INIT ((uint16_t)MULTIPHASE_PERIOD / 5 )
#define DUTY_CYCLE_INIT ((uint16_t)PHASE_SHIFT_INIT / 2)

/* Take values based on 220ns */
#define ADC_CONVERSION_TIME     ((uint16_t)0x6E0) 

typedef enum {
  FIVE_PHASE_INIT, FOUR_PHASE_INIT, THREE_PHASE_INIT, TWO_PHASE_INIT, ONE_PHASE_INIT, BURST_INIT,
  FIVE_PHASE, FOUR_PHASE, THREE_PHASE, TWO_PHASE, ONE_PHASE, BURST, FAULT} DemoModeTypeDef;

extern DemoModeTypeDef DemoState;


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
