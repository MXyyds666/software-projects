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

/* Size of array aADCxConvertedValues[]: set to ADC sequencer number of ranks converted, to have a rank in each address */
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint32_t)5)

/* Define the interrupt rate vs switching frequency */
#define REPETITON_RATE ((uint32_t)   31)

#define BURST_PERIOD ((uint32_t)   1)
#define BURST_IDLE ((uint32_t)   0)

/* Buffer containing ADC conversions results */
__IO uint16_t aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];

void SystemClock_Config(void);
static void SHRTIM1_Init(void);
static void ADC_Config(void);
static void GPIO_Init(void);

DemoModeTypeDef DemoState;

/**
*\*\fun Main program.
*\*\return none
**/
int main(void)
{    
    /* LED and button init */
    GPIO_Init();

    /* Initialize ADC to be triggered by the SHRTIMER */
    ADC_Config();
    
    /* Demo starts in 5-phase mode */
    DemoState = FIVE_PHASE;
    
    /* Initialize all configured peripherals */
    SHRTIM1_Init();
    
    /* Enable SHRTIM's outputs TA2, TB1, TC2, TD1, TD2 */
    SHRTIM_EnableOutput(SHRTIM1, \
    SHRTIM_OUTPUT_TA2 + SHRTIM_OUTPUT_TB1 + SHRTIM_OUTPUT_TC2 + SHRTIM_OUTPUT_TD1 + SHRTIM_OUTPUT_TD2);

    /* Start SHRTIM's TIMER A, B, C, D */
    SHRTIM_TIM_CounterEnable(SHRTIM1, SHRTIM_TIMER_MASTER + SHRTIM_TIMER_A + SHRTIM_TIMER_B\
                             + SHRTIM_TIMER_C + SHRTIM_TIMER_D);
    
    while (1)
    {
        if(SHRTIM1->sCommonRegs.INTSTS & SHRTIM_INTSTS_FALT1ITF)
        {
            DemoState = FAULT;
        }

        /* ---------------- */
        /* Normal operation */
        /* ---------------- */
        switch(DemoState)
        {
            case FIVE_PHASE:
                /* Phase shedding is done in the Master timer IRQ */
                if (BUTTON_PRESSED())
                {
                    DemoState = FOUR_PHASE_INIT;
                    while(BUTTON_PRESSED());
                }
                LED2_TOGGLE();
                LED1_OFF();
                break;
        
            case FOUR_PHASE:
                if (BUTTON_PRESSED())
                {
                    DemoState = THREE_PHASE_INIT;
                    while(BUTTON_PRESSED());
                }
                LED2_OFF();
                LED1_TOGGLE();
                break;

            case THREE_PHASE:
                if (BUTTON_PRESSED())
                {
                    DemoState = TWO_PHASE_INIT;
                    while(BUTTON_PRESSED());
                }
                LED2_ON();
                LED1_OFF();
                break;

            case TWO_PHASE:
                if (BUTTON_PRESSED())
                {
                    DemoState = ONE_PHASE_INIT;
                    while(BUTTON_PRESSED());
                }
                LED2_OFF();
                LED1_ON();
                break;

            case ONE_PHASE:
                if (BUTTON_PRESSED())
                {
                    DemoState = BURST_INIT;
                    while(BUTTON_PRESSED());
                }
                LED2_ON();
                LED1_ON();
                break;

            case BURST:
                if (BUTTON_PRESSED())
                {
                    DemoState = FIVE_PHASE_INIT;
                    while(BUTTON_PRESSED());
                }
                LED2_TOGGLE();
                LED1_TOGGLE();
                break;

            case FIVE_PHASE_INIT:
            case FOUR_PHASE_INIT:
            case THREE_PHASE_INIT:
            case TWO_PHASE_INIT:
            case ONE_PHASE_INIT:
            case BURST_INIT:
            break;

            case FAULT:
                /* If fault is triggered, operation can be re-started with the push-button */
                if (BUTTON_PRESSED())
                {
                    SHRTIM_ClearFlag_FALT1(SHRTIM1);
                    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OUTPUT_TA2 | SHRTIM_OUTPUT_TB1 | SHRTIM_OUTPUT_TC2
                                                                   | SHRTIM_OUTPUT_TD1 | SHRTIM_OUTPUT_TD2);

                    DemoState = FIVE_PHASE_INIT;
                    while(BUTTON_PRESSED());
                }
                LED2_OFF();
                LED1_OFF();
                break;

            default:
                SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OUTPUT_TA2 | SHRTIM_OUTPUT_TB1 | SHRTIM_OUTPUT_TC2
                                                               | SHRTIM_OUTPUT_TD1 | SHRTIM_OUTPUT_TD2);
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
    /* Master timer initialization: PWM frequency and mode (continuous) */
    /* Update on repetition, interrupt issued on repetition event       */
    /* ---------------------------------------------------------------- */
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_MASTER, MULTIPHASE_PERIOD);
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_MASTER, REPETITON_RATE);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_MASTER, SHRTIM_MODE_CONTINUOUS);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_MASTER);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_MASTER, SHRTIM_UPDATETRIG_REPETITION);
    SHRTIM_EnableIT_REP(SHRTIM1, SHRTIM_TIMER_MASTER);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_MASTER, PHASE_SHIFT_INIT);
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_MASTER, 2 * PHASE_SHIFT_INIT);
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_MASTER, 3 * PHASE_SHIFT_INIT);
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_MASTER, 4 * PHASE_SHIFT_INIT);

    /* ---------------------------------------------------------------------- */
    /* TIMERA initialization: PWM frequency and Repetition rate               */
    /* Single-shot retriggerable mode, preload enabled on Master update event */
    /* Reset on Master period                                                 */
    /* ---------------------------------------------------------------------- */
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_A, REPETITON_RATE);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_MODE_RETRIGGERABLE);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_A);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_UPDATETRIG_MASTER);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_RESETTRIG_MASTER_PER);

    /* ----------------------------------------------------------------- */
    /* TA2 waveforms description: Set on master period, reset on CMP1    */
    /* Fault1 selected, disabling TA2 output, IDLE active for burst mode */
    /* ----------------------------------------------------------------- */
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TA2, SHRTIM_OUTPUTSET_MASTERPER);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TA2, SHRTIM_OUTPUTRESET_TIMCMP1);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TA2, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_A, SHRTIM_FAULT_1);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_A, DUTY_CYCLE_INIT);

    /* ----------------------------------- */
    /* TIMERB initialization: Idem Timer A */
    /* except Reset on Master Compare 1    */
    /* ----------------------------------- */
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_B, MULTIPHASE_PERIOD - 1 - ADC_CONVERSION_TIME);
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_B, REPETITON_RATE);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_MODE_RETRIGGERABLE);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_B);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_UPDATETRIG_MASTER);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_RESETTRIG_MASTER_CMP1);

    /* ----------------------------------------------------------------- */
    /* TB1 waveforms description: Set on master compare1, reset on CMP1  */
    /* Fault1 selected, disabling TB1 output, IDLE active for burst mode */
    /* ----------------------------------------------------------------- */
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TB1, SHRTIM_OUTPUTSET_MASTERCMP1);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TB1, SHRTIM_OUTPUTRESET_TIMCMP1);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TB1, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_B, SHRTIM_FAULT_1);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_B, DUTY_CYCLE_INIT);

    /* ----------------------------------- */
    /* TIMERC initialization: Idem Timer A */
    /* except Reset on Master Compare 2    */
    /* ----------------------------------- */
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_C, MULTIPHASE_PERIOD - 1 - ADC_CONVERSION_TIME);
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_C, REPETITON_RATE);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_MODE_RETRIGGERABLE);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_C);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_UPDATETRIG_MASTER);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_RESETTRIG_MASTER_CMP2);

    /* ----------------------------------------------------------------- */
    /* TC2 waveform description: Set on master compare2, reset on CMP1   */
    /* Fault1 selected, disabling TB1 output, IDLE active for burst mode */
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TC2, SHRTIM_OUTPUTSET_MASTERCMP2);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TC2, SHRTIM_OUTPUTRESET_TIMCMP1);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TC2, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_C, SHRTIM_FAULT_1);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_C, DUTY_CYCLE_INIT);


    /* ----------------------------------- */
    /* TIMERD initialization: Idem Timer A */
    /* except Reset on Master Compare 3    */
    /* ----------------------------------- */
    SHRTIM_TIM_SetPeriod(SHRTIM1, SHRTIM_TIMER_D, MULTIPHASE_PERIOD - 1 - ADC_CONVERSION_TIME);
    SHRTIM_TIM_SetRepetition(SHRTIM1, SHRTIM_TIMER_D, REPETITON_RATE);
    SHRTIM_TIM_SetCounterMode(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_MODE_RETRIGGERABLE);
    SHRTIM_TIM_EnablePreload(SHRTIM1, SHRTIM_TIMER_D);
    SHRTIM_TIM_SetUpdateTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_UPDATETRIG_MASTER);
    SHRTIM_TIM_SetResetTrig(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_RESETTRIG_MASTER_CMP3);

    /* ----------------------------------------------------------------------- */
    /* TD1 and TD2 waveform description: Set on master compare2, reset on CMP1 */
    /* Fault1 selected, disabling TB1 output, IDLE active for burst mode       */
    /* ----------------------------------------------------------------------- */
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTSET_MASTERCMP3);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUTPUTRESET_TIMCMP1);
    SHRTIM_OUT_SetOutputSetSrc(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUTPUTSET_MASTERCMP4);
    SHRTIM_OUT_SetOutputResetSrc(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUTPUTRESET_TIMCMP2);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TD2, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_OUT_Config(SHRTIM1, SHRTIM_OUTPUT_TD1, SHRTIM_OUT_FAULTSTATE_INACTIVE);
    SHRTIM_TIM_EnableFault(SHRTIM1, SHRTIM_TIMER_D, SHRTIM_FAULT_1);
    SHRTIM_TIM_SetCompare1(SHRTIM1, SHRTIM_TIMER_D, DUTY_CYCLE_INIT);
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_D, PHASE_SHIFT_INIT + DUTY_CYCLE_INIT);

    /* -------------------------------------------------- */
    /* Burst mode configuration for light load management */
    /* Trigger is master reset/roll-over event            */
    /* Preload enabled, continous mode                    */
    /* -------------------------------------------------- */ 
    SHRTIM_BM_EnablePreload(SHRTIM1);
    SHRTIM_BM_SetMode(SHRTIM1, SHRTIM_BM_MODE_CONTINOUS);
    SHRTIM_BM_SetTrig(SHRTIM1, SHRTIM_BM_TRIG_MASTER_RESET);
    SHRTIM_BM_SetPeriod(SHRTIM1, BURST_PERIOD);
    SHRTIM_BM_SetCompare(SHRTIM1, BURST_IDLE);

    /* ------------------------------------------- */
    /* ADC trigger intialization (with CMP2 event) */
    /* ------------------------------------------- */
    /* Default sampling points for 5-phase configuration */
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_A, DUTY_CYCLE_INIT/2); /* Samples at 50% of Ton time */
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_B, DUTY_CYCLE_INIT/2); /* Samples at 50% of Ton time */
    SHRTIM_TIM_SetCompare2(SHRTIM1, SHRTIM_TIMER_C, DUTY_CYCLE_INIT/2); /* Samples at 50% of Ton time */
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_D, DUTY_CYCLE_INIT/2); /* Samples at 50% of Ton time */
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_D, PHASE_SHIFT_INIT + (DUTY_CYCLE_INIT/2)); /* Samples at 50% of Ton time on phase 5 */

    /* Dummy triggers for 3-phase configuration, using Timer C compare 3 and 4 */
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_C, (MULTIPHASE_PERIOD/3) - 1  - ADC_CONVERSION_TIME);
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_C, (MULTIPHASE_PERIOD/3) - 1 - (ADC_CONVERSION_TIME*2));

    /* Dummy triggers for 2-phase configuration */
    /* Configure Timer B compare 3 and 4 and timer A compare 3 for a dummy 3rd, 4th and 5th conversion */
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_B, (MULTIPHASE_PERIOD/2) - 1 - ADC_CONVERSION_TIME);
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_B, (MULTIPHASE_PERIOD/2) - 1 - (ADC_CONVERSION_TIME*2));
    SHRTIM_TIM_SetCompare3(SHRTIM1, SHRTIM_TIMER_A, MULTIPHASE_PERIOD - 1 - (ADC_CONVERSION_TIME*3));
    
    /* Dummy triggers for 1-phase configuration */
    /* Configure Timer A compare 3 and 4 for dummy 2nd and 3rd conversion */
    /* Dummy 4th conversion triggered by Timer A CMP5 event */
    SHRTIM_TIM_SetCompare5(SHRTIM1, SHRTIM_TIMER_A, MULTIPHASE_PERIOD - 1 - (ADC_CONVERSION_TIME));
    SHRTIM_TIM_SetCompare4(SHRTIM1, SHRTIM_TIMER_A, MULTIPHASE_PERIOD - 1 - (ADC_CONVERSION_TIME*2));

    /* ADC trigger 1 update source: Master */
    /* ADC trigger event: Timer A compare 2, Timer B compare 2, Timer C compare 2, Timer D compare 3, Timer D compare 4, Timer D compare 3, Timer D compare 4 */
    SHRTIM_ConfigADCTrig(SHRTIM1, SHRTIM_ADCTRIG1_SOURCE_GROUP1, SHRTIM_ADCTRIG_1, SHRTIM_ADCTRIG_UPDATE_MASTER, \
                                  SHRTIM_ADTG13_SOURCE_GROUP1_TACMP2 | SHRTIM_ADTG13_SOURCE_GROUP1_TBCMP2 | SHRTIM_ADTG13_SOURCE_GROUP1_TCCMP2);  
    SHRTIM_ConfigADCTrig(SHRTIM1, SHRTIM_ADCTRIG1_SOURCE_GROUP2, SHRTIM_ADCTRIG_1, SHRTIM_ADCTRIG_UPDATE_MASTER, \
                                  SHRTIM_ADTG13_SOURCE_GROUP2_TDCMP3 | SHRTIM_ADTG13_SOURCE_GROUP2_TDCMP4);
                                            
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
    SHRTIM_ForceUpdate(SHRTIM1, SHRTIM_TIMER_A | SHRTIM_TIMER_B | SHRTIM_TIMER_C | SHRTIM_TIMER_D);
}

