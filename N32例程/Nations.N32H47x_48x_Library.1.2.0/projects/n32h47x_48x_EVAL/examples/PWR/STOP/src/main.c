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
#include "log.h"
#include <stdio.h>
#include <stdint.h>

void delay(vu32 nCount);
void KeyInputExtiInit(GPIO_Module* GPIOx, uint16_t Pin);
void SetSysClockToPLL(uint32_t PLL_src, uint32_t PLL_freq);

/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none
*\*\return  none
**/
int main(void)
{
    /* At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_n32h4xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_n32h47x_48x.c file
       */
    log_init(); 
    log_info("\r\n --- Reset --- \r\n");
    
    /* Enable PWR clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR,ENABLE);
    /* Initialize Key button Interrupt to wakeUp stop */
    KeyInputExtiInit(KEY_INPUT_PORT, KEY_INPUT_PIN);
    
    while (1)
    {
       /* Insert a long delay */
       delay(700);
       log_info("Entry STOP mode\n"); 
       delay(30);
       /* Request to enter STOP mode*/
       PWR_EnterSTOPMode(PWR_REGULATOR_NORMAL, PWR_STOPENTRY_WFI);
       /* Configures system clock after wake-up from STOP: enable HSI, PLL and select
          PLL as system clock source */
       SetSysClockToPLL(RCC_PLL_SRC_HSI,160000000);
       log_init();
       log_info("\nExit STOP mode\n");
    }
}

/**
*\*\name    delay.
*\*\fun     Delay function.
*\*\param   nCount
*\*\return  none
**/
void delay(vu32 nCount)
{
    vu32 index = 0;
    for (index = (34000 * nCount); index != 0; index--)
    {
    }
}

/**
*\*\name    KeyInputExtiInit.
*\*\fun     Configures key GPIO.
*\*\param   GPIOx
*\*\param   Pin
*\*\return  none
**/
void KeyInputExtiInit(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;
 
    /* Enable the GPIO Clock */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);

    /*Configure the GPIO pin as input floating*/
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin        = Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);

    /*Configure key EXTI Line to key input  Pin*/
    GPIO_ConfigEXTILine(KEY_EXTI_LINE_SOURCE, GPIOA_PORT_SOURCE, KEY_GPIO_PIN_SOURCE);

    /*Configure key EXTI line*/
    EXTI_InitStructure.EXTI_Line = KEY_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /*Set key input interrupt priority*/
    NVIC_InitStructure.NVIC_IRQChannel                   = KEY_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    SetSysClockToPLL.
*\*\fun     Selects PLL clock as System clock source and configure HCLK, PCLK2
*\*\         and PCLK1 prescalers.  
*\*\param   PLL_src
*\*\         - RCC_PLL_SRC_HSI      
*\*\         - RCC_PLL_SRC_HSE  
**\*\param   PLL_freq
*\*\         - 160000000      
*\*\         - 240000000  
*\*\return  none 
*\*\note    none
**/
void SetSysClockToPLL(uint32_t PLL_src, uint32_t PLL_freq)
{
    ErrorStatus ClockStatus;
    uint32_t pllmul;
    uint32_t latency;
    uint32_t pclk1div, pclk2div;
    uint32_t pllpre;

    if ((PLL_src == RCC_PLL_SRC_HSE)&&((HSE_VALUE != 8000000)&&(HSE_VALUE != 16000000)))
    {
        /* HSE_VALUE == 8000000 or 16000000 is needed in this project! */
        while (1);
    }
    if ((PLL_src == RCC_PLL_SRC_HSE)&&(HSE_VALUE == 16000000))
    {
        pllpre = RCC_PLL_PRE_2;
    }
    else
    {
        pllpre = RCC_PLL_PRE_1;
    }

    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    if(PLL_src == RCC_PLL_SRC_HSE)
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
        /* clock source fails to start-up, user can add here some code to deal with this error */
        while (1)
        {
        }
    }

    switch (PLL_freq)
    {
        case 160000000:
            latency  = FLASH_LATENCY_3;
            pllmul   = RCC_PLL_MUL_20;
            pclk1div = RCC_HCLK_DIV1;
            pclk2div = RCC_HCLK_DIV2;
            break;
        case 240000000:
            latency  = FLASH_LATENCY_5;
		    pllmul   =  RCC_PLL_MUL_30;
            pclk1div = RCC_HCLK_DIV2;
            pclk2div = RCC_HCLK_DIV2;
            break;
        default:
            while (1);
    }

    FLASH_SetLatency(latency);

    /* HCLK = SYSCLK */
    RCC_ConfigHclk(RCC_SYSCLK_DIV1);

    /* PCLK2 */
    RCC_ConfigPclk2(pclk2div);

    /* PCLK1 */
    RCC_ConfigPclk1(pclk1div);
    
    if(PLL_src == RCC_PLL_SRC_HSE)
    {
        RCC_ConfigPll(RCC_PLL_SRC_HSE,pllpre,pllmul, RCC_PLLOUT_DIV_1);
    }
    else
    {
        RCC_ConfigPll(RCC_PLL_SRC_HSI,pllpre,pllmul, RCC_PLLOUT_DIV_1);
    }

    /* Enable PLL */
    RCC_EnablePll(ENABLE);

     /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDF) != SET)
    {
        /* if this bit always not set, user can add here some code to deal with this error */
    }

    /* Select PLL as system clock source */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLL);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL);
    /* if this bit always not set, user can add here some code to deal with this error */
}

