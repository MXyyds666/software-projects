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
#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_iwdg.h"
#include "n32h47x_48x_flash.h"

/* Internal SRAM config fo MCU */
#define SRAM_BASE_ADDR          (0x20000000)
#define SRAM_SIZE               (0x24000)

/* Constant for BOOT */
uint32_t BootAddr = 0x1FFF01a1,SPAddr = 0x200024a8,_mainAddr = 0x1FFF0189;   // BOOT V1.0 Version

#define SRAM_VECTOR_WORD_SIZE   (128)
#define SRAM_VECTOR_ADDR        (SRAM_BASE_ADDR+SRAM_SIZE-0x200)

typedef enum
{
    CMD_CR_SUCCESS  = 0x00,
    CMD_CR_FAILED   = 0x01,
}CMD_RETURN_CR;

CMD_RETURN_CR     (*Get_NVR)(uint32_t addr, uint8_t* data, uint32_t len) = (CMD_RETURN_CR (*)(uint32_t ,uint8_t*,uint32_t))0x1FFF02A5;

/* The pointer of function */
typedef void (*pFunction)(void);


/* Function of how to jump to Boot */
void Jump_To_BOOT(void)
{
    uint8_t value1[8] = {0x00},value2 = 8;
    uint32_t i,*pVec;
    
    /* Init vector */
    pVec = (uint32_t *)SRAM_VECTOR_ADDR;
    for(i=0;i<SRAM_VECTOR_WORD_SIZE;i++)
        pVec[i] = 0;
    
    /* clean up ICache*/
    FLASH_iCacheCmd(DISABLE);
    FLASH_iCacheRST();
    
    /* Get BOOT Version */
    Get_NVR(0x1FFF37A8,value1,value2);
    
    if((value1[0] & 0xFF) == 0x11)    //BOOT V1.1 Version
    {
        SPAddr = 0x200024C0;
        pVec[SysTick_IRQn+16]           = 0x1FFF2F15;
        pVec[USART3_IRQn+16]            = 0x1FFF2F95;
        pVec[USB_FS_LP_IRQn+16]         = 0x1FFF3095;
    }
    else                           //BOOT V1.0 Version
    {
        pVec[SysTick_IRQn+16]           = 0x1FFF2d31;
        pVec[USART3_IRQn+16]            = 0x1FFF2db1;
        pVec[USB_FS_LP_IRQn+16]         = 0x1FFF2eb1;
    }
    /* Disable all interrupt */
    __disable_irq();
    
    /* Config IWDG */
    IWDG_ReloadKey();
    IWDG_WriteConfig(IWDG_WRITE_DISABLE);
    IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV256);
       
    /* Config system clock with HSI */ 
    SetSysClockToHSI();
    
    /* Set JumpBoot addr */
    pFunction JumpBoot = (pFunction)BootAddr;
    
    /* Initalize Stack Pointer */
    __set_MSP(SPAddr);
    
    /* Enable interrupt */
    __enable_irq();
    
    /* Initialize vector table */
    SCB->VTOR = SRAM_VECTOR_ADDR;

    /* Jump to BOOT */
    JumpBoot();
}

/* Main program. */
int main(void)
{
    /* Init debug usart */
    log_init();
    /* Init a button */
    IAP_Button_Config();
    
    /* Debug information */
    printf("Jump to Boot demo start with normal state!\r\n");
    printf("If you press S1(PA4),system will jump to BOOT!\r\n\r\n");
    
    /* Wait button to be pressed */
    while(IAP_Button_Read() == SET);
    printf("System jumped to BOOT!\r\n\r\n");
    
    /*Jump to BOOT */
    Jump_To_BOOT();
    
    /* Infinite loop */
    while(1);
}


/**
*\*\name    SetSysClockToHSI.
*\*\fun     Selects HSI as System clock source and configure HCLK, PCLK2
*\*\         and PCLK1 prescalers.
*\*\param   none
*\*\return  none 
**/
ErrorStatus SetSysClockToHSI(void)
{
    uint32_t timeout_value = 0xFFFFFFFF; 
    ErrorStatus ClockStatus;
    
    RCC_DeInit();

    RCC_EnableHsi(ENABLE);

    /* Wait till HSI is ready */
    ClockStatus = RCC_WaitHsiStable();

    if (ClockStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufSet(FLASH_PrefetchBuf_EN);

        /* Flash 0 wait state */
        FLASH_SetLatency(FLASH_LATENCY_0);

        /* HCLK = SYSCLK */
        RCC_ConfigHclk(RCC_SYSCLK_DIV1);

        /* PCLK2 = HCLK */
        RCC_ConfigPclk2(RCC_HCLK_DIV1);

        /* PCLK1 = HCLK */
        RCC_ConfigPclk1(RCC_HCLK_DIV1);
        
        /* Restore HSI to PLL frequency division as default value */
        RCC->PLLCTRL |= 0x00040000;

        /* Select HSI as system clock source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSI);
           
        /* Wait till HSI is used as system clock source */
        while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_HSI)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* HSI fails  */
        return ERROR;
    }
    return SUCCESS;
}

/* Init a button for test */
void IAP_Button_Config(void)
{  
    GPIO_InitType GPIO_InitStructure;

    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA,ENABLE);

    GPIO_InitStructure.Pin              = GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Slew_Rate   = GPIO_SLEW_RATE_SLOW;
    GPIO_InitStructure.GPIO_Pull        = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Mode        = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

/* Get the state of button */
uint8_t IAP_Button_Read(void)
{
    return GPIO_ReadInputDataBit(GPIOA, GPIO_PIN_4);
}


