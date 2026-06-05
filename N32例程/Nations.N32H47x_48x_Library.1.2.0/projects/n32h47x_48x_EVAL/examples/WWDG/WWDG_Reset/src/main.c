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
 * @file main.c
 * @author Nations
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
 */
#include "main.h"
#include "n32h47x_48x_dbg.h"
#include "delay.h"


/** @addtogroup n32h47x_48x_StdPeriph_Examples
 * @{
 */

/** @addtogroup WWDG_Reset
 * @{
 */

#define LED1 GPIO_PIN_3
#define LED2 GPIO_PIN_8

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
         file (startup_n32h47x_48x.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_n32h47x_48x.c file
       */
    log_init();
    log_info("--- WWDG demo reset ---\n");
    WWDG_ClrEWINTF();
    LedInit(GPIOA, LED1 | LED2);
    LedOff(GPIOA, LED1 | LED2);
    /* Enable PWR Clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    
    /* Check if the system has resumed from WWDG reset */
    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRSTF) != RESET)
    {
        /* WWDGRST flag set */
        /* Turn on LED1 */
        LedOn(GPIOA, LED1);
        log_info("reset by WWDG\n");
        /* Clear reset flags */
        RCC_ClearResetFlag();
    }
    else
    {
        /* WWDGRST flag is not set */
        /* Turn off LED1 */
        LedOff(GPIOA, LED1);
    }

    /*
      When the window value is very small, the system is in a frequent reset state,
      at this time, easy to cause the program can not download normally.
      Add a delay of 1 second here to avoid this phenomenon. Of course,
      it can also be downloaded without delay, directly pull the pin of BOOT0 high.
    */
    systick_delay_ms(1000);
    
    /* WWDG configuration */
    /* Enable WWDG clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_WWDG, ENABLE);

    /* WWDG clock counter = 1/((PCLK1(120MHz)/4096)/8) = (~273 us)  */
    /* WWDG clock counter = 1/((PCLK1(100MHz)/4096)/8) = (~327 us)  */
    WWDG_SetPrescalerDiv(WWDG_PRESCALER_DIV8);

    /* Set Window value to 80; WWDG counter should be refreshed only when the counter
      is below 80 (and greater than 64) otherwise a reset will be generated */
    WWDG_SetWValue(80);

    /*
      When PCLK1 = 120MHz, in this case the refresh window is: ~273 us * (127-80) = 12.831 ms < refresh window < ~273 us * (127-63) = 17.472ms
      When PCLK1 = 100MHz, in this case the refresh window is: ~327 us * (127-80) = 15.401 ms < refresh window < ~327 us * (127-63) = 20.971ms
    */
    WWDG_Enable(127);

    while (1)
    {
        /* Toggle LED2 */
        LedBlink(GPIOA, LED2);
        systick_delay_ms(16);
        /* Update WWDG counter */
        WWDG_SetCnt(127);
    }
}


