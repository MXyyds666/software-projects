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
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_user.h"
#include "usbd_mouse_core.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "usbhs_bsp.h"


#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif
__ALIGN_BEGIN USB_CORE_MODULE USB_dev __ALIGN_END;

typedef enum
{
    BUTTON_NULL = 1u,
    BUTTON_RIGHT,
    BUTTON_LEFT,
    BUTTON_UP,
    BUTTON_DOWN,
}Button_TypeDef;

#define CURSOR_STEP     10u

/**
*\*\name    Key_ReadIOPin_continuous.
*\*\fun     Key status read function.
*\*\param   none.
*\*\return  button id.
*\*\
**/
Button_TypeDef Key_ReadIOPin_continuous(void)
{
    Button_TypeDef enKey = BUTTON_NULL;
    
    if(KEY_Press_Status_Get(KEY_BUTTON_UP_PORT, KEY_BUTTON_UP_PIN))
    {
        enKey = BUTTON_UP;
    }
    else if(KEY_Press_Status_Get(KEY_BUTTON_DOWN_PORT, KEY_BUTTON_DOWN_PIN))
    {
        enKey = BUTTON_DOWN;
    }
    else if(KEY_Press_Status_Get(KEY_BUTTON_LEFT_PORT, KEY_BUTTON_LEFT_PIN))
    {
        enKey = BUTTON_LEFT;
    }
    else if(KEY_Press_Status_Get(KEY_BUTTON_RIGHT_PORT, KEY_BUTTON_RIGHT_PIN))
    {
        enKey = BUTTON_RIGHT;
    }
    else
    {
        enKey = BUTTON_NULL;
    }

    return enKey;
}

/**
*\*\name    get_mouse_pos.
*\*\fun     get the position of the mouse.
*\*\param   none.
*\*\return  Pointer to report buffer.
*\*\
**/
uint8_t* get_mouse_pos(void)
{
    int8_t  x = (int8_t)0, y = (int8_t)0;
    static uint8_t HID_Buffer [4];

    switch (Key_ReadIOPin_continuous())
    {
        case BUTTON_UP:
            y -= (int8_t)CURSOR_STEP;
            break;
        case BUTTON_DOWN:
            y += (int8_t)CURSOR_STEP;
            break;
        case BUTTON_LEFT:
            x -= (int8_t)CURSOR_STEP;
            break;
        case BUTTON_RIGHT:
            x += (int8_t)CURSOR_STEP;
            break;
        default:
            break;
    }
    HID_Buffer[0] = (uint8_t)0;
    HID_Buffer[1] = (uint8_t)x;
    HID_Buffer[2] = (uint8_t)y;
    HID_Buffer[3] = (uint8_t)0;

    return HID_Buffer;
}

/**
*\*\name    USBHS_ConfigCLK.
*\*\fun     Configure USBHS clock.
*\*\param   none.
*\*\return  none.
*\*\
**/
static void USBHS_ConfigCLK(void)
{
    /* Select the corresponding bandwidth and frequency*/
    RCC_ConfigUSBHSBandwidth(RCC_USBHS_BW_16M);
    
    /* Select USBHS clock source frequency */
    RCC_ConfigUSBHSFrequency(RCC_USBHS_FREQ_16_OR_32M);

    /* Select HSE as USBHS clock */
    RCC_ConfigUSBHSClk(RCC_USBHS_CLKSRC_HSE);

    /* Reset the USBHS phy clock*/
    RCC_EnableAHBPeriphReset(RCC_AHBPRST_USBHSPHYRST); 
    
    /* Enables the USBHS peripheral clock*/
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_USBHS, ENABLE);

    /* Set USBHS timing tune */
    RCC->USBHSCTRL2 = (RCC->USBHSCTRL2 & (~(0xF << 4)))  | (0x5 << 4);  //TX vref TUNE
    RCC->USBHSCTRL2 = (RCC->USBHSCTRL2 & (~(0x3 << 12))) | (0x3 << 12); //TX Rise TUNE
    RCC->USBHSCTRL2 = (RCC->USBHSCTRL2 & (~(0x3 << 14))) | (0x3 << 14); //TX Res TUNE
    RCC->USBHSCTRL2 = (RCC->USBHSCTRL2 & (~(0x3 << 16))) | (0x3 << 16); //TX preempamp TUNE
}


/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none.
*\*\return  none.
*\*\
**/
int main(void)
{
    USBHS_ConfigCLK();

    USBD_Init(&USB_dev, &USBD_desc, &USBD_MOUSE_cb, &USER_cb);

    while (1)
    {
    }
}
