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
*\*\file usb_cdc_vcp.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifdef USB_INTERNAL_DMA_ENABLED
#pragma     data_alignment = 4
#endif                          /* USB_INTERNAL_DMA_ENABLED */


#include "usbd_cdc_vcp.h"

__IO uint32_t data_sent;
__IO uint32_t receive_flag = 0;
extern __IO uint32_t receive_count;

#define USB_SEND_TIMEOUT        0x20000

/* Private function prototypes ----------------------------------------------- */
static uint16_t VCP_DataTx(void);
static uint16_t VCP_DataRx(uint32_t Len);

CDC_IF_Prop_TypeDef VCP_fops = 
{
    VCP_DataTx,
    VCP_DataRx
};

/**
*\*\name    VCP_DataTx.
*\*\fun     CDC received data to be send over USB IN endpoint are managed in this function.
*\*\param   none
*\*\return  USBD_OK
**/
static uint16_t VCP_DataTx(void)
{
    data_sent = 1;

    return USBD_OK;
}

/**
*\*\name    VCP_DataRx.
*\*\fun     Data received from USB are sent to uart port.
*\*\param   Len: data length in bytes
*\*\return  USBD_OK
**/
static uint16_t VCP_DataRx(uint32_t Len)
{
    receive_count = Len;

    return USBD_OK;
}

/**
*\*\name    VCP_SendData.
*\*\fun     send data function.
*\*\param   pdev: selected device.
*\*\param   pbuf: send data pointer.
*\*\param   buf_len: send data length.
*\*\return  USBD_OK
**/
void VCP_SendData(USB_CORE_MODULE * pdev, uint8_t * pbuf, uint32_t buf_len)
{
    uint32_t temp_value = 0;
    uint32_t max_packet_size = 0;
#ifdef USE_USB_HS_IN_HS
    max_packet_size = USB_HS_MAX_PACKET_SIZE;
#else
    max_packet_size = USB_FS_MAX_PACKET_SIZE;
#endif
    
    if(buf_len < max_packet_size)
    {
        data_sent = 0;
        USBDEV_EP_Tx(pdev, CDC_IN_EP, pbuf, buf_len);
    }
    else if(buf_len == max_packet_size)
    {
        data_sent = 0;
        USBDEV_EP_Tx(pdev, CDC_IN_EP, pbuf, buf_len);
        while(data_sent != 1 && temp_value < USB_SEND_TIMEOUT)
        {
            temp_value++;
        }
        data_sent = 0;
        USBDEV_EP_Tx(pdev, CDC_IN_EP, pbuf, 0);
    }
    else
    {
    }
}

/**
*\*\name    VCP_ReceiveData.
*\*\fun     prepare data to be received.
*\*\param   pdev: selected device.
*\*\param   pbuf: receive data pointer.
*\*\param   buf_len: receive data length.
*\*\return  none
**/
void VCP_ReceiveData(USB_CORE_MODULE * pdev, uint8_t * pbuf, uint32_t buf_len)
{
    receive_flag = 0;
    USBDEV_EP_PrepareRx(pdev, CDC_OUT_EP, pbuf, buf_len);
}
