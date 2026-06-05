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
*\*\file n32h47x_48x_it.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "main.h"
#include "n32h47x_48x_it.h"

/**
*\*\name    NMI_Handler.
*\*\fun     This function handles NMI exception.
*\*\return  none
**/
void NMI_Handler(void)
{
    RCC->CLKINT |= RCC_CLKINT_HSECSSICLR;
}

/**
*\*\name    HardFault_Handler.
*\*\fun     This function handles Hard Fault exception.
*\*\return  none
**/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    MemManage_Handler.
*\*\fun     This function handles Memory Manage exception.
*\*\return  none
**/
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    BusFault_Handler.
*\*\fun     This function handles Bus Fault exception.
*\*\return  none
**/
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    UsageFault_Handler.
*\*\fun     This function handles Usage Fault exception.
*\*\return  none
**/
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
*\*\name    SVC_Handler.
*\*\fun     This function handles SVCall exception.
*\*\return  none
**/
void SVC_Handler(void)
{
}

/**
*\*\name    DebugMon_Handler.
*\*\fun     This function handles Debug Monitor exception.
*\*\return  none
**/
void DebugMon_Handler(void)
{
}

/**
*\*\name    PendSV_Handler.
*\*\fun     This function handles Pendable request for system service.
*\*\return  none
**/

void PendSV_Handler(void)
{
}

/**
*\*\name    SysTick_Handler.
*\*\fun     This function handles SysTick Handler.
*\*\return  none
**/
void SysTick_Handler(void)
{
}

