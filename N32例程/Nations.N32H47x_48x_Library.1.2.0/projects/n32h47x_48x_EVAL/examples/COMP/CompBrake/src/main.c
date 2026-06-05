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
 */
#include "main.h"


void RCC_Configuration(void);
void GPIO_Configuration(void);
void EXTI_Configuration(void);
void NVIC_Configuration(void);
void COMP_Configuration(void);
void TIM_Intial(TIM_Module* TIMx);

TIM_TimeBaseInitType TIM_TimeBaseStructure;
OCInitType TIM_OCInitStructure;
TIM_BDTRInitType TIM_BDTRInitStructure;
uint16_t TimerPeriod   = 0;
uint16_t Channel1Pulse = 0, Channel2Pulse = 0;


/**
*\*\name    ChangeVmVp.
*\*\fun     Self Generate Puls ,by skip line connect to vp and vm if need.
*\*\return  none
**/
void ChangeVmVp(void)
{
    GPIO_SetBits(GPIOC, GPIO_PIN_10);
    GPIO_ResetBits(GPIOC, GPIO_PIN_11);
    {
        uint32_t i = 0;
        while (i++ < 100000)
            ;
    }
    GPIO_ResetBits(GPIOC, GPIO_PIN_10);
    GPIO_SetBits(GPIOC, GPIO_PIN_11);
    {
        uint32_t i = 0;
        while (i++ < 100000)
        ;
    }
}
/**
*\*\name    main.
*\*\fun     main program.
*\*\return  none
**/
int main(void)
{
    /* System clocks configuration ---------------------------------------------*/
    RCC_Configuration();

    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration();

    /* COMP configuration ------------------------------------------------------*/
    COMP_Configuration();
    
    /* ATIM1 configuration -----------------------------------------------------*/
    TIM_Intial(ATIM1);
    
    /* ATIM1 counter enable */
    TIM_Enable(ATIM1, ENABLE);

    /* ATIM1 Main Output Enable */
    TIM_EnableCtrlPwmOutputs(ATIM1, ENABLE);

    while (1)
    {
        ChangeVmVp();
    }
}

/**
*\*\name    COMP_Configuration.
*\*\fun     Configures the comp module.
*\*\return  none
**/
void COMP_Configuration(void)
{
    COMP_InitType COMP_Initial;

    /*Initial comp*/
    COMP_StructInit(&COMP_Initial);
    COMP_Initial.InpSel     = COMP1_CTRL_INPSEL_PA1;
    COMP_Initial.InmSel     = COMP1_CTRL_INMSEL_PA4;
    COMP_Initial.SampWindow = 18;       //(0~32)
    COMP_Initial.Threshold  = 30;       //(0~32)
    COMP_Init(COMP1, &COMP_Initial);
    COMP_Enable(COMP1, ENABLE);
    /*Enable comp1 output to timer*/
    COMP_OutToTimEnable(COMP1_TIM_EN,ENABLE);
}
/**
*\*\name    RCC_Configuration.
*\*\fun     Configures the different system clocks.
*\*\return  none
**/
void RCC_Configuration(void)
{
    /* Enable GPIOA, GPIOB, GPIOC clocks */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA | RCC_AHB_PERIPHEN_GPIOB | RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    /* Enable COMP COMPFILT AFIO clocks */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_COMPFILT | RCC_APB2_PERIPH_COMP | RCC_APB2_PERIPH_AFIO,ENABLE);
    /* ATIM1 clocks enable */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_ATIM1,ENABLE);
}

/**
*\*\name    GPIO_Configuration.
*\*\fun     Configures the different GPIO ports.
*\*\return  none
**/
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    // INP INM
    GPIO_InitStructure.Pin        = GPIO_PIN_1 | GPIO_PIN_4 ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    // PC10,PC11
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pin       = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOC, GPIO_PIN_10);
    GPIO_SetBits(GPIOC, GPIO_PIN_11);
    
    // OutSel
    GPIO_InitStructure.Pin       = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF12;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
    
    /* GPIOx Configuration: Pin of TIMx_CH1N */
    GPIO_InitStructure.Pin        = GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF5;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);    

    /* GPIOx Configuration: Pin of TIMx_CH2N */
    GPIO_InitStructure.Pin        = GPIO_PIN_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF5;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);    
}

