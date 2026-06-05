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
TIM_ICInitType TIM_ICInitStructure;

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
    Timers synchronisation in cascade mode with an external trigger 
    1/TIMx is configured as Master Timer:
     - Toggle Mode is used
     - The TIMx Enable event is used as Trigger Output

    2/TIMx is configured as Slave Timer for an external Trigger connected
     to TIMx TI2 pin (TIMx CH2 configured as input pin):
     - The TIMx TI2FP2 is used as Trigger Input
     - Rising edge is used to start and stop the TIMx: Gated Mode.

    3/TIMy is slave for TIMx and Master for TIMz,
     - Toggle Mode is used
     - The ITRx(TIMx) is used as input trigger
     - Gated mode is used, so start and stop of slave counter
       are controlled by the Master trigger output signal(TIMx enable event).
     - The TIMy enable event is used as Trigger Output.

    4/TIMz is slave for TIMy,
     - Toggle Mode is used
     - The ITRx(TIMy) is used as input trigger
     - Gated mode is used, so start and stop of slave counter
       are controlled by the Master trigger output signal(TIMy enable event).

    The starts and stops of the TIMx counters are controlled by the
    external trigger.
    The TIMy starts and stops are controlled by the TIMx, and the TIMz
    starts and stops are controlled by the TIMy.
    */

    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = 73;
    TIM_TimeBaseStructure.Prescaler = 2;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIMx, &TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.Period = 73;
    TIM_InitTimeBase(TIMy, &TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.Period = 73;
    TIM_InitTimeBase(TIMz, &TIM_TimeBaseStructure);

    /* Master Configuration in Toggle Mode */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_TOGGLE;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 64;
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;

    TIM_InitOc1(TIMx, &TIM_OCInitStructure);
    
    /* TIMx Input Capture Configuration */
    TIM_InitIcStruct(&TIM_ICInitStructure);
    TIM_ICInitStructure.Channel     = TIM_CH_2;
    TIM_ICInitStructure.ICPolarity  = TIM_IC_POLARITY_RISING;
    TIM_ICInitStructure.ICSelection = TIM_IC_SELECTION_DIRECTTI;
    TIM_ICInitStructure.ICPrescaler = TIM_IC_PSC_DIV1;
    TIM_ICInitStructure.ICFilter    = 0;

    TIM_ICInit(TIMx, &TIM_ICInitStructure);    

    /* TIMx Input trigger configuration: External Trigger connected to TI2 */
    TIM_SelectInputTrig(TIMx, TIM_TRIG_SEL_TI2FP2);
    TIM_SelectSlaveMode(TIMx, TIM_SLAVE_MODE_GATED);
    
    /* Select the Master Slave Mode */
    TIM_SelectMasterSlaveMode(TIMx, TIM_MASTER_SLAVE_MODE_ENABLE);    
    
    /* Master Mode selection: TIMx */
    TIM_SelectOutputTrig(TIMx, TIM_TRGO_SRC_ENABLE);  

    /* Slaves Configuration: Toggle Mode */
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_TOGGLE;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;

    TIM_InitOc1(TIMy, &TIM_OCInitStructure);

    TIM_InitOc1(TIMz, &TIM_OCInitStructure);
    
    /* Slave Mode selection: TIMy */
    TIM_SelectInputTrig(TIMy, TIM_TRIG_SEL_IN_TR0);
    TIM_SelectSlaveMode(TIMy, TIM_SLAVE_MODE_GATED);    

    /* Select the Master Slave Mode */
    TIM_SelectMasterSlaveMode(TIMy, TIM_MASTER_SLAVE_MODE_ENABLE);

    /* Master Mode selection: TIMy */
    TIM_SelectOutputTrig(TIMy, TIM_TRGO_SRC_ENABLE);

    /* Slave Mode selection: TIMz */
    TIM_SelectInputTrig(TIMz, TIM_TRIG_SEL_IN_TR12);
    TIM_SelectSlaveMode(TIMz, TIM_SLAVE_MODE_GATED);

    /* TIM1 Main Output Enable */
    TIM_EnableCtrlPwmOutputs(TIMx, ENABLE);

    /* TIM enable counter */
    TIM_Enable(TIMx, ENABLE);
    TIM_Enable(TIMy, ENABLE);
    TIM_Enable(TIMz, ENABLE);   
    
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
    RCC_EnableAHBPeriphClk(TIMx_CLK,ENABLE);
    RCC_EnableAPB1PeriphClk(TIMy_CLK | TIMz_CLK,ENABLE);
    RCC_EnableAHB1PeriphClk(TIMx_CH1_GPIO_CLK | TIMx_CH2_GPIO_CLK |\
                            TIMy_CH1_GPIO_CLK | TIMz_CH1_GPIO_CLK,ENABLE);
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
    
    GPIO_InitStructure.Pin        = TIMy_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Alternate = TIMy_CH1_GPIO_AF;
    GPIO_InitPeripheral(TIMy_CH1_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin        = TIMz_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Alternate = TIMz_CH1_GPIO_AF;
    GPIO_InitPeripheral(TIMz_CH1_GPIO_PORT, &GPIO_InitStructure);
    
}


