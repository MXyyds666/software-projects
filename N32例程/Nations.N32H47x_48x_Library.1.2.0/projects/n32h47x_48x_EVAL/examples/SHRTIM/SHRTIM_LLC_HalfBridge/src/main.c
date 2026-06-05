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

void SystemClock_Config(void);
static void SHRTIM1_Init(void);
static void GPIO_Init(void);
static void ADC_Config(void);
static void DAC_Config(void);
static void COMP_Config(void);


DemoModeTypeDef DemoState;

/**
*\*\fun Main program.
*\*\return none
**/
int main(void)
{    
    /* LED and button init */
    GPIO_Init();
    
    /* Initialize all configured peripherals */
    ADC_Config();
    
    DAC_Config();
    
    COMP_Config();

    SHRTIM1_Init();
    
    DemoState = RUN;

    /* Enable SHRTIM's outputs TA1, TA2, TB1, TB2 */
    SHRTIM_EnableOutput(SHRTIM1, \
    SHRTIM_OUTPUT_TA1 + SHRTIM_OUTPUT_TA2 + SHRTIM_OUTPUT_TB1 + SHRTIM_OUTPUT_TB2);

    /* This is a bug: delay protection cannot work when enabled before first output enable.
       note: delay protection must enable before counter enable*/
    SHRTIM_TIM_EnableDP(SHRTIM1, SHRTIM_TIMER_A);
    
    /* Start SHRTIM's TIMER A, B, C,*/
    SHRTIM_TIM_CounterEnable(SHRTIM1, SHRTIM_TIMER_MASTER + SHRTIM_TIMER_A 
                                    + SHRTIM_TIMER_B + SHRTIM_TIMER_C);

    while (1)
    {
        /* ---------------- */
        /* Fault management */
        /* ---------------- */
        if(SHRTIM1->sCommonRegs.INTSTS & SHRTIM_INTSTS_FALT1ITF)
        {
            DemoState = FAULT;
        }
        /* ---------------- */
        /* Normal operation */
        /* ---------------- */
        switch(DemoState)
        {
            case RUN:
                LED1_TOGGLE();
                LED2_OFF();
                break;

            case FAULT:
                /* If fault is triggered, operation can be re-started with the push-button */
                if (BUTTON_PRESSED())
                {
                    SHRTIM_ClearFlag_FALT1(SHRTIM1);
                    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OUTPUT_TA1 | SHRTIM_OUTPUT_TA2 | SHRTIM_OUTPUT_TB1
                                                                | SHRTIM_OUTPUT_TB2 );

                    DemoState = RUN;
                    while(BUTTON_PRESSED());
                }
                LED2_TOGGLE();
                LED1_OFF();
            break;

            default:
            break;
        }

        /* This delay defines the main loop time and LED blinking rate*/
        systick_delay_ms(50);
        
    }
}