/**
*\*\name    SHRT1_MST_IRQHandler.
*\*\fun     This function handles SHRTIM1 Master Handler.
*\*\return  none
**/
void SHRT1_MST_IRQHandler(void)
{
    /* For interrupt duration monitoring purpose */
    LED3_ON();

    /* Clear ISR flag */
    SHRTIM1->sMasterRegs.MINTCLR = SHRTIM_MINTCLR_MREPTIC;

    switch (DemoState)
    {
        /* ----------------------------------------- */
        /* Run time operation (duty cycle regulation */
        /* Not implemented, example code only        */
        /* ----------------------------------------- */
        case FIVE_PHASE:
            /* Update here duty cycle as per regulation outputs.
            To be noticed: specific management for 5th phase (simplified if TIMER E is used)
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].TxCMP2DAT = PHASE_SHIFT_INIT + NewDuty; */
            break;

        case FOUR_PHASE:
            /* Update here duty cycle as per regulation outputs
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_D].TxCMP1DAT = NewDuty; */
            break;

        case THREE_PHASE:
            /* Update here duty cycle as per regulation outputs.
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_C].TxCMP1DAT = NewDuty; */
            break;

        case TWO_PHASE:
            /* Update here duty cycle as per regulation outputs.
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].TxCMP1DAT = NewDuty;
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].TxCMP1DAT = NewDuty; */
            break;

        case ONE_PHASE:
            /* Update here duty cycle as per regulation outputs.
            SHRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].TxCMP1DAT = NewDuty; */
            break;

        case BURST:
            /* Update here duty cycle and burst ratio as per regulation outputs */
            break;

        /* ----------------------------------------- */
        /* Transient operation (phase shedding)      */
        /* ----------------------------------------- */
        case FIVE_PHASE_INIT:
            DemoState = FIVE_PHASE;
            SHRTIM1->sMasterRegs.MCMP1DAT = (MULTIPHASE_PERIOD / 5);
            SHRTIM1->sMasterRegs.MCMP2DAT = 2 * (MULTIPHASE_PERIOD / 5);
            SHRTIM1->sMasterRegs.MCMP3DAT = 3 * (MULTIPHASE_PERIOD / 5);
            SHRTIM1->sMasterRegs.MCMP4DAT = 4 * (MULTIPHASE_PERIOD / 5);

            /* Restore waveform generation for TD2 (CMP4 is used as ADC trigger in case of single-phase */
            SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_D].TxSET2 = SHRTIM_OUTPUTSET_MASTERCMP4;

            /* Reconfigure triggers for 5 conversions */
            SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_D].TxCMP4DAT = PHASE_SHIFT_INIT + (DUTY_CYCLE_INIT / 2); /* Samples at 50% of Ton time on phase 5 */
            SHRTIM1->sCommonRegs.ADTG1SRC1 = SHRTIM_ADTG1SRC1_ADTG1TACMP2                                                  /* ADC trigger event: Timer A compare 2 */
                                                | SHRTIM_ADTG1SRC1_ADTG1TBCMP2                                                  /* ADC trigger event: Timer B compare 2 */
                                                | SHRTIM_ADTG1SRC1_ADTG1TCCMP2;                                                 /* ADC trigger event: Timer C compare 2 */

            SHRTIM1->sCommonRegs.ADTG1SRC2 = SHRTIM_ADTG1SRC2_ADTG1TDCMP3                                                  /* ADC trigger event: Timer D compare 3 */
                                                | SHRTIM_ADTG1SRC2_ADTG1TDCMP4;                                                 /* ADC trigger event: Timer D compare 4 */

            /* Disable burst mode */
            SHRTIM1->sCommonRegs.BMCTRL &= ~(SHRTIM_BMCTRL_BMEN);

            break;

        case FOUR_PHASE_INIT:
            DemoState = FOUR_PHASE;
            /* Disables 5th phase and re-balance others */
            SHRTIM1->sMasterRegs.MCMP1DAT = (MULTIPHASE_PERIOD / 4);
            SHRTIM1->sMasterRegs.MCMP2DAT = 2 * (MULTIPHASE_PERIOD / 4);
            SHRTIM1->sMasterRegs.MCMP3DAT = 3 * (MULTIPHASE_PERIOD / 4);
            SHRTIM1->sMasterRegs.MCMP4DAT = MULTIPHASE_PERIOD + 0x20;

            /* Reconfigure Timer D compare 4 for a dummy 5th conversion 250ns before multiphase period end */
            SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_D].TxCMP4DAT = (MULTIPHASE_PERIOD / 4) - 1 - ADC_CONVERSION_TIME;
            break;

        case THREE_PHASE_INIT:
            DemoState = THREE_PHASE;
            /* Disables 4th phase and re-balance others */
            SHRTIM1->sMasterRegs.MCMP1DAT = (MULTIPHASE_PERIOD / 3);
            SHRTIM1->sMasterRegs.MCMP2DAT = 2 * (MULTIPHASE_PERIOD / 3);
            SHRTIM1->sMasterRegs.MCMP3DAT = MULTIPHASE_PERIOD + 0x20;

            /* Reconfigure ADC triggers for a dummy 4th and 5th conversion */
            SHRTIM1->sCommonRegs.ADTG1SRC2 = 0;
            SHRTIM1->sCommonRegs.ADTG1SRC1 = SHRTIM_ADTG1SRC1_ADTG1TACMP2 | SHRTIM_ADTG1SRC1_ADTG1TBCMP2 /* New trigger for dummy conversion */
                                           | SHRTIM_ADTG1SRC1_ADTG1TCCMP2 | SHRTIM_ADTG1SRC1_ADTG1TCCMP3 /* New trigger for dummy conversion */
                                           | SHRTIM_ADTG1SRC1_ADTG1TCCMP4;                               /* New trigger for dummy conversion */
            break;

        case TWO_PHASE_INIT:
            DemoState = TWO_PHASE;
            /* Disables 3rd phase and re-balance others */
            SHRTIM1->sMasterRegs.MCMP1DAT = (MULTIPHASE_PERIOD / 2);
            SHRTIM1->sMasterRegs.MCMP2DAT = MULTIPHASE_PERIOD + 0x20;

            /* Reconfigure ADC triggers for a dummy 3rd, 4th and 5th conversion */
            SHRTIM1->sCommonRegs.ADTG1SRC2 = 0;
            SHRTIM1->sCommonRegs.ADTG1SRC1 = SHRTIM_ADTG1SRC1_ADTG1TACMP2 | SHRTIM_ADTG1SRC1_ADTG1TBCMP2    /* New trigger for dummy conversion */
                                           | SHRTIM_ADTG1SRC1_ADTG1TBCMP3 | SHRTIM_ADTG1SRC1_ADTG1TBCMP4    /* New trigger for dummy conversion */
                                           | SHRTIM_ADTG1SRC1_ADTG1TACMP3;                                  /* New trigger for dummy conversion */
            break;

        case ONE_PHASE_INIT:
            DemoState = ONE_PHASE;
            /* Disables 2nd phase */
            SHRTIM1->sMasterRegs.MCMP1DAT = MULTIPHASE_PERIOD + 0x20;

            /* Reconfigure ADC triggers for a dummy 2nd, 3rd, 4th and 5th conversion */
            SHRTIM1->sMasterRegs.MCMP4DAT = MULTIPHASE_PERIOD - 1 - (ADC_CONVERSION_TIME * 4);
            SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_D].TxSET2 = 0;
            SHRTIM1->sCommonRegs.ADTG1SRC2 = 0;
            SHRTIM1->sCommonRegs.ADTG1SRC1 = SHRTIM_ADTG1SRC1_ADTG1TACMP2 
                                           | SHRTIM_ADTG1SRC1_ADTG1TACMP3 /* New trigger for dummy conversion */
                                           | SHRTIM_ADTG1SRC1_ADTG1TACMP4                                /* New trigger for dummy conversion */
                                           | SHRTIM_ADTG1SRC1_ADTG1TACMP5                                 /* New trigger for dummy conversion */
                                           | SHRTIM_ADTG1SRC1_ADTG1MCMP4;                                /* New trigger for dummy conversion */
            break;

        case BURST_INIT:
            DemoState = BURST;
            /* Enable Burst mode (will start on next master timer period */
            SHRTIM1->sCommonRegs.BMCTRL |= SHRTIM_BMCTRL_BMEN;
            break;

        /* ---------------- */
        /* Fault management */
        /* ---------------- */
        case FAULT:
        default:
            break;
    }

    LED3_OFF();
}