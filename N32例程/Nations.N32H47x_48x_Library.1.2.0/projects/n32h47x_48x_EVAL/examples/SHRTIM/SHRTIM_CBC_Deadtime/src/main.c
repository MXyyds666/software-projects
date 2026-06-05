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

void SystemClock_Config(void);
static void GPIO_Init(void);
static void SHRTIM1_Init(void);

/**
*\*\fun Main program.
*\*\return none
**/
int main(void)
{
    /* Initialize all configured peripherals */
    GPIO_Init();

    SHRTIM1_Init();

    /* Start the timer and enable the outputs */
    SHRTIM_EnableIT_REP(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_CounterEnable(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OUTPUT_TD1);
    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OUTPUT_TD2);

#ifdef ENTRY_TEST
    TestEntry();
#endif
    while (1)
    {
        /* Toggle the board's LED2 at 10Hz */
        LED2_TOGGLE();
        systick_delay_ms(100);
    }
}

/**
*\*\name SHRTIM1 Initialization Function
*\*\return None
**/
static void SHRTIM1_Init(void)
{
    SHRTIM1_MspInit();
    
    SHRTIM_EXEV_SetPrescaler(SHRTIM1, SHRTIM_EXEV_PRESCALER_DIV1);
    SHRTIM_EXEV_SetSrc(SHRTIM1, SHRTIM_EVENT_5, SHRTIM_EXEV5SRC_GPIO);
    SHRTIM_EXEV_SetPolarity(SHRTIM1, SHRTIM_EVENT_5, SHRTIM_EXEV_POLARITY_HIGH);
    SHRTIM_EXEV_SetSensitivity(SHRTIM1, SHRTIM_EVENT_5, SHRTIM_EXEV_SENSITIVITY_LEVEL);
    SHRTIM_EXEV_SetFastMode(SHRTIM1, SHRTIM_EVENT_5, SHRTIM_EXEV_FASTMODE_DISABLE);
    SHRTIM_EXEV_SetSrc(SHRTIM1, SHRTIM_EVENT_8, SHRTIM_EXEV8SRC_GPIO);
    SHRTIM_EXEV_SetPolarity(SHRTIM1, SHRTIM_EVENT_8, SHRTIM_EXEV_POLARITY_HIGH);
    SHRTIM_EXEV_SetSensitivity(SHRTIM1, SHRTIM_EVENT_8, SHRTIM_EXEV_SENSITIVITY_BOTHEDGES);
    SHRTIM_EXEV_SetFilter(SHRTIM1, SHRTIM_EVENT_8, SHRTIM_EXEV_FILTER_NONE);
    SHRTIM_TIM_SetPrescaler(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_PRESCALERRATIO_MUL32);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_MODE_CONTINUOUS);
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_D, TIMD_PERIOD);
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_D, 0x02);
    SHRTIM_TIM_SetUpdateGating(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_UPDATEGATING_INDEPENDENT);
    SHRTIM_TIM_SetCountingMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_COUNTING_MODE_UP);
    SHRTIM_TIM_SetTriggeredHalfMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_TRIGHALF_DISABLED);
    SHRTIM_TIM_SetComp1Mode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_GTCMP1_EQUAL);
    SHRTIM_TIM_SetComp3Mode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_GTCMP3_EQUAL);
    SHRTIM_TIM_SetDACTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_DACTRIG_NONE);
    SHRTIM_TIM_DisableHalfMode(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetInterleavedMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_INTERLEAVED_MODE_DISABLED);
    SHRTIM_TIM_DisableStartOnSync(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_DisableResetOnSync(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_DisableResyncUpdate(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_UPDATETRIG_NONE | SHRTIM_UPDATETRIG_REPETITION);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_RESETTRIG_NONE);
    SHRTIM_TIM_DisablePushPullMode(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_DisableDeadTime(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetBurstModeOption(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_BURSTMODE_MAINTAINCLOCK);
    SHRTIM_ForceUpdate(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_EnableResyncUpdate(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_D, DEADTIME);
    SHRTIM_TIM_SetCompareMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_COMPAREUNIT_2, SHRTIM_COMPAREMODE_DELAY_CMP3);
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_D, DEADTIME);
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_D, 0x0000);
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_D, DEADTIME);
    SHRTIM_TIM_SetCompareMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_COMPAREUNIT_4, SHRTIM_COMPAREMODE_DELAY_NOTIMEOUT);
    SHRTIM_TIM_SetCaptureTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_CAPTUREUNIT_1, SHRTIM_CAPTURETRIG_EXEV_8);
    SHRTIM_TIM_SetCaptureTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_CAPTUREUNIT_2, SHRTIM_CAPTURETRIG_EXEV_8);
    SHRTIM_TIM_SetEventFilter(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_EVENT_8, SHRTIM_EXEVFLT_BLANKINGCMP1);
    SHRTIM_TIM_SetEventLatchStatus(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_EVENT_8, SHRTIM_EXEVLATCH_ENABLED);
    SHRTIM_OUT_SetPolarity(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_POSITIVE_POLARITY);
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTSET_TIMCMP1);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTRESET_TIMCMP3 | SHRTIM_OUTPUTRESET_EXEV_5 | SHRTIM_OUTPUTRESET_EXEV_8);
    SHRTIM_OUT_SetIdleLevel(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_IDLELEVEL_INACTIVE);
    SHRTIM_OUT_SetFaultState(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_OUT_SetChopperMode(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_CHOPPERMODE_DISABLED);
    SHRTIM_OUT_SetPolarity(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUT_POSITIVE_POLARITY);
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUTPUTSET_TIMCMP2);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUTPUTRESET_TIMCMP4 | SHRTIM_OUTPUTRESET_TIMPER | SHRTIM_OUTPUTRESET_EXEV_5);
    SHRTIM_OUT_SetIdleLevel(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUT_IDLELEVEL_INACTIVE);
    SHRTIM_OUT_SetFaultState(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_OUT_SetChopperMode(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUT_CHOPPERMODE_DISABLED);

    SHRTIM_FALT_EnableSysFault(SHRTIM1,SHRTIM_SYSTEM_FAULT_CLOCKSECURITY_ENABLE);
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_FAULT_1);

}

/**
*\*\fun GPIO Initialization Function
*\*\return None
**/
static void GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);

    /* GPIO Ports Clock Enable */
    LED2_CLOCK_ENABLE();

    GPIO_ResetBits(LED2_PORT, LED2_PIN);

    GPIO_InitStructure.Pin = LED2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(LED2_PORT, &GPIO_InitStructure);
}
