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



ADC_InitType ADC_InitStructure;
__IO uint16_t ADCTempValue;
DMA_InitType DMA_InitStructure;

TIM_TimeBaseInitType TIM_TimeBaseStructure;
OCInitType TIM_OCInitStructure;
uint32_t CCR4_Val       = 100;
uint32_t CCR6_Val       = 500;
uint32_t PrescalerValue = 0;

__IO uint16_t ADCConvertedValue[16][2] = {0};


void RCC_Configuration(void);
void GPIO_Configuration(void);
void PGA_Configuration(void);
void TIM_Configuration(void);
void ADC_Initial(void);



/**
*\*\name    DMA_Config.
*\*\fun     DMA_Initial program.
*\*\return  none
**/
void DMA_Config(void)
{
    DMA_DeInit(DMA1_CH1);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&ADC1->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)&ADCConvertedValue;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = 32;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(DMA1_CH1, &DMA_InitStructure);
    
    DMA_RequestRemap(DMA_REMAP_ADC1,DMA1_CH1,ENABLE);
    
    /* Enable DMA1 channel1 */
    DMA_EnableChannel(DMA1_CH1, ENABLE);
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
	
	/* DMA configuration -------------------------------------------------------*/
	DMA_Config();
	
    /* PGA configuration -------------------------------------------------------*/
    PGA_Configuration();

	/* TIM Configuration */
    TIM_Configuration();
	/* ADC configuration -------------------------------------------------------*/
    ADC_Initial();
	/* TIMx enable counter */
	TIM_EnableCtrlPwmOutputs(ATIM1, ENABLE);
	TIM_Enable(ATIM1, ENABLE);

    while (1)
    {

    }
}

/**
*\*\name    TIM_Configuration.
*\*\fun     Configures the ATIM1.
*\*\param   none
*\*\return  none 
**/
void TIM_Configuration(void)
{
	TIM_DeInit(ATIM1);
	/* Compute the prescaler value */
    PrescalerValue = (uint32_t)((SystemCoreClock) / 12000) - 1;
    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = 1000;
    TIM_TimeBaseStructure.Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CounterMode   = TIM_CNT_MODE_CENTER_ALIGN3;

    TIM_InitTimeBase(ATIM1, &TIM_TimeBaseStructure);
	TIM_ClearFlag(ATIM1, TIM_FLAG_UPDATE);
    /* PWM1 Mode configuration: Channel1 */
    TIM_InitOcStruct(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode      = TIM_OCMODE_PWM1;
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = 200;
    TIM_OCInitStructure.OCPolarity  = TIM_OC_POLARITY_HIGH;

    TIM_InitOc1(ATIM1, &TIM_OCInitStructure);

    TIM_ConfigOc1Preload(ATIM1, TIM_OC_PRE_LOAD_ENABLE);

    /* PWM1 Mode configuration: Channel6 */
    TIM_OCInitStructure.OutputState = TIM_OUTPUT_STATE_ENABLE;
    TIM_OCInitStructure.Pulse       = CCR6_Val;
    TIM_InitOc6(ATIM1, &TIM_OCInitStructure);
	
    TIM_ConfigOc6Preload(ATIM1, TIM_OC_PRE_LOAD_ENABLE);

	TIM_SelectOutputTrig(ATIM1,TIM_TRGO_SRC_UPDATE);

	TIM_ConfigArPreload(ATIM1, ENABLE);

}

/**
*\*\name    PGA_Configuration.
*\*\fun     Configures the PGA module.
*\*\return  none
**/
void PGA_Configuration(void)
{
    PGA_InitType PGA_Initial;

    /*Initial comp*/
    PGA_StructInit(&PGA_Initial);
    PGA_Initial.DiffModeEn  = DISABLE;
    PGA_Initial.Gain1       = PGA_CTRL_CH1GAIN_2_DIFF_4;
    PGA_Initial.Vpsel       = PGA1_CTRL_VPSEL_PA1; 
	PGA_Initial.Vpssel      = PGA1_CTRL_VPSSEL_PA5;
	PGA_Initial.TimeAutoMuxEn  = ENABLE ;
    PGA_Initial.En1         = DISABLE;
    PGA_Initial.En2         = DISABLE;
    PGA_Init(PGA1, &PGA_Initial);
    /*enable PGA1 CH1 and CH2 */
    PGA_Enable(PGA1,PGA_Channel1, ENABLE);
}

/**
*\*\name    ADC_Initial.
*\*\fun     ADC_Initial program.
*\*\return  none
**/
void ADC_Initial(void)
{
    /* ADCx configuration ------------------------------------------------------*/
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_ATIM1_TRGO;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_ConfigRegularChannel(ADC1, ADC_CH_1, 1, ADC_SAMP_TIME_CYCLES_2_5);
    /* Enable ADC1 channel 1 positive end connecting to output of PGA1 positive end. */
    ADC_EnableCH1PositiveEndConnetPGA_P(ADC1,ENABLE);
    /* Enable ADC1 */
    ADC_Enable(ADC1, ENABLE);
    /*Check ADC1 Ready*/
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC1 single-ended calibration */
    ADC_CalibrationOperation(ADC1,ADC_CALIBRATION_SINGLE_MODE);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC1,ADC_CALIBRATION_SINGLE_MODE))
        ;
	/* Enable ADC1 DMA */
    ADC_SetDMATransferMode(ADC1, ADC_MULTI_REG_DMA_EACH_ADC);
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
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
    /* Enable PGA clocks */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_PGA,ENABLE);
    /* Enable ADC1 clocks */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1 ,ENABLE);
    /* TIMx, DMA1 clocks enable */
	RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_ATIM1 | RCC_AHB_PERIPHEN_DMA1,ENABLE);

    /* RCC_ADCHCLK_DIV4*/
    ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB,RCC_ADCHCLK_DIV4);
    RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8);  //selsect HSI as RCC ADC1M CLK Source
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
    GPIO_InitStructure.Pin        = GPIO_PIN_1 | GPIO_PIN_5 ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}


