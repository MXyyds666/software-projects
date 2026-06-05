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
*\*\file n32h47x_48x_dma.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "main.h"
#include "n32h47x_48x_it.h"
/** Cortex-M4 Processor Exceptions Handlers **/


uint32_t gCntAwdg = 0;

extern void DMA_Config(void)    ;
/**
*\*\name    NMI_Handler.
*\*\fun     This function handles NMI exception.
*\*\return  None
**/
void NMI_Handler(void)
{
}


/**
*\*\name    HardFault_Handler.
*\*\fun     This function handles Hard Fault exception.
*\*\return  None
**/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}



/**
*\*\name    SVC_Handler.
*\*\fun     This function handles SVCall exception.
*\*\return  None
**/
void SVC_Handler(void)
{
}


/**
*\*\name    PendSV_Handler.
*\*\fun     This function handles PendSV_Handler exception.
*\*\return  None
**/
void PendSV_Handler(void)
{
}


/**
*\*\name    SysTick_Handler.
*\*\fun     This function handles SysTick Handler.
*\*\return  None
**/
void SysTick_Handler(void)
{
}


/**
*\*\name    ADC1_2_IRQHandler.
*\*\fun     This function handles ADC1 and ADC2 global interrupts requests.
*\*\return  None
**/
void ADC1_2_IRQHandler(void)
{
    if(ADC_GetIntStatus(ADC1,ADC_INT_AWD1) != RESET)
    {
        /* Clear ADC1 AWD1 pending interrupt bit */
        ADC_ClearIntPendingBit(ADC1, ADC_INT_AWD1);
        gCntAwdg++;
    }
    else
    {
        /*no process */
    }
    
    if((ADC_GetAnalogWatchdog23IntConfig(ADC1,ADC_AWDG2_EN_OFFSET)&(1<<12)) != 0)
    {
        /* Clear ADC1 AWD2 pending interrupt bit */
        ADC_ClearAnalogWatchdog23StatusFlag(ADC1,ADC_AWDG2_STS_OFFSET,(1<<12));
        gCntAwdg++;
    }
    else
    {
        /*no process */
    }
        
        
}


