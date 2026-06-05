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
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_pwr.h"
#include "delay.h"
#include "n32h47x_48x_dbg.h"

/** Define the GPIO port to which the LED is connected **/
#define LED1_GPIO_PORT      GPIOA                       /* GPIO port */
#define LED1_GPIO_CLK       RCC_AHB_PERIPH_GPIOA        /* GPIO port clock */
#define LED1_GPIO_PIN       GPIO_PIN_3                  /* GPIO pin */

#define LED2_GPIO_PORT      GPIOA                       /* GPIO port */
#define LED2_GPIO_CLK       RCC_AHB_PERIPH_GPIOA        /* GPIO port clock */
#define LED2_GPIO_PIN       GPIO_PIN_8                  /* GPIO pin */


__IO uint32_t TimingDelay = 0;
__IO uint32_t LsiFreq = 32000;
extern uint16_t CaptureNumber;

void GTIM1_ConfigForLSI(void);

/**
*\*\name    LedInit.
*\*\fun     Configures LED GPIO.
*\*\param   GPIOx :
*\*\          - GPIOA
*\*\          - GPIOB
*\*\          - GPIOC
*\*\          - GPIOD
*\*\          - GPIOE
*\*\          - GPIOF
*\*\          - GPIOG
*\*\          - GPIOH
*\*\param   Pin :
*\*\          - GPIO_PIN_0
*\*\          - GPIO_PIN_1
*\*\          - GPIO_PIN_2
*\*\          - GPIO_PIN_3
*\*\          - GPIO_PIN_4
*\*\          - GPIO_PIN_5
*\*\          - GPIO_PIN_6
*\*\          - GPIO_PIN_7
*\*\          - GPIO_PIN_8
*\*\          - GPIO_PIN_9
*\*\          - GPIO_PIN_10
*\*\          - GPIO_PIN_11
*\*\          - GPIO_PIN_12
*\*\          - GPIO_PIN_13
*\*\          - GPIO_PIN_14
*\*\          - GPIO_PIN_15
*\*\          - GPIO_PIN_ALL
*\*\return  none
**/
void LedInit(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    }
    else if (GPIOx == GPIOD)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    }
    else if (GPIOx == GPIOE)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE);
    }
    else if (GPIOx == GPIOF)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF, ENABLE);
    }
    else if (GPIOx == GPIOG)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG, ENABLE);
    }
    else if (GPIOx == GPIOH)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);
    }

    /* Configure the GPIO pin */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin        = Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
}

/**
*\*\name    LedOn.
*\*\fun     Turns selected Led on
*\*\param   GPIOx :
*\*\          - GPIOA
*\*\          - GPIOB
*\*\          - GPIOC
*\*\          - GPIOD
*\*\          - GPIOE
*\*\          - GPIOF
*\*\          - GPIOG
*\*\          - GPIOH
*\*\param   Pin :
*\*\          - GPIO_PIN_0
*\*\          - GPIO_PIN_1
*\*\          - GPIO_PIN_2
*\*\          - GPIO_PIN_3
*\*\          - GPIO_PIN_4
*\*\          - GPIO_PIN_5
*\*\          - GPIO_PIN_6
*\*\          - GPIO_PIN_7
*\*\          - GPIO_PIN_8
*\*\          - GPIO_PIN_9
*\*\          - GPIO_PIN_10
*\*\          - GPIO_PIN_11
*\*\          - GPIO_PIN_12
*\*\          - GPIO_PIN_13
*\*\          - GPIO_PIN_14
*\*\          - GPIO_PIN_15
*\*\          - GPIO_PIN_ALL
*\*\return  none
**/
void LedOn(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIOx->PBSC = Pin;
}

/**
*\*\name    LedOff.
*\*\fun     Turns selected Led off.
*\*\param   GPIOx :
*\*\          - GPIOA
*\*\          - GPIOB
*\*\          - GPIOC
*\*\          - GPIOD
*\*\          - GPIOE
*\*\          - GPIOF
*\*\          - GPIOG
*\*\          - GPIOH
*\*\param   Pin :
*\*\          - GPIO_PIN_0
*\*\          - GPIO_PIN_1
*\*\          - GPIO_PIN_2
*\*\          - GPIO_PIN_3
*\*\          - GPIO_PIN_4
*\*\          - GPIO_PIN_5
*\*\          - GPIO_PIN_6
*\*\          - GPIO_PIN_7
*\*\          - GPIO_PIN_8
*\*\          - GPIO_PIN_9
*\*\          - GPIO_PIN_10
*\*\          - GPIO_PIN_11
*\*\          - GPIO_PIN_12
*\*\          - GPIO_PIN_13
*\*\          - GPIO_PIN_14
*\*\          - GPIO_PIN_15
*\*\          - GPIO_PIN_ALL
*\*\return  none
**/
void LedOff(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIOx->PBC = Pin;
}