/**
*\*\name SHRTIM1 Initialization Function
*\*\return None
**/
static void SHRTIM1_Init(void)
{
    SHRTIM1_MspInit();
    
    /* ---------------------------------------------------------------- */
    /*                   Master timer initialization                    */
    /* ---------------------------------------------------------------- */
    SHRTIM_TIM_SetPrescaler(SHRTIM1, SHRTIM_TIMER_MASTER, SHRTIM_PRESCALERRATIO_MUL8);
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_MASTER, LLC_PWM_PERIOD);
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_MASTER, REPETITON_RATE);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_MASTER, SHRTIM_MODE_CONTINUOUS);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_MASTER);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_MASTER, SHRTIM_UPDATETRIG_REPETITION);
    SHRTIM_EnableIT_REP(SHRTIM1, SHRTIM_TIMER_MASTER);
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_MASTER, (65 * LLC_PWM_PERIOD)/100); 
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_MASTER, (85 * LLC_PWM_PERIOD)/100); 
    SHRTIM_TIM_EnableHalfMode(SHRTIM1, SHRTIM_TIMER_MASTER);

    /* ---------------------------------------------------------------------- */
    /*                        TIMERA initialization:                          */
    /* ---------------------------------------------------------------------- */
    SHRTIM_TIM_SetPrescaler(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_PRESCALERRATIO_MUL8);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_MODE_CONTINUOUS);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_A);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_UPDATETRIG_MASTER);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_RESETTRIG_MASTER_PER + SHRTIM_RESETTRIG_MASTER_CMP1);
    SHRTIM_TIM_EnableDeadTime(SHRTIM1, SHRTIM_TIMER_A);
    SHRTIM_DT_SetPrescaler(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_DT_PRESCALER_MUL4);
    SHRTIM_DT_SetRisingValue(SHRTIM1, SHRTIM_TIMER_A, DT_RISING);
    SHRTIM_DT_SetFallingValue(SHRTIM1, SHRTIM_TIMER_A, DT_FALLING);
    SHRTIM_DT_LockRisingSign(SHRTIM1, SHRTIM_TIMER_A);
    SHRTIM_DT_LockFallingSign(SHRTIM1, SHRTIM_TIMER_A);
    
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TA1, SHRTIM_OUTPUTSET_MASTERPER);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TA1, SHRTIM_OUTPUTRESET_MASTERCMP1);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TA1, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TA2, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    
    SHRTIM_TIM_SetDPMode(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_DP_DELAYBOTH_EXEV6);
    
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_FAULT_1);

    /* ----------------------------------- */
    /*       TIMERB initialization:        */
    /* ----------------------------------- */
    SHRTIM_TIM_SetPrescaler(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_PRESCALERRATIO_MUL8);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_MODE_RETRIGGERABLE);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_B);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_UPDATETRIG_MASTER);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_RESETTRIG_MASTER_PER);
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TB1, SHRTIM_OUTPUTRESET_TIMCMP1);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TB1, SHRTIM_OUTPUTRESET_TIMCMP2);
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TB2, SHRTIM_OUTPUTRESET_MASTERCMP3);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TB2, SHRTIM_OUTPUTRESET_MASTERCMP4);    
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TB1, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TB2, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_FAULT_1);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_B, (15 * LLC_PWM_PERIOD)/100); /* Turn-on at 5%*/
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_B, (35 * LLC_PWM_PERIOD)/100); /* Turn-off at 45%*/
    
    
    /* ----------------------------------- */
    /*       TIMERC initialization:        */
    /* ----------------------------------- */
    SHRTIM_TIM_SetPrescaler(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_PRESCALERRATIO_MUL8);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_MODE_RETRIGGERABLE);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_C);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_UPDATETRIG_MASTER);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_RESETTRIG_MASTER_PER);
    
    /* ---------------------------------------------------------- */
    /* External event 6 init： source is COMP1_out, rising edge   */
    /* ---------------------------------------------------------- */
    SHRTIM_EXEV_Config(SHRTIM1,SHRTIM_EVENT_6, SHRTIM_EXEV_SOURCE_2, SHRTIM_EXEVXSRC2_COMP1_OUT,
    SHRTIM_EXEV_POLARITY_HIGH, SHRTIM_EXEV_SENSITIVITY_LEVEL, SHRTIM_EXEV_FASTMODE_DISABLE,
    SHRTIM_EXEV_FILTER_NONE, SHRTIM_EXEV_PRESCALER_DIV1);
    

    /* ----------------------------------- */
    /*       Configuration of ADC trig     */
    /* ----------------------------------- */
    /* Set compare 3 for sampling before turn-on on SR1 */
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_B, (15 * LLC_PWM_PERIOD)/100 - ADC_CONVERSION_TIME);
    /* Set compare 4 for sampling before turn-off on SR1 */
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_B, (15 * LLC_PWM_PERIOD)/100 + ADC_CONVERSION_TIME);
    /* Compare 2 is used as ADC trigger for SR1 sampling before turn-off */
    /* 1st value is set above period to avoid a 1st ADC sampling */
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_MASTER, LLC_PWM_PERIOD + 0x20); 
    /* TIMERB period event can serve as */
    /* ADC trigger (to optimize timer's ressources). TimerB Period event is used */
    /* as 4th sampling point for SR1 monitoring */
    /* Period event is used sampling after turn-off on SR1 */
    SHRTIM_TIM_SetCompare5(SHRTIM1, SHRTIM_TIMER_B, (35 * LLC_PWM_PERIOD)/100 + ADC_CONVERSION_TIME);

    /* Set compare registers for 1st sample before SR2 turn-on */
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_C, (65 * LLC_PWM_PERIOD)/100 - ADC_CONVERSION_TIME);
    /* Set compare registers for 2nd sample after SR2 turn-on */
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_C, (65 * LLC_PWM_PERIOD)/100 + ADC_CONVERSION_TIME);
    /* Set compare registers for 3rd sample before SR2 turn-off */
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_C, (85 * LLC_PWM_PERIOD)/100 - ADC_CONVERSION_TIME);
    /* TIMERC period event can serve as */
    /* ADC trigger (to optimize timer's ressources). TimerC Period event is used */
    /* as 4th sampling point for SR2 monitoring */
    /* Period event is used sampling after turn-off on SR2 */
    SHRTIM_TIM_SetCompare5(SHRTIM1, SHRTIM_TIMER_C, (85 * LLC_PWM_PERIOD)/100 + ADC_CONVERSION_TIME);

    /* ADC trigger 2 update source: Master */
    /* ADC trigger event: Timer B compare 3, Timer B compare 4, master compare 2, Timer B period */
    SHRTIM_ConfigADCTrig(SHRTIM1, SHRTIM_ADCTRIG2_SOURCE_GROUP1, SHRTIM_ADCTRIG_2, SHRTIM_ADCTRIG_UPDATE_MASTER,
                          SHRTIM_ADTG13_SOURCE_GROUP1_TBCMP3 | SHRTIM_ADTG13_SOURCE_GROUP1_TBCMP4 
                        | SHRTIM_ADTG13_SOURCE_GROUP1_MCMP2  | SHRTIM_ADTG13_SOURCE_GROUP1_TBCMP5);  
    
    /* ADC trigger 4 update source: Master */
    /* ADC trigger event: Timer C compare 2, Timer C compare 3, Timer C compare 4, Timer C period */    
    SHRTIM_ConfigADCTrig(SHRTIM1, SHRTIM_ADCTRIG4_SOURCE_GROUP1, SHRTIM_ADCTRIG_4, SHRTIM_ADCTRIG_UPDATE_MASTER, \
                                  SHRTIM_ADTG24_SOURCE_GROUP1_TCCMP2   | SHRTIM_ADTG24_SOURCE_GROUP1_TCCMP3
                                | SHRTIM_ADTG24_SOURCE_GROUP1_TCCMP4 | SHRTIM_ADTG24_SOURCE_GROUP1_TCCMP5);


    /* ---------------------------------------------------------- */
    /* FAULT1 global init: no filter, high polarity, Fault1 enable */
    /* ---------------------------------------------------------- */
    SHRTIM_FALT_SetPolarity(SHRTIM1, SHRTIM_FAULT_1, SHRTIM_FALT_POLARITY_HIGH);
    SHRTIM_FALT_Enable(SHRTIM1, SHRTIM_FAULT_1);

    SHRTIM_FALT_EnableSysFault(SHRTIM1,SHRTIM_SYSTEM_FAULT_CLOCKSECURITY_ENABLE);
    /* ---------------*/
    /* SHRTIM start-up */
    /* ---------------*/
    /* Force register update before starting */
    SHRTIM_ForceUpdate(SHRTIM1, SHRTIM_TIMER_A | SHRTIM_TIMER_B | SHRTIM_TIMER_C);
}

