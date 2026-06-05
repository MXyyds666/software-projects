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
#include "delay.h"

#ifdef N32H474
#define TIMD_PERIOD         (0xFFFFU)

#define SHRTIM_INPUT_CLOCK  (250000000U)
#define TIMD_PWM_FREQ       (2000U)
#define TIMD_DUTY_CYCLE     (0.5)

#define CMP2_VALUE          (TIMD_PERIOD/100)
#else
#define DAC_Step_Num        (150)
#endif


void RCC_Configuration(void);
void GPIO_Configuration(void);
void DAC_Configuratoin(void);
#ifdef N32H474
void SHRTIM1_Init(void);
#else
void TIM_Configuration(void);
void NVIC_Configuration(void);

uint8_t  DAC_Reset_Flag = 0;
uint16_t  DAC_Step_Index = 0;
#endif
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
    /* DAC configuration -------------------------------------------------------*/
    DAC_Configuratoin();
    #ifdef N32H474
    /* SHRTIM1_Init all configured peripherals */
    SHRTIM1_Init();
    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OUTPUT_TD1);
    SHRTIM_TIM_CounterEnable(SHRTIM1, SHRTIM_TIMER_D);
    #else
    NVIC_Configuration();
    TIM_Configuration();
    #endif
    
    while (1)
    {
        #if defined(N32H474) || defined(N32H488)
        
        #else
        if(DAC_Reset_Flag == 1)
        {
            if(DAC_Step_Index < DAC_Step_Num )
            {   
                DAC_SoftTrgSawStepEnable(DAC1,ENABLE);
                systick_delay_ms(2);
                DAC_Step_Index++;
            }
            else
            {
                DAC_Step_Index = 0;
                DAC_Reset_Flag = 0;
            }
        }
        #endif
    }
}

/**
*\*\name    DAC_Configuratoin.
*\*\fun     Configures the DAC1 module.
*\*\return  none
**/
void DAC_Configuratoin(void)
{
    DAC_InitType DAC_InitStructure;

    DAC_StructInit(&DAC_InitStructure);
    /* DAC1 Configuration */
    #ifdef N32H474
    DAC_InitStructure.DAC_Trigger                       = DAC_Trigger_SHRTIM1_RST_TRG4;
    DAC_InitStructure.DAC_Trigger2                      = DAC_Trigger2_SHRTIM1_STEP_TRG4;
    #else
    DAC_InitStructure.DAC_Trigger                       = DAC_Trigger_ATIM1_TRGO;
    DAC_InitStructure.DAC_Trigger2                      = DAC_Trigger2_Software;
    #endif
    DAC_InitStructure.DAC_WaveGeneration                = DAC_WaveGeneration_SAWTOOTH_INC;
    DAC_InitStructure.DAC_OutputBuffer                  = ENABLE;
    DAC_InitStructure.DAC_ConnectOnChipPeripheral       = DISABLE;
    DAC_InitStructure.DAC_ConnectExternalPin            = ENABLE;
    DAC_InitStructure.DAC_TriggerEnable                 = ENABLE;
    DAC_Init(DAC1, &DAC_InitStructure);
    DAC_SetSawtoothResetValue(DAC1,0x100);
    DAC_SetSawtoothStepValue(DAC1,60);
    /* Enable DAC1  */
    DAC_Enable(DAC1, ENABLE);
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
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_ATIM1,ENABLE);
    /* Enable DAC1 DAC2 clocks */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_DAC12 ,ENABLE);
    /* Config DAC12 prescaler factor,DAC_CLK = 1M*/
    #if defined (N32H473) || defined (N32H474)
    DAC_ConfigClkPrescaler(DAC12,100);
    #elif defined (N32H475) || defined (N32H482) || defined (N32H487) || defined (N32H481) || defined (N32H488)
    DAC_ConfigClkPrescaler(DAC12,120);
    #endif
    DAC_SetHighFrequencyMode(DAC12,DAC_HIGH_FREQ_MODE_BELOW_160M);
}

