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
*\*\file magic_wakeup.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "magic_wakeup.h"
#include "n32h47x_48x_eth.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_flash.h"

#include "lwip/sys.h"
#include "lwip/api.h"

#include "FreeRTOS.h"
#include "task.h"


static void magic_wakeup_thread(void *arg);
static void key_gpio_init(GPIO_Module* GPIOx, uint16_t Pin);
static void eth_enter_lowpower(void);
static void set_sys_clock_to_pll(uint32_t PLL_src, uint32_t PLL_freq);

/**
*\*\name    set_sys_clock_to_pll.
*\*\fun     Selects PLL clock as System clock source and configure HCLK, PCLK2
*\*\        and PCLK1 prescalers.
*\*\param   PLL_src
*\*\         - RCC_PLL_SRC_HSI
*\*\         - RCC_PLL_SRC_HSE
**\*\param  PLL_freq
*\*\         - 160000000
*\*\         - 240000000
*\*\return  none
*\*\note    none
**/
static void set_sys_clock_to_pll(uint32_t PLL_src, uint32_t PLL_freq)
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
        /* clock source fails to start-up */
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
            pllmul   = RCC_PLL_MUL_30;
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
    }
    /* Select PLL as system clock source */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLL);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL);
}

/**
*\*\name    eth_enter_lowpower.
*\*\fun     Configure the ETH module to enter low-power mode.
*\*\param   none
*\*\return  none
**/
static void eth_enter_lowpower(void)
{
    /* Enable PMT interrupt */
    ETH_ConfigMACInterrupt(ETH, ETH_MAC_INT_PMT, ENABLE);
    /* Disable transmit DMA */
    ETH_TxDMACmd(ETH, DISABLE);
    
    /* Wait for any previous frame transmissions to complete */
    while (((ETH->MTLTXQDBG & ETH_TRCSTS_MASK) == ETH_MTLTXQSTS_READ)
        && ((ETH->MTLTXQDBG & ETH_MTLTXQSTS_NOT_EMPTY) == ETH_MTLTXQSTS_NOT_EMPTY))
    {
    }
    
    /* Disable transmit MAC */
    ETH_TxMACCmd(ETH, DISABLE);
    /* Disable receive MAC */
    ETH_RxMACCmd(ETH, DISABLE);
    
    /* Wait for all data to be transferred to system memory */
    while (((ETH->MTLRXQDBG & ETH_RXQSTS_MASK) != ETH_MTLRXQSTS_EMPTY)
        && ((ETH->MTLRXQDBG & ETH_MTLRXQSTS_PACK_NBR) != (uint32_t)RESET))
    {
    }
    
    /* Disable receive DMA */
    ETH_RxDMACmd(ETH, DISABLE);
    /* Enable Magic Packet Detection */
    ETH_MagicPacketDetectionCmd(ETH, ENABLE);
    
    printf("Enter ETH Low-power mode\n");
    /* Enter ETH power-down mode */
    ETH_PowerDownCmd(ETH, ENABLE);
    /* Enable receive MAC */
    ETH_RxMACCmd(ETH, ENABLE);
}

/**
*\*\name    key_gpio_init.
*\*\fun     Configures key GPIO.
*\*\param   GPIOx
*\*\param   Pin
*\*\return  none
**/
static void key_gpio_init(GPIO_Module* GPIOx, uint16_t Pin)
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
    else
    {
        /* Do nothing */
    }
    /* Configure the GPIO pin */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin        = Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
}


/**
*\*\name    magic_wakeup_thread.
*\*\fun     Magic packet wakeup task entry function.
*\*\param   arg
*\*\         - Task entry function parameter
*\*\return  none
**/
static void magic_wakeup_thread(void *arg)
{
    LWIP_UNUSED_ARG(arg);
    
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    /* Initialize key */
    key_gpio_init(KEY_GPIO, KEY_PIN);
    
    while (1)
    {
        if (GPIO_ReadInputDataBit(KEY_GPIO, KEY_PIN) == SET)
        {
            /* Enter low-power mode and configure the detection magic packet */
            eth_enter_lowpower();
            
            printf("Enter STOP mode\n");
            /* Before entering STOP, make sure that the USART transmission has been completed */
            while (USART_GetFlagStatus(USART1, USART_FLAG_TXC) == RESET)
            {
            }
            /* Request to enter STOP mode */
            PWR_EnterSTOPMode(PWR_REGULATOR_NORMAL, PWR_STOPENTRY_WFI);
            /* Configures system clock after wake-up from STOP: enable HSI, PLL and select
            PLL as system clock source */
            set_sys_clock_to_pll(RCC_PLL_SRC_HSI, 240000000);

            printf("Exit STOP mode\n");
            
            /* Enable transmit MAC */
            ETH_TxMACCmd(ETH, ENABLE);
            /* Enable receive DMA */
            ETH_RxDMACmd(ETH, ENABLE);
            /* Enable transmit DMA */
            ETH_TxDMACmd(ETH, ENABLE);
            /* Disable Magic Packet Detection */
            ETH_MagicPacketDetectionCmd(ETH, DISABLE);
        }
        
        /* System can switch to other tasks */
        vTaskDelay(5);
    }
}

/**
*\*\name    magic_wakeup_init.
*\*\fun     Initialize magic packet wakeup function by creating a task thread.
*\*\param   none
*\*\return  none
**/
void magic_wakeup_init(void)
{
    printf("\r\nCall %s\r\n", __FUNCTION__);
    
    /* Create a magic packet wakeup task thread */
    sys_thread_new("magic_wakeup_thread",        /* Task name */
                   magic_wakeup_thread,          /* Task entry function */
                   NULL,                         /* Task entry function parameter */
                   MAGIC_WAKEUP_TASK_STACK_SIZE, /* Task stack size */
                   MAGIC_WAKEUP_TASK_PRIORITY);  /* Task priority */
}

