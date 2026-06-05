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
*\*\file n32h47x_48x_msp.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "n32h47x_48x_msp.h"

/**
*\*\name    SHRTPLL_Init.
*\*\fun     Config SHRTPLL clock
*\*\param   SHRTPLL_source
*\*\          - RCC_SHRTPLL_SRC_HSI         HSI oscillator clock selected as SHRTPLL clock entry
*\*\          - RCC_SHRTPLL_SRC_HSE         HSE oscillator clock selected as SHRTPLL clock entry
*\*\param   fin(SHRTPLL input frequency):
*\*\         1000000 ~ 50000000 (Hz)
*\*\param   fout(SHRTPLL output frequency):
*\*\         75000000 ~ 250000000 (Hz)
*\*\return  none 
**/
static void SHRTPLL_Init(uint32_t SHRTPLL_source, uint64_t fin, uint64_t fout)
{
    ErrorStatus ClockStatus;

    if(SHRTPLL_source == RCC_SHRTPLL_SRC_HSE)
    {
        /* Enable HSE */
        RCC_ConfigHse(RCC_HSE_ENABLE);

        /* Wait till HSE is ready */
        ClockStatus = RCC_WaitHseStable();
           
    }
    else
    {
        /* Enable HSI */
        RCC_EnableHsi(ENABLE);

        /* Wait till HSI is ready */
        ClockStatus = RCC_WaitHsiStable();
    }
    
    if(ClockStatus != SUCCESS)
    {
        /* clock source fails to start-up */
        while (1)
        {
        }
    }

    /* Enable phase reset to filter waveform burrs */
    RCC_EnableSHRTPLLPHAReset(ENABLE);
    if(RCC_ConfigSHRTPll(SHRTPLL_source, fin, fout, ENABLE) == SUCCESS)
    {
        
         /* Wait till SHRTPLL is used as system clock source */
        while (RCC_GetFlagStatus(RCC_FLAG_SHRTPLLRDF) != SET)
        {
        }
        /* Disable phase reset */
        RCC_EnableSHRTPLLPHAReset(DISABLE);
    }
    else
    {
        /* SHRTPLL fails  */
        while (1)
        {
        }
    }
}

/**
*\*\name  SHRTIM1_MspInit
*\*\param  None
*\*\return None
**/
void SHRTIM1_MspInit(void)
{
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);

    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    /* Enable the HSE frequency offset detection function and set the offset threshold. HSE 8MHz, range set to 5.3~12.8MHz approximately */
    RCC->CTRL |= (uint32_t)0x00000080;
    RCC->CLKINT |= RCC_CLKINT_HSECSSIEN;
    RCC_EnableHSECSSChk(RCC_CSSEN_HSEFHEN, ENABLE);
    RCC_EnableHSECSSChk(RCC_CSSEN_HSEFLEN, ENABLE);
    RCC_SetHSECSSFrequencyChangeLimit((uint8_t)8);
    RCC_SetHSECSSFrequencyHighLimit((uint8_t)5);
    RCC_SetHSECSSFrequencyLowLimit((uint8_t)5);

    /**SHRTIM1 GPIO Configuration
    PB8   ------> SHRTIM1_EXEV8
    PB9   ------> SHRTIM1_EXEV5
    */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
    AFIO_Config_SHRTIM_EXEV_Pin(AFIO_SHRTIM_EXEV_8, GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE8);
    //AFIO->SHRT_EXEV_CFG[1] = 0x41000000; 
    
    GPIO_InitStructure.Pin = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
    AFIO_Config_SHRTIM_EXEV_Pin(AFIO_SHRTIM_EXEV_5, GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE9);

    /* Configure SHRTIM output: TD1 (PB14) */
    GPIO_InitStructure.Pin = GPIO_PIN_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF10;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* Configure SHRTIM output: TD2 (PB15) */
    GPIO_InitStructure.Pin = GPIO_PIN_15;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF10;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* Configure SHRTIM input: FAULT1 (PA12) */
    GPIO_InitStructure.Pin = GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_DOWN;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF12;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    /* SHRTIM1 interrupt Init */
    NVIC_SetPriority(SHRT1_TIMD_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(SHRT1_TIMD_IRQn);

    /* Initialize SHRTPLL */
    SHRTPLL_Init(RCC_SHRTPLL_SRC_HSE, HSE_VALUE, SHRTIM_INPUT_CLOCK);

    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_SHRTIM, ENABLE);

}
