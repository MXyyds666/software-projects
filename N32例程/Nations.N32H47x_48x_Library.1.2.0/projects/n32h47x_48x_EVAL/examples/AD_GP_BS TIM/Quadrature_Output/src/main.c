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
    
    /* GTIM7 Configuration */
    GTIM7_Configuration();

    /* ATIM1 Configuration */    
    ATIM1_Configuration();

    /* ATIM2 Configuration */    
    ATIM2_Configuration();
 
    /* GTIM8 Configuration */
    GTIM8_Configuration();

    /* GTIM9 Configuration */
    GTIM9_Configuration();

    /* Enable MOEN */
    TIM_EnableCtrlPwmOutputs(ATIM1, ENABLE);
    TIM_EnableCtrlPwmOutputs(ATIM2, ENABLE); 
    TIM_EnableCtrlPwmOutputs(GTIM8, ENABLE); 
    TIM_EnableCtrlPwmOutputs(GTIM9, ENABLE); 
    
    /* Enable GTIM7 */
    TIM_Enable(GTIM7, ENABLE);  
    
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
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_ATIM1 | RCC_AHB_PERIPHEN_ATIM2,ENABLE);
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM7,ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GTIM8 | RCC_APB2_PERIPH_GTIM9, ENABLE);
    RCC_EnableAHB1PeriphClk(ATIM1_CH1_GPIO_CLK | ATIM2_CH1_GPIO_CLK |\
                            GTIM8_CH1_GPIO_CLK | GTIM9_CH1_GPIO_CLK,ENABLE);
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
    /* ATIM1 CH1 */    
    GPIO_InitStructure.Pin        = ATIM1_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = ATIM1_CH1_GPIO_AF;
    GPIO_InitPeripheral(ATIM1_CH1_GPIO_PORT, &GPIO_InitStructure);
    
    /* GPIOx Configuration: Pin of TIMx */
    /* ATIM2 CH1 */    
    GPIO_InitStructure.Pin        = ATIM2_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = ATIM2_CH1_GPIO_AF;
    GPIO_InitPeripheral(ATIM2_CH1_GPIO_PORT, &GPIO_InitStructure);  
    
    /* GPIOx Configuration: Pin of TIMx */
    /* GTIM8 CH1 */    
    GPIO_InitStructure.Pin        = GTIM8_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = GTIM8_CH1_GPIO_AF;
    GPIO_InitPeripheral(GTIM8_CH1_GPIO_PORT, &GPIO_InitStructure);     

    /* GPIOx Configuration: Pin of TIMx */
    /* GTIM9 CH1 */    
    GPIO_InitStructure.Pin        = GTIM9_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = GTIM9_CH1_GPIO_AF;
    GPIO_InitPeripheral(GTIM9_CH1_GPIO_PORT, &GPIO_InitStructure); 

}

/**
*\*\name    GTIM7_Configuration.
*\*\fun     Configures the ATIM1.
*\*\param   none
*\*\return  none 
**/
void GTIM7_Configuration(void)
{
    /* Time Base configuration */
    PrescalerValue = (uint32_t)((SystemCoreClock/2) / 12000000) - 1;    
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period        = 1199;
    TIM_TimeBaseStructure.Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_InitTimeBase(GTIM7, &TIM_TimeBaseStructure);

    /* Channel 1 Configuration in PWM mode */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 149;
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;
    TIM_InitOc1(GTIM7, &TIM_OCInitStructure);
    
    /* Channel 2 Configuration in PWM mode */
    TIM_OCInitStructure.Pulse       = 299;
    TIM_InitOc2(GTIM7, &TIM_OCInitStructure);
    
    /* Channel 3 Configuration in PWM mode */
    TIM_OCInitStructure.Pulse       = 449;
    TIM_InitOc3(GTIM7, &TIM_OCInitStructure);
    
    /* Channel 4 Configuration in PWM mode */
    TIM_OCInitStructure.Pulse       = 599;
    TIM_InitOc4(GTIM7, &TIM_OCInitStructure);    
    
}

