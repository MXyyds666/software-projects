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
*\*\file hw_config.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "hw_config.h"
#include "usbfsd_lib.h"
#include "usbfsd_desc.h"
#include "usbfsd_pwr.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"
#include "misc.h"
#include "delay.h"

/**
*\*\name    KeyInputExtiInit.
*\*\fun     Configures key port.
*\*\param   GPIOx x can be A to G to select the GPIO port.
*\*\param   Pin This parameter can be GPIO_PIN_0~GPIO_PIN_15.
*\*\return  none 
**/
void KeyInputExtiInit(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_InitType GPIO_InitStructure;

    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOD)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOE)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOF)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOG)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG | RCC_APB2_PERIPH_AFIO, ENABLE);
    }
    else if (GPIOx == GPIOH)
    {
        RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH | RCC_APB2_PERIPH_AFIO, ENABLE);
    }


    /*Configure the GPIO pin as input floating*/
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin        = Pin;
        GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
        GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }
}

/**
*\*\name    Cfg_KeyIO.
*\*\fun     Configures the gpio for keyboard.
*\*\param   none
*\*\return  none 
**/
void Cfg_KeyIO(void)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA | RCC_AHB_PERIPHEN_GPIOB | RCC_AHB_PERIPHEN_GPIOC, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);

    GPIO_InitStructure.Pin = KEY_A_PIN;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(KEY_A_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin = KEY_B_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(KEY_B_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin = KEY_C_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(KEY_C_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED_CAPSLOCK_PIN ;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(LED_CAPSLOCK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LED_NUMLOCK_PIN ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(LED_NUMLOCK_PORT, &GPIO_InitStructure);
    
    /*Configure key EXTI Line to key input Pin*/
    GPIO_ConfigEXTILine(KEY_INPUT_EXTI_SOURCE, KEY_INPUT_PORT_SOURCE, KEY_INPUT_PIN_SOURCE);
    
    KeyInputExtiInit(KEY_INPUT_PORT, KEY_INPUT_PIN);
    
    /*Configure key EXTI line*/
    EXTI_InitStructure.EXTI_Line    = KEY_INPUT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /*Set key input interrupt priority*/
    NVIC_InitStructure.NVIC_IRQChannel                   = KEY_INPUT_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    USBFS_IO_Configure.
*\*\fun     Configures usb fs device gpio.
*\*\param   none
*\*\return  none 
**/
void USBFS_IO_Configure(void)
{
    GPIO_InitType GPIO_InitStructure;  
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    
    GPIO_InitStruct(&GPIO_InitStructure);
  
    GPIO_InitStructure.Pin              = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate   = GPIO_AF10;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin              = GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate   = GPIO_AF10;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

/**
*\*\name    Set_USBClock.
*\*\fun     Configures USB Clock input (48MHz).
*\*\param   sysclk: system clock.
*\*\          - SYSCLK_VALUE_48MHz
*\*\          - SYSCLK_VALUE_72MHz
*\*\          - SYSCLK_VALUE_96MHz
*\*\          - SYSCLK_VALUE_144MHz
*\*\          - SYSCLK_VALUE_192MHz
*\*\          - SYSCLK_VALUE_240MHz
*\*\return  ErrorStatus 
*\*\          - SUCCESS
*\*\          - ERROR
**/
ErrorStatus Set_USBClock(uint32_t sysclk)
{
    ErrorStatus status = SUCCESS;

    switch(sysclk)
    {
    case SYSCLK_VALUE_48MHz: 
        RCC_ConfigUSBPLLPresClk(RCC_USBPLLCLK_SRC_PLL, RCC_USBPLLCLK_DIV1);
        RCC->CFG3 &= ~RCC_CFG3_USBFSTM;
        break;

    case SYSCLK_VALUE_96MHz: 
        RCC_ConfigUSBPLLPresClk(RCC_USBPLLCLK_SRC_PLL, RCC_USBPLLCLK_DIV2);
        RCC->CFG3 &= ~RCC_CFG3_USBFSTM;
        break;
    
    case SYSCLK_VALUE_144MHz: 
        RCC_ConfigUSBPLLPresClk(RCC_USBPLLCLK_SRC_PLL, RCC_USBPLLCLK_DIV3);
        RCC->CFG3 |= RCC_CFG3_USBFSTM;
        break;

    case SYSCLK_VALUE_192MHz: 
        RCC_ConfigUSBPLLPresClk(RCC_USBPLLCLK_SRC_PLL, RCC_USBPLLCLK_DIV4);
        RCC->CFG3 |= RCC_CFG3_USBFSTM;
        break;
    
    case SYSCLK_VALUE_240MHz: 
        RCC_ConfigUSBPLLPresClk(RCC_USBPLLCLK_SRC_PLL, RCC_USBPLLCLK_DIV5);
        RCC->CFG3 |= RCC_CFG3_USBFSTM;
        break;

    default:
        status = ERROR;
        break;
    }

    return status;
}

/**
*\*\name    USB_ActiveRemoteWakeup.
*\*\fun     Active remote wakeup host.
*\*\param   none
*\*\return  none 
**/
void USB_ActiveRemoteWakeup(void)
{
    uint16_t wCNTR;
    
    /* CTRL_LP_MODE = 0 */
    wCNTR = _GetCNTR();
    wCNTR &= (~CTRL_LP_MODE);
    _SetCNTR(wCNTR);

    /* reset FSUSP bit */ 
    _SetCNTR(IMR_MSK);
    
    /*Pull up DP*/
    _EnPortPullup();
    
    /* active Remote wakeup signaling */
    wCNTR = _GetCNTR();
    wCNTR |= CTRL_RESUM;
    _SetCNTR(wCNTR);
    
    systick_delay_ms(10);

    wCNTR = _GetCNTR();
    wCNTR &= (~CTRL_RESUM);
    _SetCNTR(wCNTR);
}

/**
*\*\name    Enter_LowPowerMode.
*\*\fun     Power-off system clocks and power while entering suspend mode.
*\*\param   none
*\*\return  none 
**/
void Enter_LowPowerMode(void)
{
    /* Set the device state to suspend */
    bDeviceState = SUSPENDED;
}

/**
*\*\name    Leave_LowPowerMode.
*\*\fun     Restores system clocks and power while exiting suspend mode.
*\*\param   none
*\*\return  none 
**/
void Leave_LowPowerMode(void)
{
    USB_DeviceMess* pInfo = &Device_Info;

    /* Set the device state to the correct state */
    if (pInfo->CurrentConfiguration != 0)
    {
        /* Device configured */
        bDeviceState = CONFIGURED;
    }
    else
    {
        bDeviceState = ATTACHED;
    }
}

/**
*\*\name    USB_Interrupts_Config.
*\*\fun     Configures the USB interrupts.
*\*\param   none
*\*\return  none 
**/
void USB_Interrupts_Config(void)
{
    NVIC_InitType NVIC_InitStructure;
    EXTI_InitType EXTI_InitStructure;

    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* Enable the USB interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_FS_LP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USB Wake-up interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_FS_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure the EXTI line 18 connected internally to the USB IP */
    EXTI_ClrITPendBit(EXTI_LINE18);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE18; 
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);
}

/**
*\*\name    USB_Config.
*\*\fun     Configures USB Clock input (48MHz).
*\*\param   sysclk: system clock.
*\*\          - SYSCLK_VALUE_48MHz
*\*\          - SYSCLK_VALUE_72MHz
*\*\          - SYSCLK_VALUE_96MHz
*\*\          - SYSCLK_VALUE_144MHz
*\*\          - SYSCLK_VALUE_192MHz
*\*\          - SYSCLK_VALUE_240MHz
*\*\return  ErrorStatus 
*\*\          - SUCCESS
*\*\          - ERROR
**/
ErrorStatus USB_Config(uint32_t sysclk)
{
    ErrorStatus status = SUCCESS;
    
    RCC_ConfigUSBFSClk(RCC_USBFS_CLKSRC_PLLPRES);

    if(Set_USBClock(sysclk) == SUCCESS)
    {
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USBFS, ENABLE);
        status = SUCCESS;
    }
    else
    {
        status = ERROR;
    }

    return status;
}
