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
__IO uint16_t ADC1ConvertedValue[3];

void RCC_Configuration(void);
void GPIO_Configuration(void);
uint16_t ADC_GetData(ADC_Module* ADCx, uint8_t ADC_Channel);

/**
*\*\name    ADC_Initial.
*\*\fun     ADC_Initial program.
*\*\return  none
**/
void ADC_Initial(ADC_Module* ADCx)
{
	ADC_InitStruct(&ADC_InitStructure);
    /* ADCx configuration ------------------------------------------------------*/
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = DISABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_SOFTWARE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = 1;
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    ADC_Init(ADCx, &ADC_InitStructure);

    /* Enable ADCx */
    ADC_Enable(ADCx, ENABLE);
    /*Check ADC Ready*/
    while(ADC_GetFlagStatus(ADCx,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADCx calibration */
    ADC_CalibrationOperation(ADCx,ADC_CALIBRATION_SINGLE_MODE);
    /* Check the end of ADCx calibration */
    while (ADC_GetCalibrationStatus(ADCx,ADC_CALIBRATION_SINGLE_MODE))
        ;
}

int main(void)
{
    /* System clocks configuration ----------------------------------------------*/
    RCC_Configuration();

    /* GPIO configuration -------------------------------------------------------*/
    GPIO_Configuration();

    /* ADC1  configuration -----------------------------------------*/
    ADC_Initial(ADC1);
    
    while (1)
    {
        ADC1ConvertedValue[0]=ADC_GetData(ADC1,ADC1_Channel_01_PB0);
        ADC1ConvertedValue[1]=ADC_GetData(ADC1,ADC1_Channel_12_PB1);
        
    }
}


/**
*\*\name    RCC_Configuration.
*\*\fun     Configures the different system clocks.
*\*\return  none
**/
void RCC_Configuration(void)
{
    /* Enable peripheral clocks ------------------------------------------------*/

    /* Enable GPIOA, GPIOB, GPIOC ,GPIOD clocks */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA | RCC_AHB_PERIPHEN_GPIOB | RCC_AHB_PERIPHEN_GPIOC | RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    
    /* Enable ADC1 clocks */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1 ,ENABLE);

    /* RCC_ADCHCLK_DIV16*/
    ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB,RCC_ADCHCLK_DIV16);
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

	GPIO_InitStruct(&GPIO_InitStructure);
    /* Configure PB0 PB1 as analog input -----------------------*/
    GPIO_InitStructure.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
}

/**
*\*\name    ADC_Config_And_Get_Data.
*\*\fun     Configures ADC and get ADC conversion results.
*\*\param   ADCx :
*\*\          - ADC1
*\*\          - ADC2
*\*\          - ADC3
*\*\          - ADC4
*\*\param   ADC_Channel :
*\*\          - ADC_CH_0 : ADC Channel0 selected
*\*\          - ADC_CH_1 : ADC Channel1 selected
*\*\          - ADC_CH_2 : ADC Channel2 selected
*\*\          - ADC_CH_3 : ADC Channel3 selected
*\*\          - ADC_CH_4 : ADC Channel4 selected
*\*\          - ADC_CH_5 : ADC Channel5 selected
*\*\          - ADC_CH_6 : ADC Channel6 selected
*\*\          - ADC_CH_7 : ADC Channel7 selected
*\*\          - ADC_CH_8 : ADC Channel8 selected
*\*\          - ADC_CH_9 : ADC Channel9 selected
*\*\          - ADC_CH_10 : ADC Channel10 selected
*\*\          - ADC_CH_11 : ADC Channel11 selected
*\*\          - ADC_CH_12 : ADC Channel12 selected
*\*\          - ADC_CH_13 : ADC Channel13 selected
*\*\          - ADC_CH_14 : ADC Channel14 selected
*\*\          - ADC_CH_15 : ADC Channel15 selected
*\*\          - ADC_CH_16 : ADC Channel16 selected
*\*\          - ADC_CH_17 : ADC Channel17 selected
*\*\          - ADC_CH_18 : ADC Channel18 selected
*\*\return  The Data conversion value.
**/
uint16_t ADC_GetData(ADC_Module* ADCx, uint8_t ADC_Channel)
{
    uint16_t dat;
    
    ADC_ConfigRegularChannel(ADCx, ADC_Channel, 1, ADC_SAMP_TIME_CYCLES_239_5);
    /* Start ADC Software Conversion */
    ADC_EnableSoftwareStartConv(ADCx, ENABLE);
    while(ADC_GetFlagStatus(ADCx, ADC_FLAG_ENDC)==0){
    }
    ADC_ClearFlag(ADCx, ADC_FLAG_ENDC);
    ADC_ClearFlag(ADCx, ADC_FLAG_STR);
    dat=ADC_GetDat(ADCx);
    return dat;
}