/**
*\*\name    DAC_Configuratoin.
*\*\fun     Configures the DAC1.
*\*\return  none
**/
void DAC_Config(void)
{
    DAC_InitType DAC_InitStructure;

    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_DAC12 ,ENABLE);

    /* Config DAC12 prescaler factor,DAC_CLK = 1M*/
    DAC_ConfigClkPrescaler(DAC12, 100);
    DAC_SetHighFrequencyMode(DAC12, DAC_HIGH_FREQ_MODE_BELOW_160M);

    DAC_StructInit(&DAC_InitStructure);

    /* DAC1 Configuration */
    DAC_InitStructure.DAC_TriggerEnable                 = DISABLE;
    DAC_InitStructure.DAC_WaveGeneration                = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_ConnectOnChipPeripheral       = ENABLE;
    DAC_InitStructure.DAC_OutputBuffer                  = DISABLE;
    DAC_Init(DAC1, &DAC_InitStructure);

    /* Enable DAC1  */
    DAC_Enable(DAC1, ENABLE);   

    /* Set DAC1 DHR12R register */
    DAC_SetData(DAC1,DAC_ALIGN_R_12BIT, 0x4FF);
}

/**
*\*\name    COMP_Configuratoin.
*\*\fun     Configures the analog comparator.
*\*\return  none
**/
void COMP_Config(void)
{       
    COMP_InitType COMP_Initial;

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_COMPFILT | RCC_APB2_PERIPH_COMP ,ENABLE);

    /*Initial comp*/
    COMP_StructInit(&COMP_Initial);
    COMP_Initial.InpSel     = COMP1_CTRL_INPSEL_PA1;
    COMP_Initial.InmSel     = COMP1_CTRL_INMSEL_DAC1_IOUT;
    COMP_Initial.SampWindow = 4;       //(0~32)
    COMP_Initial.Threshold  = 8;       //(0~32)
    COMP_Init(COMP1, &COMP_Initial);

    /* Enable the connection between COMP1 out and timer */
    COMP_OutToTimEnable(COMP1_TIM_EN, ENABLE);

    COMP_Enable(COMP1, ENABLE);
}

