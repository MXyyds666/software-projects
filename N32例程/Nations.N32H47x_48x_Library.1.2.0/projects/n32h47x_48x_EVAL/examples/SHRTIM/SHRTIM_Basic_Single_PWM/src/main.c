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
*\*\file main.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "main.h"


/* Formula below works for PWM frequency > 83kHz (with prescaler ratio = 1) */
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) * 32) / TIMD_PWM_FREQ))

/* If the prescaler ratio is above 1 the formula must be changed as following
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) * 16) / TIMD_PWM_FREQ))
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) * 8) / TIMD_PWM_FREQ))
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) * 4) / TIMD_PWM_FREQ))
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) * 2) / TIMD_PWM_FREQ))
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK)) / TIMD_PWM_FREQ))
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) / 2) / TIMD_PWM_FREQ))
#define TIMD_PERIOD ((uint16_t)((((uint64_t)SHRTIM_INPUT_CLOCK) / 4) / TIMD_PWM_FREQ))
*/

static void SHRTIM1_Init(void);
static void GPIO_Init(void);

/**
*\*\fun Main program.
*\*\return none
**/
int main(void)
{
    /* Init LED pin and button pin */
    GPIO_Init();

    /* SHRTIM1_Init all configured peripherals */
    SHRTIM1_Init();
    
    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OUTPUT_TD1);
    
    SHRTIM_TIM_CounterEnable(SHRTIM1, SHRTIM_TIMER_D);

    while (1)
    {
        LED1_TOGGLE();
        systick_delay_ms(1000);
    }
}

/**
*\*\name SHRTIM1 Initialization Function
*\*\return None
**/
static void SHRTIM1_Init(void)
{
    SHRTIM1_MspInit();
    
    SHRTIM_TIM_SetPrescaler(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_PRESCALERRATIO_MUL32);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_MODE_CONTINUOUS);
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_D, TIMD_PERIOD);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_D, (((uint32_t)50 * (TIMD_PERIOD - 1)) / 100));
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTSET_TIMPER);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTRESET_TIMCMP1);

    /* Configure clock security system error to system fault.
    Fault state is inactive. Enable fault1 */
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_FAULTSTATE_INACTIVE);

    SHRTIM_FALT_EnableSysFault(SHRTIM1,SHRTIM_SYSTEM_FAULT_CLOCKSECURITY_ENABLE);

    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_FAULT_1);

    SHRTIM_ForceUpdate(SHRTIM1, SHRTIM_TIMER_D);
}

/**
*\*\fun GPIO Initialization Function
*\*\return None
**/
void GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    GPIO_InitStruct(&GPIO_InitStructure);

    /* GPIO Ports Clock Enable */
    LED1_CLOCK_ENABLE();
    LED2_CLOCK_ENABLE();
    LED3_CLOCK_ENABLE();

    GPIO_InitStructure.Pin = LED1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(LED1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED2_PIN;
    GPIO_InitPeripheral(LED2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED3_PIN;
    GPIO_InitPeripheral(LED3_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin = BUTTION_PIN;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitPeripheral(BUTTION_PORT, &GPIO_InitStructure);
}
