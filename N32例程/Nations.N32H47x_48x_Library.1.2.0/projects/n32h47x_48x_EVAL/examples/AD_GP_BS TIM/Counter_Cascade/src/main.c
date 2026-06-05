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

TIM_TimeBaseInitType TIM_TimeBaseStructure;
OCInitType TIM_OCInitStructure;
TIM_BDTRInitType TIM_BDTRInitStructure;
uint32_t number = 0;
/**
*\*\name    main.
*\*\fun     main program.
*\*\param   none
*\*\return  none 
**/
int main(void)
{
    /* System Clocks Configuration */
    RCC_Configuration();

    /* GPIO Configuration */
    GPIO_Configuration();

    /* NVIC Configuration */
    NVIC_Configuration();
    
    /*
    TIMx Configuration:
    Master Mode
    TIMxCLK = SystemCoreClock / 2,
    TIMxFreq = TIMxCLK / (Prescaler + 1) / (Period + 1),
    TIMxCycle = 1 / TIMxFreq,
    The TIMx Update event is used as Trigger Output.
    
    TIMy Configuration:
    Slave Mode
    TIMyCLK: External Clock Source Mode 1,
    TIMyFreq = TIMxFreq / (Prescaler + 1) / (Period + 1),
    TIMyCycle = 1 / TIMyFreq,
    The TIMx Update events trigger the TIMy to count only once.
    */
    
    /* TIMy Slave Configuration: PWM1 Mode */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period        = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler     = 1 - 1;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIMy, &TIM_TimeBaseStructure);

    /* Slave Mode selection: TIMy */
    TIM_SelectSlaveMode(TIMy, TIM_SLAVE_MODE_EXT1);
    TIM_SelectInputTrig(TIMy, TIM_TRIG_SEL_IN_TR0);

    /* Time Base configuration */
    TIM_TimeBaseStructure.Period        = 1000 - 1;
    TIM_TimeBaseStructure.Prescaler     = (uint16_t) ((SystemCoreClock/2) / 1000000) - 1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.RepetCnt      = 0;

    TIM_InitTimeBase(TIMx, &TIM_TimeBaseStructure);

    /* Master Mode selection */
    TIM_SelectOutputTrig(TIMx, TIM_TRGO_SRC_UPDATE);

    /* Select the Master Slave Mode */
    TIM_SelectMasterSlaveMode(TIMx, TIM_MASTER_SLAVE_MODE_ENABLE);

    /* Select TIMx Update Request Interrupt source*/
    TIM_ConfigUpdateRequestIntSrc(TIMx, TIM_UPDATE_SRC_REGULAR);
    
    /* Set ITConfig */
    TIM_ConfigInt(TIMx, TIM_INT_UPDATE, ENABLE);
    TIM_ConfigInt(TIMy, TIM_INT_UPDATE, ENABLE);
    
    /* TIMx counter enable */
    TIM_Enable(TIMx, ENABLE);

    /* TIM enable counter */
    TIM_Enable(TIMy, ENABLE);

    /* Main Output Enable */
    TIM_EnableCtrlPwmOutputs(TIMx, ENABLE);


    while (1)
    {
        number = Get_32bit_cnt();
    }
}

/**
*\*\name    Get_32bit_cnt.
*\*\fun     Configures Get_32bit_cnt.
*\*\param   none
*\*\return  none 
**/
uint32_t Get_32bit_cnt(void)
{
    uint32_t ttL,ttH=0;
    ttH=GTIM5->CNT;
    ttL=ATIM1->CNT;
    ttH=ttH<<16|ttL;    

    return ttH; 
}

/**
*\*\name    RCC_Configuration.
*\*\fun     Configures the different system clocks.
*\*\param   none
*\*\return  none 
**/
void RCC_Configuration(void)
{
    /* TIMx, GPIOx and AFIO clocks enable */
    RCC_ConfigPclk1(RCC_HCLK_DIV4);/* The maximum operating clock of GTIM1-7/BTIM1-2 is 180MHz,the division frequency of PCLK1 cannot be 1 or 2 divisions */
    RCC_EnableAHBPeriphClk(TIMx_CLK,ENABLE);
    RCC_EnableAPB1PeriphClk(TIMy_CLK,ENABLE);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA,ENABLE);
}

/**
*\*\name    GPIO_Configuration.
*\*\fun     Configures the GPIO pins.
*\*\param   none
*\*\return  none 
**/
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    GPIO_InitStruct(&GPIO_InitStructure);
    
    /* GPIOx Configuration: Pin of TIMx */
    GPIO_InitStructure.Pin        = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

/**
*\*\name    NVIC_Configuration.
*\*\fun     Configures the nested vectored interrupt controller.
*\*\param   none
*\*\return  none 
**/
void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Enable the TIMx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                    = ATIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the TIMy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                    = GTIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
