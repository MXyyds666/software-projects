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
    uint32_t CurrentPeriod;

    LED3_ON();
    /* Clear ISR flag */
    SHRTIM1->sMasterRegs.MINTCLR = SHRTIM_MINTCLR_MREPTIC;

    switch(DemoState)
    {
    case RUN:
        /* Get current LLC period value */
        CurrentPeriod = SHRTIM1->sMasterRegs.MPRD;
        /* Decrease the LLC period down to 1/4 the init value */
        /* PWM freq is from 50KHz ~ 200KHz */
        if (CurrentPeriod > (LLC_PWM_PERIOD)/4)
        {
            CurrentPeriod--;
        }
        else  /* Re-start ramp from initial period */
        {
            /* Update duty cycle (CMP1) and ADC triggering point (CMP2)*/
            CurrentPeriod = LLC_PWM_PERIOD;
        }
        /* Set new LLC frequency */
        SHRTIM1->sMasterRegs.MPRD = CurrentPeriod; 
        
        /* Adjust Turn-on and turn-off time for SR1 */
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_B].TxCMP1DAT =  (15 * CurrentPeriod)/100;
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_B].TxCMP2DAT =  (35 * CurrentPeriod)/100;

        /* Adjust Turn-on and turn-off time for SR2 */
        SHRTIM1->sMasterRegs.MCMP3DAT =  (65 * CurrentPeriod)/100;
        SHRTIM1->sMasterRegs.MCMP4DAT =  (85 * CurrentPeriod)/100;

        /* Adjust new sampling points for SR1 */
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_B].TxCMP3DAT = (15 * CurrentPeriod)/100 - ADC_CONVERSION_TIME;
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_B].TxCMP4DAT = (15 * CurrentPeriod)/100 + ADC_CONVERSION_TIME;
        SHRTIM1->sMasterRegs.MCMP2DAT = (35 * CurrentPeriod)/100 - ADC_CONVERSION_TIME;
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_B].TxCMP5DAT = (35 * CurrentPeriod)/100 + ADC_CONVERSION_TIME;

        /* Adjust new sampling points for SR2 */
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_C].TxCMP2DAT = (65 * CurrentPeriod)/100 - ADC_CONVERSION_TIME;
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_C].TxCMP3DAT = (65 * CurrentPeriod)/100 + ADC_CONVERSION_TIME;
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_C].TxCMP4DAT = (85 * CurrentPeriod)/100 - ADC_CONVERSION_TIME;
        SHRTIM1->sTimerxRegs[SHRTIM_TIMERINDEX_TIMER_C].TxCMP5DAT = (85 * CurrentPeriod)/100 + ADC_CONVERSION_TIME;
        
        break;

    case FAULT:
        break;

    default:
        break;
    }
    LED3_OFF();

}