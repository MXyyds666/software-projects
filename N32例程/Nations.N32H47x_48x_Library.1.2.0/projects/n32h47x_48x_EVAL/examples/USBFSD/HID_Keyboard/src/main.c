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

#include "n32h47x_48x.h"
#include "hw_config.h"
#include "usbfsd_lib.h"
#include "usbfsd_pwr.h"
#include "n32h47x_48x_flash.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"

__IO uint8_t PrevXferComplete = 1;
uint8_t key_buffer[8] = {0};
u8* Ep1DataPtr = 0;

extern __IO uint8_t bIntPackSOF;
uint8_t LastIntPackSOF = 0;
uint8_t remotewakeup_flag = 0;


/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none.
*\*\return  none.
*\*\
**/
int main(void)
{
    uint32_t system_clock = 0;
    uint16_t i;
    
    Cfg_KeyIO();
    USBFS_IO_Configure();
    USB_Interrupts_Config();

#if defined (N32H473) || defined (N32H474)
    system_clock = SYSCLK_VALUE_192MHz;
#elif defined (N32H482) || defined (N32H485) || defined (N32H487) || defined (N32H488)
    system_clock = SYSCLK_VALUE_240MHz;
#elif defined (N32H480)
    system_clock = SYSCLK_VALUE_144MHz;
#endif
    
    if(USB_Config(system_clock) == SUCCESS)
    {
        USB_Init();

        while (bDeviceState != CONFIGURED)
        {
        }

        while (1)
        {
            if (bDeviceState == SUSPENDED)
            {
                LastIntPackSOF = bIntPackSOF;
            
                for (i = 0; i < 0xffff; i++);
                
                if (LastIntPackSOF != bIntPackSOF)
                {
                    bDeviceState = CONFIGURED;
                }
            }
            if (bDeviceState == CONFIGURED)
            {
                if (remotewakeup_flag == 0)
                {
                    if (PrevXferComplete)
                    {
                        if (!GPIO_ReadInputDataBit(KEY_A_PORT, KEY_A_PIN))
                        {
                            key_buffer[2] = 0x04;
                        }
                        else if (!GPIO_ReadInputDataBit(KEY_B_PORT, KEY_B_PIN))
                        {
                            key_buffer[2] = 0x05;
                        }
                        else if (!GPIO_ReadInputDataBit(KEY_C_PORT, KEY_C_PIN))
                        {
                            key_buffer[2] = 0x06;
                        }
                        else
                        {
                            key_buffer[2] = 0x00;
                        }
                        if (key_buffer[2] != 0)
                        {
                            PrevXferComplete = 0;
                            Ep1DataPtr       = key_buffer;
                            USB_SilWrite(EP1_IN, Ep1DataPtr, 8);
                            _SetEPTxStatus(ENDP1, EP_TX_VALID);
                            
                            while(!PrevXferComplete);
                            while((GPIO_ReadInputDataBit(KEY_A_PORT, KEY_A_PIN) == 0) && 
                                  (GPIO_ReadInputDataBit(KEY_B_PORT, KEY_B_PIN) == 0) &&
                                  (GPIO_ReadInputDataBit(KEY_C_PORT, KEY_C_PIN) == 0));
                            key_buffer[2] = 0x00;
                            Ep1DataPtr       = key_buffer;
                            USB_SilWrite(EP1_IN, Ep1DataPtr, 8);
                            _SetEPTxStatus(ENDP1, EP_TX_VALID);
                        }
                    }
                }
                else
                {
                    remotewakeup_flag = 0;

                    key_buffer[2] = 0x05;
                    if (key_buffer[2] != 0)
                    {
                        PrevXferComplete = 0;
                        Ep1DataPtr       = key_buffer;
                        USB_SilWrite(EP1_IN, Ep1DataPtr, 8);
                        _SetEPTxStatus(ENDP1, EP_TX_VALID);
                    }
                    
                    while(!PrevXferComplete);
                    
                    key_buffer[2] = 0x00;
                    Ep1DataPtr = key_buffer;
                    USB_SilWrite(EP1_IN, Ep1DataPtr, 8);
                    _SetEPTxStatus(ENDP1, EP_TX_VALID);
                    
                    while(!PrevXferComplete);
                }
            }
        }
    }
    while(1)
    {
    }
}
