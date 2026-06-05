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
static void GPIO_Init(void);
static void SHRTIM1_Init(void);

/**
*\*\fun Main program.
*\*\return none
**/
int main(void)
{
    /* LED and button init */
    GPIO_Init();

    SHRTIM1_Init();

    while (1)
    {
        while(SHRTIM1->sCommonRegs.INTSTS & SHRTIM_INTSTS_FALT1ITF)
        {
            LED1_ON();
            systick_delay_ms(20);
            LED1_OFF();
            systick_delay_ms(80);
            
            if(BUTTON_PRESSED() != RESET)
            {
                SHRTIM1->sCommonRegs.INTCLR = SHRTIM_INTCLR_FALT1IC;
                
                SHRTIM1->sCommonRegs.OEN = SHRTIM_OEN_TD1OEN;
            }
        }

        LED3_ON();
        systick_delay_ms(100);
        LED3_OFF();
        systick_delay_ms(400);
    }
}

/**
*\*\name SHRTIM1 Initialization Function
*\*\return None
**/
static void SHRTIM1_Init(void)
{
    SHRTIM1_MspInit();

    /* ---------------------------------------------------------------- */
    /* Master timer initialization: PWM frequency and mode (continuous) */
    /* Update on repetition, interrupt issued on repetition event       */
    /* ---------------------------------------------------------------- */
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_MODE_CONTINUOUS);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_UPDATETRIG_RESET);
    SHRTIM_TIM_SetCompareMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_COMPAREUNIT_2, SHRTIM_COMPAREMODE_DELAY_CMP1);
    SHRTIM_TIM_SetCompareMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_COMPAREUNIT_4, SHRTIM_COMPAREMODE_DELAY_CMP1);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_RESETTRIG_EXEV_4 | SHRTIM_RESETTRIG_CMP4);

    /* --------------------------------- */
    /* TD1 and TD2 waveforms description */
    /* TD1 is the PFC output             */
    /* TD2 is here for debugging purpose */
    /* --------------------------------- */
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_TXSET1_CMP4 | SHRTIM_TXSET1_EXEV4);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_TXRST1_CMP1 | SHRTIM_TXRST1_EXEV3);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_FAULTSTATE_INACTIVE);

    /* TD2 allows to monitor either the OC or the ZCD blaking windows */
    /* Uncomment the following lines to monitor the ZCD blanking window on TD2 output */
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_TXSET2_CMP4 | SHRTIM_TXSET2_EXEV4);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_TXRST2_CMP2);

    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_D, TON_MAX);
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_D, TON_MIN);
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_D, TOFF_MIN);
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_D, TOFF_MAX);

    /* ------------------------------ */
    /* OC and ZCD filters programming */
    /* ------------------------------ */
    /* OC (EXEV3 signal) blanking done with CMP3 */
    SHRTIM_TIM_SetEventFilter(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_EVENT_3, SHRTIM_EXEVFLT_BLANKINGCMP3);
    SHRTIM_TIM_SetEventFilter(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_EVENT_4, SHRTIM_EXEVFLT_BLANKINGCMP2);
    SHRTIM_TIM_SetEventLatchStatus(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_EVENT_4, SHRTIM_EXEVLATCH_ENABLED);

    /* ------------------------------------------------- */
    /* Capture units programming for autodelayed compare */
    /* ------------------------------------------------- */
    /* ZCD (EEV4) triggers Capture 1 on rising edge for autodelayed CMP2 */
    /* ZCD (EXEV4) triggers Capture 2 on rising edge for autodelayed CMP4 */
    SHRTIM_TIM_SetCaptureTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_CAPTUREUNIT_1, SHRTIM_CAPTURETRIG_EXEV_3);
    SHRTIM_TIM_SetCaptureTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_CAPTUREUNIT_2, SHRTIM_CAPTURETRIG_EXEV_3);
    /* ZCD (EEV4) falling edge sensitive and OC (EXEV3) rising edge sensitive */
    SHRTIM_EXEV_SetSensitivity(SHRTIM1, SHRTIM_EVENT_4, SHRTIM_EXEV_SENSITIVITY_FALLINGEDGE);
    SHRTIM_EXEV_SetSensitivity(SHRTIM1, SHRTIM_EVENT_3, SHRTIM_EXEV_SENSITIVITY_RISINGEDGE);

    /* ---------------------*/
    /* FAULT initialization */
    /* ---------------------*/
    /* FAULT1 global init: no filter, high polarity, Fault1 enable */
    SHRTIM_FALT_SetPolarity(SHRTIM1, SHRTIM_FAULT_1, SHRTIM_FALT_POLARITY_HIGH);
    SHRTIM_FALT_Enable(SHRTIM1, SHRTIM_FAULT_1);

    /* Fault 1 enabled for Timer D */
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_FAULT_1);

    SHRTIM_FALT_EnableSysFault(SHRTIM1,SHRTIM_SYSTEM_FAULT_CLOCKSECURITY_ENABLE);
    /* ---------------*/
    /* HRTIM start-up */
    /* ---------------*/
    /* Force register update before starting (optional) */
    SHRTIM_ForceUpdate(SHRTIM1, SHRTIM_TIMER_A);

    /* Enable HRTIM's outputs TD1 and TD2 */
    /* Note: it is necessary to enable also GPIOs to have outputs functional */
    /* This must be done after HRTIM initialization */
    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OEN_TD1OEN | SHRTIM_OEN_TD2OEN);

    /* Start HRTIM's TIMER D*/
    SHRTIM_TIM_CounterEnable(SHRTIM1, SHRTIM_TIMER_D);
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
    LED3_CLOCK_ENABLE();
    BUTTON_CLOCK_ENABLE();

    /* The PB4 of N32H474VEL7-STB V1.0 is connected to the download chip. To enable PB4 for LED output, JTAG-DP must be disabled and SW-DP must be enabled. */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    GPIO_ConfigPinRemap(0,0,GPIO_RMP_SWJ_SWD);
    
    GPIO_InitStructure.Pin = LED1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(LED1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED3_PIN;
    GPIO_InitPeripheral(LED3_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin = BUTTION_PIN;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitPeripheral(BUTTION_PORT, &GPIO_InitStructure);
}