void ADC_Config(void)
{
    ADC_InitType ADC_InitStructure;
    ADC_InitStruct(&ADC_InitStructure);
    ADC_MspInit();
    /* ADC configuration ------------------------------------------------------*/
    ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;
    ADC_InitStructure.MultiChEn      = ENABLE;
    ADC_InitStructure.ContinueConvEn = DISABLE;
    ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_SHRTIM_TRGO1;
    ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;
    ADC_InitStructure.ChsNumber      = ADCCONVERTEDVALUES_BUFFER_SIZE;
    ADC_InitStructure.Resolution     = ADC_DATA_RES_12BIT;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Enable discontinuous mode */
    ADC1->CTRL1 |= ADC_DISC_REG_EN_MASK;

    /* Disable scan mode */
    ADC1->CTRL1 &= (~ADC_SCANMD_EN_MASK);
    
    /* ADC regular channel1 configuration */
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_03_PA0, 1, ADC_SAMP_TIME_CYCLES_1_5); 
    /* ADC regular channel2 configuration */
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_03_PA0, 2, ADC_SAMP_TIME_CYCLES_1_5); 
    /* ADC regular channel3 configuration */
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_03_PA0, 3, ADC_SAMP_TIME_CYCLES_1_5); 
    /* ADC regular channel4 configuration */
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_03_PA0, 4, ADC_SAMP_TIME_CYCLES_1_5); 
    /* ADC regular channel5 configuration */
    ADC_ConfigRegularChannel(ADC1, ADC1_Channel_03_PA0, 5, ADC_SAMP_TIME_CYCLES_1_5); 

    /* Enable ADC DMA */
    ADC_SetDMATransferMode(ADC1, ADC_MULTI_REG_DMA_EACH_ADC);
    
    DMA1_CH1->TXNUM = ADCCONVERTEDVALUES_BUFFER_SIZE;
    DMA1_CH1->PADDR = (uint32_t)&ADC1->DAT;
    DMA1_CH1->MADDR = (uint32_t)&aADCxConvertedValues;
    
    /* Enable DMA channel1 */
    DMA_EnableChannel(DMA1_CH1, ENABLE);

    /* Enable ADC */
    ADC_Enable(ADC1, ENABLE);
    
    /*Check ADC Ready*/
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_RDY) == RESET)
        ;
    /* Start ADC1 calibration */
    ADC_CalibrationOperation(ADC1,ADC_CALIBRATION_SINGLE_MODE);
    /* Check the end of ADC1 calibration */
    while (ADC_GetCalibrationStatus(ADC1, ADC_CALIBRATION_SINGLE_MODE))
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

