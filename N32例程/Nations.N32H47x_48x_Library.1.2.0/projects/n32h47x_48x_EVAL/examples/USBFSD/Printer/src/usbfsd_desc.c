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
*\*\file usbfsd_desc.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "usbfsd_lib.h"
#include "usbfsd_desc.h"

/*---------- USB Standard Device Descriptor ----------*/
const uint8_t Printer_DeviceDescriptor[18] = 
{
    0x12,   /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,   /* bDescriptorType */  
    0x00,
    0x02,   /* bcdUSB = 2.00 */
    0x00,   /* bDeviceClass: CDC */ 
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    0x40,   /* bMaxPacketSize0 */
    0xf5,   /*idVendor (0x19F5)*/
    0x19,   //19
    0x90,   /*idProduct = (0x5590)*/
    0x55,   //32 
    0x00,
    0x02,   /* bcdDevice = 2.00 */
    1,              /* Index of string descriptor describing manufacturer */
    2,              /* Index of string descriptor describing product */
    3,              /* Index of string descriptor describing the device's serial number */
    0x01    /* bNumConfigurations */
};      


/*---------- USB Configuration Descriptor ----------*/
const uint8_t Printer_ConfigDescriptor[CONFIG_DESC_SIZE] = 
{
    /**---------- Configure ----------*/
    0x09,               /* bLength              */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,          /* bDescriptorType      */
    CONFIG_DESC_SIZE,   /* wTotalLength         */
    0x00,         
    0x01,               /* bNumInterfaces       */
    0x01,               /* bConfigurationValue  */
    0x00,               /* iConfiguration       */
    0xC0,               /* bmAttributes         */
    0x32,               /* MaxPower             */

    /**---------- ONE INTERFACE descriptor ----------*/
    0x09,               /* bLength              */
    USB_INTERFACE_DESCRIPTOR_TYPE,              /* bDescriptorType      */
    0x00,               /* bInterfaceNumber     */
    0x00,               /* bAlternateSetting    */
    0x02,               /* bNumEndpoints        */
    0x07,               /* bInterfaceClass   07:printer   */
    0x01,               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x02,               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0x00,               /* iInterface           */

    /* ENDPOINT descriptor */
    0x07,               /* bLength          */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* bDescriptorType  */
    0x82,               /* In bEndpointAddress EP2IN*/
    0x02,               /* bulk             */
    EP2_IN_SIZE,        /* wMaxPacketSize   */
    0x00,
    0x00, /* bInterval        */

    /* ENDPOINT descriptor */
    0x07,               /* bLength          */
    USB_ENDPOINT_DESCRIPTOR_TYPE,           /* bDescriptorType  */
    0x02,               /* Out bEndpointAddress  EP2OUT*/
    0x02,               /* bulk             */
    EP2_OUT_SIZE,       /* wMaxPacketSize   */
    0x00,
    0x00,               /* bInterval        */
};

/*---------- USB String Descriptors (optional) ----------*/
const uint8_t Printer_StringLangID[Printer_SIZ_STRING_LANGID] = 
{
    Printer_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,       /* LangID = 0x0409: SB_EP7R_STAT_RX_1.S. English */
    0x04
}; 


/**----------  ----------*/
const uint8_t Printer_StringVendor[Printer_SIZ_STRING_VENDOR] = 
{
    Printer_SIZ_STRING_VENDOR,  /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType*/
    /* Manufacturer: "NATIONS" */
    'N', 0,
    'A', 0,
    'T', 0,
    'I', 0,
    'O', 0,
    'N', 0,
    'S', 0
};

/**----------  ----------*/
const uint8_t Printer_StringProduct[Printer_SIZ_STRING_PRODUCT] = 
{
    Printer_SIZ_STRING_PRODUCT, /* bLength */
    USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
    'N', 0,
    '3', 0,
    '2', 0,
    'H', 0,
    '4', 0,
    'x', 0,
    'x', 0,
    'C', 0,
    'u', 0,
    's', 0,
    't', 0,
    'm', 0,
    ' ', 0,
    'P', 0,
    'R', 0,
    'I', 0};

/**----------  ----------*/
uint8_t Printer_StringSerial[Printer_SIZ_STRING_SERIAL] = 
{
    Printer_SIZ_STRING_SERIAL,  /* bLength */
    USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
    'N', 0,
    '3', 0,
    '2', 0,
    'H', 0,
    '4', 0,
    'x', 0,
    'x', 0
};