/**
*\*\name    ATIM1_Configuration.
*\*\fun     Configures the ATIM1.
*\*\param   none
*\*\return  none 
**/
void ATIM1_Configuration(void)
{
    /* Time Base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.CapCh2Sel     = TIM_CAPCH2SEL_1;
    TIM_TimeBaseStructure.Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period        = 1199;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.RepetCnt      = 0;
    TIM_InitTimeBase(ATIM1, &TIM_TimeBaseStructure);

    /* Channel 1 Configuration in PWM mode */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode       = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_DISABLE;
    TIM_OCInitStructure.Pulse        = 599;
    TIM_OCInitStructure.OCPolarity   = TIM_OC_POLARITY_LOW;
    TIM_OCInitStructure.OCNPolarity  = TIM_OCN_POLARITY_LOW;
    TIM_OCInitStructure.OCIdleState  = TIM_OC_IDLE_STATE_SET;
    TIM_OCInitStructure.OCNIdleState = TIM_OCN_IDLE_STATE_RESET;
    TIM_InitOc1(ATIM1, &TIM_OCInitStructure);

    /* Automatic Output enable, Break, dead time and lock configuration*/
    TIM_InitBkdtStruct(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.OSSRState       = TIM_OSSR_STATE_ENABLE;
    TIM_BDTRInitStructure.OSSIState       = TIM_OSSI_STATE_ENABLE;
    TIM_BDTRInitStructure.LOCKLevel       = TIM_LOCK_LEVEL_1;
    TIM_BDTRInitStructure.DeadTime        = 0;
    TIM_BDTRInitStructure.Break           = TIM_BREAK_IN_DISABLE;
    TIM_BDTRInitStructure.BreakPolarity   = TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTO_OUTPUT_DISABLE;
    TIM_ConfigBkdt(ATIM1, &TIM_BDTRInitStructure);

    TIM_SelectSlaveMode(ATIM1, TIM_SLAVE_MODE_TRIG);
    TIM_SelectInputTrig(ATIM1, TIM_TRIG_SEL_TI2FP2);
}

/**
*\*\name    ATIM2_Configuration.
*\*\fun     Configures the ATIM1.
*\*\param   none
*\*\return  none 
**/
void ATIM2_Configuration(void)
{
    /* Time Base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.CapCh2Sel     = TIM_CAPCH2SEL_1;
    TIM_TimeBaseStructure.Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period        = 1199;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.RepetCnt      = 0;
    TIM_InitTimeBase(ATIM2, &TIM_TimeBaseStructure);

    /* Channel 1 Configuration in PWM mode */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode       = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_DISABLE;
    TIM_OCInitStructure.Pulse        = 599;
    TIM_OCInitStructure.OCPolarity   = TIM_OC_POLARITY_LOW;
    TIM_OCInitStructure.OCNPolarity  = TIM_OCN_POLARITY_LOW;
    TIM_OCInitStructure.OCIdleState  = TIM_OC_IDLE_STATE_SET;
    TIM_OCInitStructure.OCNIdleState = TIM_OCN_IDLE_STATE_RESET;
    TIM_InitOc1(ATIM2, &TIM_OCInitStructure);

    /* Automatic Output enable, Break, dead time and lock configuration*/
    TIM_InitBkdtStruct(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.OSSRState       = TIM_OSSR_STATE_ENABLE;
    TIM_BDTRInitStructure.OSSIState       = TIM_OSSI_STATE_ENABLE;
    TIM_BDTRInitStructure.LOCKLevel       = TIM_LOCK_LEVEL_1;
    TIM_BDTRInitStructure.DeadTime        = 0;
    TIM_BDTRInitStructure.Break           = TIM_BREAK_IN_DISABLE;
    TIM_BDTRInitStructure.BreakPolarity   = TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTO_OUTPUT_DISABLE;
    TIM_ConfigBkdt(ATIM2, &TIM_BDTRInitStructure);

    TIM_SelectSlaveMode(ATIM2, TIM_SLAVE_MODE_TRIG);
    TIM_SelectInputTrig(ATIM2, TIM_TRIG_SEL_TI2FP2);
}

/**
*\*\name    GTIM8_Configuration.
*\*\fun     Configures the ATIM1.
*\*\param   none
*\*\return  none 
**/
void GTIM8_Configuration(void)
{
    /* Time Base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.CapCh2Sel     = TIM_CAPCH2SEL_4;
    TIM_TimeBaseStructure.Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period        = 1199;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.RepetCnt      = 0;
    TIM_InitTimeBase(GTIM8, &TIM_TimeBaseStructure);

    /* Channel 1 Configuration in PWM mode */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode       = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_DISABLE;
    TIM_OCInitStructure.Pulse        = 599;
    TIM_OCInitStructure.OCPolarity   = TIM_OC_POLARITY_LOW;
    TIM_OCInitStructure.OCNPolarity  = TIM_OCN_POLARITY_LOW;
    TIM_OCInitStructure.OCIdleState  = TIM_OC_IDLE_STATE_SET;
    TIM_OCInitStructure.OCNIdleState = TIM_OCN_IDLE_STATE_RESET;
    TIM_InitOc1(GTIM8, &TIM_OCInitStructure);

    /* Automatic Output enable, Break, dead time and lock configuration*/
    TIM_InitBkdtStruct(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.OSSRState       = TIM_OSSR_STATE_ENABLE;
    TIM_BDTRInitStructure.OSSIState       = TIM_OSSI_STATE_ENABLE;
    TIM_BDTRInitStructure.LOCKLevel       = TIM_LOCK_LEVEL_1;
    TIM_BDTRInitStructure.DeadTime        = 0;
    TIM_BDTRInitStructure.Break           = TIM_BREAK_IN_DISABLE;
    TIM_BDTRInitStructure.BreakPolarity   = TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTO_OUTPUT_DISABLE;
    TIM_ConfigBkdt(GTIM8, &TIM_BDTRInitStructure);

    TIM_SelectSlaveMode(GTIM8, TIM_SLAVE_MODE_TRIG);
    TIM_SelectInputTrig(GTIM8, TIM_TRIG_SEL_TI2FP2);

}

/**
*\*\name    GTIM9_Configuration.
*\*\fun     Configures the ATIM1.
*\*\param   none
*\*\return  none 
**/
void GTIM9_Configuration(void)
{
    /* Time Base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.CapCh2Sel     = TIM_CAPCH2SEL_4;
    TIM_TimeBaseStructure.Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period        = 1199;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.RepetCnt      = 0;
    TIM_InitTimeBase(GTIM9, &TIM_TimeBaseStructure);

    /* Channel 1 Configuration in PWM mode */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode       = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_DISABLE;
    TIM_OCInitStructure.Pulse        = 599;
    TIM_OCInitStructure.OCPolarity   = TIM_OC_POLARITY_LOW;
    TIM_OCInitStructure.OCNPolarity  = TIM_OCN_POLARITY_LOW;
    TIM_OCInitStructure.OCIdleState  = TIM_OC_IDLE_STATE_SET;
    TIM_OCInitStructure.OCNIdleState = TIM_OCN_IDLE_STATE_RESET;
    TIM_InitOc1(GTIM9, &TIM_OCInitStructure);

    /* Automatic Output enable, Break, dead time and lock configuration*/
    TIM_InitBkdtStruct(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.OSSRState       = TIM_OSSR_STATE_ENABLE;
    TIM_BDTRInitStructure.OSSIState       = TIM_OSSI_STATE_ENABLE;
    TIM_BDTRInitStructure.LOCKLevel       = TIM_LOCK_LEVEL_1;
    TIM_BDTRInitStructure.DeadTime        = 0;
    TIM_BDTRInitStructure.Break           = TIM_BREAK_IN_DISABLE;
    TIM_BDTRInitStructure.BreakPolarity   = TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTO_OUTPUT_DISABLE;
    TIM_ConfigBkdt(GTIM9, &TIM_BDTRInitStructure);

    TIM_SelectSlaveMode(GTIM9, TIM_SLAVE_MODE_TRIG);
    TIM_SelectInputTrig(GTIM9, TIM_TRIG_SEL_TI2FP2);

}

