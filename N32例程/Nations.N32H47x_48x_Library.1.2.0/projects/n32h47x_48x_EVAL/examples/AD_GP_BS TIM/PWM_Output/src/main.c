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
uint32_t CCR1_Val       = 600;
uint32_t CCR2_Val       = 300;
uint32_t CCR3_Val       = 150;
uint32_t CCR4_Val       = 75;
uint32_t PrescalerValue = 0;

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

    /*
    TIMx Configuration: generate 4 PWM signals with 4 different duty cycles:
    TIMx Frequency = TIMx counter clock/(AR + 1)
    TIMx Channel1 duty cycle = (TIMx_CCDAT1/ TIMx_AR)* 100
    TIMx Channel2 duty cycle = (TIMx_CCDAT2/ TIMx_AR)* 100
    TIMx Channel3 duty cycle = (TIMx_CCDAT3/ TIMx_AR)* 100
    TIMx Channel4 duty cycle = (TIMx_CCDAT4/ TIMx_AR)* 100
    */
    /* Compute the prescaler value */
    PrescalerValue = (uint32_t)((SystemCoreClock/2) / 12000000) - 1;
    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = 1200;
    TIM_TimeBaseStructure.Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIMx, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR1_Val;
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;

    TIM_InitOc1(TIMx, &TIM_OCInitStructure);

    TIM_ConfigOc1Preload(TIMx, TIM_OC_PRE_LOAD_ENABLE);

    /* PWM1 Mode configuration: Channel2 */
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR2_Val;

    TIM_InitOc2(TIMx, &TIM_OCInitStructure);

    TIM_ConfigOc2Preload(TIMx, TIM_OC_PRE_LOAD_ENABLE);

    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR3_Val;

    TIM_InitOc3(TIMx, &TIM_OCInitStructure);

    TIM_ConfigOc3Preload(TIMx, TIM_OC_PRE_LOAD_ENABLE);

    /* PWM1 Mode configuration: Channel4 */
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR4_Val;

    TIM_InitOc4(TIMx, &TIM_OCInitStructure);

    TIM_ConfigOc4Preload(TIMx, TIM_OC_PRE_LOAD_ENABLE);

    TIM_ConfigArPreload(TIMx, ENABLE);

    /* TIMx enable counter */
    TIM_Enable(TIMx, ENABLE);
    
    while (1);
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
    RCC_EnableAPB1PeriphClk(TIMx_CLK,ENABLE);
    RCC_EnableAHB1PeriphClk(TIMx_CH1_GPIO_CLK | TIMx_CH2_GPIO_CLK |\
                            TIMx_CH3_GPIO_CLK | TIMx_CH4_GPIO_CLK,ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
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
    GPIO_InitStructure.Pin        = TIMx_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = TIMx_CH1_GPIO_AF;
    GPIO_InitPeripheral(TIMx_CH1_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin        = TIMx_CH2_GPIO_PIN;
    GPIO_InitStructure.GPIO_Alternate = TIMx_CH2_GPIO_AF;
    GPIO_InitPeripheral(TIMx_CH2_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin        = TIMx_CH3_GPIO_PIN;
    GPIO_InitStructure.GPIO_Alternate = TIMx_CH3_GPIO_AF;
    GPIO_InitPeripheral(TIMx_CH3_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin        = TIMx_CH4_GPIO_PIN;
    GPIO_InitStructure.GPIO_Alternate = TIMx_CH4_GPIO_AF;
    GPIO_InitPeripheral(TIMx_CH4_GPIO_PORT, &GPIO_InitStructure);
}