/**
*\*\name    ADC_Configuratoin.
*\*\fun     Configures the ADC1 and ADC2.
*\*\return  none
**/
void ADC_Config(void)
{
    ADC_InitType ADC_InitStructure;
    ADC_InitStruct(&ADC_InitStructure);

    ADC_MspInit();

    /* ADC1 and ADC2 configuration ------------------------------------------------------*/
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = ENABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Init(ADC2, &ADC_InitStructure);

    /* Enable the discontinuous mode of ADC1 and ADC2 */
    ADC_EnableInjectedDiscMode(ADC1, ENABLE);
    ADC_EnableInjectedDiscMode(ADC2, ENABLE);

    /* Set injected sequencer length of ADC1 and ADC2*/
    ADC_ConfigInjectedSequencerLength(ADC1, 4);
    ADC_ConfigInjectedSequencerLength(ADC2, 4);

    /* Injected channel configuration of ADC1 and ADC2 */
    ADC_ConfigInjectedChannel(ADC1, ADC1_Channel_03_PA0, 1, ADC_SAMP_TIME_CYCLES_2_5);
    ADC_ConfigInjectedChannel(ADC1, ADC1_Channel_03_PA0, 2, ADC_SAMP_TIME_CYCLES_2_5);
    ADC_ConfigInjectedChannel(ADC1, ADC1_Channel_03_PA0, 3, ADC_SAMP_TIME_CYCLES_2_5);
    ADC_ConfigInjectedChannel(ADC1, ADC1_Channel_03_PA0, 4, ADC_SAMP_TIME_CYCLES_2_5);

    ADC_ConfigInjectedChannel(ADC2, ADC2_Channel_00_PA6, 1, ADC_SAMP_TIME_CYCLES_2_5);
    ADC_ConfigInjectedChannel(ADC2, ADC2_Channel_00_PA6, 2, ADC_SAMP_TIME_CYCLES_2_5);
    ADC_ConfigInjectedChannel(ADC2, ADC2_Channel_00_PA6, 3, ADC_SAMP_TIME_CYCLES_2_5);
    ADC_ConfigInjectedChannel(ADC2, ADC2_Channel_00_PA6, 4, ADC_SAMP_TIME_CYCLES_2_5);

    /* ADC1 injected external trigger configuration */
    ADC_ConfigExternalTrigInjectedConv(ADC1, ADC_EXT_TRIG_INJ_CONV_SHRTIM_TRGO2);
    ADC_ConfigExternalTrigInjectedConv(ADC2, ADC_EXT_TRIG_INJ_CONV_SHRTIM_TRGO4);

    /* Enable ADC1 */
    ADC_Enable(ADC1, ENABLE);

    /*Check ADC Ready*/
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC1 calibration */
    ADC_CalibrationOperation(ADC1,ADC_CALIBRATION_SINGLE_MODE);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC1, ADC_CALIBRATION_SINGLE_MODE))
        ;

    ADC_Enable(ADC2, ENABLE);

    /*Check ADC Ready*/
    while(ADC_GetFlagStatus(ADC2,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC2 calibration */
    ADC_CalibrationOperation(ADC2,ADC_CALIBRATION_SINGLE_MODE);
    /* Check the end of ADC2 calibration */
    while (ADC_GetCalibrationStatus(ADC2, ADC_CALIBRATION_SINGLE_MODE))
        ;
}

/**
*\*\fun GPIO Initialization Function
*\*\return None
**/
void GPIO_Init(void)
{
    GPIO_InitType GPIO_InitStructure;
    
    GPIO_InitStruct(&GPIO_InitStructure);

    /* GPIO Ports Clock Enable */
    LED1_CLOCK_ENABLE();
    LED2_CLOCK_ENABLE();
    LED3_CLOCK_ENABLE();
    BUTTON_CLOCK_ENABLE();

    /* The PB4 of N32H474VEL7-STB V1.0 is connected to the download chip. To enable PB4 for LED output, JTAG-DP must be disabled and SW-DP must be enabled. */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    GPIO_ConfigPinRemap(0,0,GPIO_RMP_SWJ_SWD);

    GPIO_InitStructure.Pin = LED1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(LED1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED2_PIN;
    GPIO_InitPeripheral(LED2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED3_PIN;
    GPIO_InitPeripheral(LED3_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin = BUTTION_PIN;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitPeripheral(BUTTION_PORT, &GPIO_InitStructure);
}