/**
*\*\name    LedBlink.
*\*\fun     Toggles the selected Led.
*\*\param   GPIOx :
*\*\          - GPIOA
*\*\          - GPIOB
*\*\          - GPIOC
*\*\          - GPIOD
*\*\          - GPIOE
*\*\          - GPIOF
*\*\          - GPIOG
*\*\          - GPIOH
*\*\param   Pin :
*\*\          - GPIO_PIN_0
*\*\          - GPIO_PIN_1
*\*\          - GPIO_PIN_2
*\*\          - GPIO_PIN_3
*\*\          - GPIO_PIN_4
*\*\          - GPIO_PIN_5
*\*\          - GPIO_PIN_6
*\*\          - GPIO_PIN_7
*\*\          - GPIO_PIN_8
*\*\          - GPIO_PIN_9
*\*\          - GPIO_PIN_10
*\*\          - GPIO_PIN_11
*\*\          - GPIO_PIN_12
*\*\          - GPIO_PIN_13
*\*\          - GPIO_PIN_14
*\*\          - GPIO_PIN_15
*\*\          - GPIO_PIN_ALL
*\*\return  none
**/
void LedBlink(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIOx->POD ^= Pin;
}

/**
*\*\name main.
*\*\fun Main program.
*\*\return none
**/
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_n32h4xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_n32h47x_48x.c file
       */
    log_init();
    log_info("--- IWDG demo reset ---\n");
    LedInit(LED1_GPIO_PORT, LED1_GPIO_PIN);
    LedInit(LED2_GPIO_PORT, LED2_GPIO_PIN);
    LedOff(LED1_GPIO_PORT, LED1_GPIO_PIN);
    LedOff(LED2_GPIO_PORT, LED2_GPIO_PIN);
    
    /* Enable PWR Clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    
    /* Check if the system has resumed from IWDG reset */
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRSTF) != RESET)
    {
        /* IWDGRST flag set */
        /* Turn on LED1 */
        LedOn(LED1_GPIO_PORT, LED1_GPIO_PIN);
        log_info("reset by IWDG\n");
        /* Clear reset flags */
        RCC_ClearResetFlag();
    }
    else
    {
        /* IWDGRST flag is not set */
        /* Turn off LED1 */
        LedOff(LED1_GPIO_PORT, LED1_GPIO_PIN);
    }
    
#ifdef LSI_TIM_MEASURE
    /* Enable the LSI OSC */
    RCC_EnableLsi(ENABLE);

    /* Wait till LSI is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDF) == RESET)
    {
    }
    /* TIM Configuration -------------------------------------------------------*/
    GTIM1_ConfigForLSI();
    /* Wait until the GTIM1 get 3 LSI edges */
    while (CaptureNumber != 3)
    {
    }
    /* Disable GTIM1 CC3 Interrupt Request */
    TIM_ConfigInt(GTIM1, TIM_INT_CC3, DISABLE);
#endif  /* LSI_TIM_MEASURE */

    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
       dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteConfig(IWDG_WRITE_ENABLE);

    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV32);

    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/128)
                            = 0.25s / (LsiFreq/32)
                            = LsiFreq/(32 * 4)
                            = LsiFreq/128 */
    log_debug("LsiFreq is: %d\n", LsiFreq);

    IWDG_CntReload(LsiFreq / 128);
    /* Reload IWDG counter */
    IWDG_ReloadKey();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

    while(1)
    {
        /* Toggle LED2 */
        LedBlink(LED2_GPIO_PORT, LED2_GPIO_PIN);
        /* Insert 249 ms delay */
        systick_delay_ms(249);
        /* Reload IWDG counter */
        IWDG_ReloadKey();
    }
}

#ifdef LSI_TIM_MEASURE
/**
*\*\name   GTIM1_ConfigForLSI.
*\*\fun    Configures GTIM1 to measure the LSI oscillator frequency.
*\*\param  none.
*\*\return none.
**/
void GTIM1_ConfigForLSI(void)
{
    NVIC_InitType NVIC_InitStructure;
    TIM_ICInitType TIM_ICInitStructure;

    /* Enable GTIM1 clocks */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM1 | RCC_APB2_PERIPH_AFIO, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* Enable the GTIM1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                      = GTIM1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure GTIM1 prescaler */
    TIM_ConfigPrescaler(GTIM1, 1, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    
    /* GTIM1 configuration: Input Capture mode 
    The LSI oscillator is connected to GTIM1 CH3
    The Rising edge is used as active edge,
    The GTIM1 CCDAT3 is used to compute the frequency value */
    TIM_ICInitStructure.Channel     = TIM_CH_3;
    TIM_ICInitStructure.ICPolarity  = TIM_IC_POLARITY_RISING;
    TIM_ICInitStructure.ICSelection = TIM_IC_SELECTION_DIRECTTI;
    TIM_ICInitStructure.ICPrescaler = TIM_IC_PSC_DIV8;
    TIM_ICInitStructure.ICFilter    = 0x0;
    TIM_ICInit(GTIM1, &TIM_ICInitStructure);
                                       
    GTIM1->CTRL1 |= TIM_CTRL1_C3SEL;
    /* GTIM1 Counter Enable */
    TIM_Enable(GTIM1, ENABLE);

    /* Reset the flags */
    GTIM1->STS = 0;

    /* Enable the CC3 Interrupt Request */
    TIM_ConfigInt(GTIM1, TIM_INT_CC3, ENABLE);
}
#endif  /* LSI_TIM_MEASURE */


