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
#include "misc.h"
#include "log.h"
#include "n32h47x_48x_flash.h"
#include "n32h47x_48x_usart.h"
#include <stdio.h>

/**
*\*\name    main.
*\*\fun     main program.
*\*\param   none
*\*\return  none 
**/
int main(void)
{
    uint32_t Value_temp;
    /* USART Init */
    log_init();
#if BOOT0_START
    log_info(" Boot start in Flash 0x8000000 area !\r\n");
#endif
#if BOOT1_START
    NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x8040000);
    log_info(" Boot start in Flash 0x8040000 area !\r\n");
#endif
    
    while (1)
    {
        /* Loop until UART1 DAT register is not empty */
        while (USART_GetFlagStatus(USART1, USART_FLAG_RXDNE) == RESET)
        {
        }

        Value_temp = USART_ReceiveData(USART1);
        
        /* Received reset command from host computer */
#if BOOT0_START
        if(Value_temp == 0x55)
#endif
#if BOOT1_START
        if(Value_temp == 0xAA)
#endif        
        {
            FLASH_Unlock();
            FLASH_EraseOB();
            FLASH_ProgramOB_RUDD(FLASH_OB_RDP1_DISABLE,FLASH_OB_IWDG_SOFTWARE,FLASH_OB_STOP_NORST,\
                                 FLASH_OB_STDBY_NORST,FLASH_OB_IWDG_STOP_NOFRZ,FLASH_OB_IWDG_STDBY_NOFRZ,\
                                 FLASH_OB_IWDG_SLEEP_NOFRZ,0xFF,0xFF);
#if BOOT0_START
            /* Configure the FLASH boot address to 0x8040000 */
            FLASH_ProgramOB_RU2U3(FLASH_OB_RDP2_DISABLE,FLASH_OB2_NBOOT0_SET,FLASH_OB2_NBOOT1_SET,\
                                  FLASH_OB2_NSWBOOT0_SET,FLASH_OB2_FLASHBOOT_CLR,0x00,0x00);
#endif
#if BOOT1_START
            /* Configure the FLASH boot address to 0x8000000 */
            FLASH_ProgramOB_RU2U3(FLASH_OB_RDP2_DISABLE,FLASH_OB2_NBOOT0_SET,FLASH_OB2_NBOOT1_SET,\
                                  FLASH_OB2_NSWBOOT0_SET,FLASH_OB2_FLASHBOOT_SET,0x00,0x00);
#endif            
            log_info("System reset !\r\n");
            NVIC_SystemReset();
        }
    }
}