/**
*\*\name    TIM_Intial.
*\*\fun     Configures timx program.
*\*\return  none
**/
void TIM_Intial(TIM_Module* TIMx)
{
    /* 
    TIM1 Configuration to:

    1/ Generate 3 complementary PWM signals with 3 different duty cycles:
    TIM1CLK is fixed to SystemCoreClock, the TIM1 Prescaler is equal to 0 so the
    TIM1 counter clock used is SystemCoreClock.

    The objective is to generate PWM signal at x Hz:
    - TIM1_Period = (SystemCoreClock / x) - 1

    The Three Duty cycles are computed as the following description:

    The channel 1 duty cycle is set to 50% so channel 1N is set to 50%.
    The channel 2 duty cycle is set to 25% so channel 2N is set to 75%.
    The Timer pulse is calculated as follows:
      - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

    2/ Insert a dead time equal to 11/SystemCoreClock ns
    3/ Configure the break feature, active at High level, and using the automatic
     output enable feature
    4/ Use the Locking parameters level1.
    */
    
    /* Compute the value to be set in AR register to generate signal frequency at x hz */
    TimerPeriod = (SystemCoreClock / 16000) - 1;
    /* Compute CCDAT1 value to generate a duty cycle at 50% for channel 1 */
    Channel1Pulse = (uint16_t)(((uint32_t)5 * (TimerPeriod - 1)) / 10);
    /* Compute CCDAT2 value to generate a duty cycle at 25%  for channel 2 */
    Channel2Pulse = (uint16_t)(((uint32_t)25 * (TimerPeriod - 1)) / 100);
    
    /* Time Base configuration */
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.ClkDiv   = TIM_CNT_MODE_UP;
    TIM_TimeBaseStructure.Period    = TimerPeriod;
    TIM_TimeBaseStructure.ClkDiv    = 0;
    TIM_TimeBaseStructure.RepetCnt  = 0;
    TIM_InitTimeBase(TIMx, &TIM_TimeBaseStructure);

    /* Channel 1, 2,3 and 4 Configuration in PWM mode */
    TIM_OCInitStructure.OCMode       = TIM_OCMODE_PWM2;
    TIM_OCInitStructure.OutputState  = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_ENABLE;
    TIM_OCInitStructure.Pulse        = Channel1Pulse;
    TIM_OCInitStructure.OCPolarity   = TIM_OC_POLARITY_LOW;
    TIM_OCInitStructure.OCNPolarity  = TIM_OCN_POLARITY_LOW;
    TIM_OCInitStructure.OCIdleState  = TIM_OC_IDLE_STATE_SET;
    TIM_OCInitStructure.OCNIdleState = TIM_OCN_IDLE_STATE_RESET;
    TIM_InitOc1(TIMx, &TIM_OCInitStructure);

    /*TIM_OCInitStructure.OutputNState = TIM_OUTPUT_NSTATE_DISABLE;*/
    TIM_OCInitStructure.Pulse = Channel2Pulse;
    TIM_InitOc2(TIMx, &TIM_OCInitStructure);

    /* Automatic Output enable, Break, dead time and lock configuration*/
    TIM_InitBkdtStruct(&TIM_BDTRInitStructure); 
    TIM_BDTRInitStructure.OSSRState       = TIM_OSSR_STATE_ENABLE;
    TIM_BDTRInitStructure.OSSIState       = TIM_OSSI_STATE_ENABLE;
    TIM_BDTRInitStructure.LOCKLevel       = TIM_LOCK_LEVEL_OFF;
    TIM_BDTRInitStructure.DeadTime        = 11;
    TIM_BDTRInitStructure.Break           = TIM_BREAK_IN_ENABLE;
    TIM_BDTRInitStructure.BreakPolarity   = TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStructure.AutomaticOutput = TIM_AUTO_OUTPUT_ENABLE;
    TIM_ConfigBkdt(TIMx, &TIM_BDTRInitStructure);
    
    TIM_BreakInputSourceEnable(TIMx,TIM_BREAK_COMP1,TIM_BREAK_SOURCE_POLARITY_NONINVERT,ENABLE);

}