/**
*\*\name    GPIO_Configuration.
*\*\fun     Configures the different GPIO ports.
*\*\return  none
**/
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;  
    /* DAC1_OUT */
	GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin        = GPIO_PIN_4  ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

#ifdef N32H474
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
    
    RCC_DeInit();
    
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

    /* Configure SHRTIM output: TD1 (PB14) */
    GPIO_InitStructure.Pin = GPIO_PIN_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF10;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);

    /* Initialize SHRTPLL */
    SHRTPLL_Init(RCC_SHRTPLL_SRC_HSE, HSE_VALUE, SHRTIM_INPUT_CLOCK);

    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_SHRTIM, ENABLE);
}
/**
*\*\name SHRTIM1 Initialization Function
*\*\return None
**/
void SHRTIM1_Init(void)
{
    SHRTIM1_MspInit();
    
    SHRTIM_TIM_SetPrescaler(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_PRESCALERRATIO_DIV4);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_MODE_CONTINUOUS);
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_D, TIMD_PERIOD);
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_D, 0x00);
    SHRTIM_TIM_SetUpdateGating(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_UPDATEGATING_INDEPENDENT);
    SHRTIM_TIM_SetCountingMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_COUNTING_MODE_UP);
    SHRTIM_TIM_SetComp1Mode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_GTCMP1_EQUAL);
    SHRTIM_TIM_SetDACTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_DACTRIG_DACTRIGOUT_1);      
    SHRTIM_TIM_EnableDualDacTrigger(SHRTIM1, SHRTIM_TIMER_D);    
    SHRTIM_TIM_DisableHalfMode(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetInterleavedMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_INTERLEAVED_MODE_DISABLED);
    SHRTIM_TIM_DisableStartOnSync(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_DisableResetOnSync(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_DisablePreload(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_DisableResyncUpdate(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_UPDATETRIG_NONE | SHRTIM_UPDATETRIG_NONE);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_RESETTRIG_NONE);
    SHRTIM_TIM_DisablePushPullMode(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_DisableDeadTime(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetBurstModeOption(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_BURSTMODE_MAINTAINCLOCK);
    SHRTIM_ForceUpdate(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_EnableResyncUpdate(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_D, (uint32_t)(TIMD_PERIOD * TIMD_DUTY_CYCLE));
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_D, CMP2_VALUE);  //
    SHRTIM_OUT_SetPolarity(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_POSITIVE_POLARITY);
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTSET_TIMPER);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTRESET_TIMCMP1);
    SHRTIM_OUT_SetIdleLevel(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_IDLELEVEL_INACTIVE);
    SHRTIM_OUT_SetFaultState(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_FAULTSTATE_NO_ACTION);
    SHRTIM_OUT_SetChopperMode(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_CHOPPERMODE_DISABLED);
}

#else
/**
*\*\name    NVIC_Configuration.
*\*\fun     Configures the nested vectored interrupt controller.
*\*\param   none
*\*\return  none 
**/
void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* Enable the TIMx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = ATIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
*\*\name    TIM_Configuration.
*\*\fun     Configures the ATIM1.
*\*\param   none
*\*\return  none 
**/
void TIM_Configuration(void)
{
    /* Compute the prescaler value */
    uint16_t  PrescalerValue = 0;
    TIM_TimeBaseInitType TIM_TimeBaseStructure;

    PrescalerValue = (uint32_t)(SystemCoreClock / 5000) - 1;
    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period        = 1500;         //300ms Trigger DAC
    TIM_TimeBaseStructure.Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.ClkDiv        = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(ATIM1, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_ConfigPrescaler(ATIM1, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    TIM_SelectOutputTrig(ATIM1,TIM_TRGO_SRC_UPDATE);
    /* ATIM1 enable update irq */
    TIM_ConfigInt(ATIM1, TIM_INT_UPDATE, ENABLE);

    /* ATIM1 enable counter */
    TIM_Enable(ATIM1, ENABLE);
}
#endif
