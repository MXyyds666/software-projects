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
*\*\file usb_desc.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "usbd_desc.h"

#define USBD_VID                     0x19F5
#define USBD_PID                     0x5810

#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "Nations"
#define USBD_PRODUCT_HS_STRING        "Composite HID MSC in HS Mode"
#define USBD_PRODUCT_FS_STRING        "Composite HID MSC in FS Mode"
#define USBD_CONFIGURATION_HS_STRING  "Composite HID MSC Config"
#define USBD_INTERFACE_HS_STRING      "Composite HID MSC Interface"
#define USBD_CONFIGURATION_FS_STRING  "Composite HID MSC Config"
#define USBD_INTERFACE_FS_STRING      "Composite HID MSC Interface"

USBD_DEVICE_DESC USBD_desc = 
{
    USBD_USER_DeviceDescriptor,
    USBD_USER_LangIDStrDescriptor,
    USBD_USER_ManufacturerStrDescriptor,
    USBD_USER_ProductStrDescriptor,
    USBD_USER_SerialStrDescriptor,
    USBD_USER_ConfigStrDescriptor,
    USBD_USER_InterfaceStrDescriptor,
};

#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
/* USB Standard Device Descriptor */
__ALIGN_BEGIN uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] __ALIGN_END = 
{
    0x12,                         /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,   /* bDescriptorType */
    0x00,                         /* bcdUSB */
    0x02,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    USB_MAX_EP0_SIZE,         /* bMaxPacketSize */
    LOBYTE(USBD_VID),             /* idVendor */
    HIBYTE(USBD_VID),             /* idVendor */
    LOBYTE(USBD_PID),             /* idVendor */
    HIBYTE(USBD_PID),             /* idVendor */
    0x00,                         /* bcdDevice rel. 2.00 */
    0x02,
    USBD_IDX_MFC_STR,             /* Index of manufacturer string */
    USBD_IDX_PRODUCT_STR,         /* Index of product string */
    USBD_IDX_SERIAL_STR,          /* Index of serial number string */
    USBD_CFG_MAX_NUM              /* bNumConfigurations */
};                              /* USB_DeviceDescriptor */

#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
/* USB Standard Device Descriptor */
__ALIGN_BEGIN uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]
  __ALIGN_END = 
{
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
};

#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
/* USB Standard Device Descriptor */
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID] __ALIGN_END = 
{
    USB_SIZ_STRING_LANGID,
    USB_DESC_TYPE_STRING,
    LOBYTE(USBD_LANGID_STRING),
    HIBYTE(USBD_LANGID_STRING),
};

uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] = 
{
    USB_SIZ_STRING_SERIAL,
    USB_DESC_TYPE_STRING,
};

#ifdef USB_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ )      /* !< IAR Compiler */
#pragma data_alignment=4
#endif
#endif                          /* USB_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN uint8_t USBD_StrDesc[USB_MAX_STR_DESC_SIZ] __ALIGN_END;

static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len);
static void Get_SerialNum(void);

/**
*\*\name   USBD_USER_DeviceDescriptor.
*\*\fun    return the device descriptor.
*\*\param  speed : current device speed
*\*\param  length : pointer to data length variable
*\*\return pointer to descriptor buffer
*/
uint8_t *USBD_USER_DeviceDescriptor(uint8_t speed, uint16_t * length)
{
    *length = sizeof(USBD_DeviceDesc);
    return (uint8_t *) USBD_DeviceDesc;
}

/**
*\*\name   USBD_USER_LangIDStrDescriptor.
*\*\fun    return the LangID string descriptor.
*\*\param  speed : current device speed
*\*\param  length : pointer to data length variable
*\*\return pointer to descriptor buffer
*/
uint8_t *USBD_USER_LangIDStrDescriptor(uint8_t speed, uint16_t * length)
{
    *length = sizeof(USBD_LangIDDesc);
    return (uint8_t *) USBD_LangIDDesc;
}

/**
*\*\name   USBD_USER_ProductStrDescriptor.
*\*\fun    return the product string descriptor.
*\*\param  speed : current device speed
*\*\param  length : pointer to data length variable
*\*\return pointer to descriptor buffer
*/
uint8_t *USBD_USER_ProductStrDescriptor(uint8_t speed, uint16_t * length)
{
    if (speed == USB_SPEED_HIGH)
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
    }
    else
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
    }
    return USBD_StrDesc;
}

/**
*\*\name   USBD_USER_ManufacturerStrDescriptor.
*\*\fun    return the manufacturer string descriptor.
*\*\param  speed : current device speed
*\*\param  length : pointer to data length variable
*\*\return pointer to descriptor buffer
*/
uint8_t *USBD_USER_ManufacturerStrDescriptor(uint8_t speed, uint16_t * length)
{
    USBD_GetString((uint8_t *) (uint8_t *) USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}

/**
*\*\name   USBD_USER_SerialStrDescriptor.
*\*\fun    return the serial number string descriptor.
*\*\param  speed : current device speed
*\*\param  length : pointer to data length variable
*\*\return pointer to descriptor buffer
*/
uint8_t *USBD_USER_SerialStrDescriptor(uint8_t speed, uint16_t * length)
{
    *length = USB_SIZ_STRING_SERIAL;

    /* Update the serial number string descriptor with the data from the unique * ID */
    Get_SerialNum();

    return (uint8_t *) USBD_StringSerial;
}

/**
*\*\name   USBD_USER_ConfigStrDescriptor.
*\*\fun    return the configuration string descriptor.
*\*\param  speed : current device speed
*\*\param  length : pointer to data length variable
*\*\return pointer to descriptor buffer
*/
uint8_t *USBD_USER_ConfigStrDescriptor(uint8_t speed, uint16_t * length)
{
    if (speed == USB_SPEED_HIGH)
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
    }
    else
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
    }
    return USBD_StrDesc;
}

/**
*\*\name   USBD_USER_InterfaceStrDescriptor.
*\*\fun    return the interface string descriptor.
*\*\param  speed : current device speed
*\*\param  length : pointer to data length variable
*\*\return pointer to descriptor buffer
*/
uint8_t *USBD_USER_InterfaceStrDescriptor(uint8_t speed, uint16_t * length)
{
    if (speed == 0)
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
    }
    else
    {
        USBD_GetString((uint8_t *) (uint8_t *) USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
    }
    return USBD_StrDesc;
}

/**
*\*\name   Get_SerialNum.
*\*\fun    Create the serial number string descriptor .
*\*\param  none
*\*\param  none
*\*\return pointer to descriptor buffer
*/
static void Get_SerialNum(void)
{
    uint32_t deviceserial0, deviceserial1, deviceserial2;

    deviceserial0 = *(uint32_t *) UCID_BASE;
    deviceserial1 = *(uint32_t *) (UCID_BASE + 4);
    deviceserial2 = *(uint32_t *) (UCID_BASE + 8);

    deviceserial0 += deviceserial2;

    if (deviceserial0 != 0)
    {
        IntToUnicode(deviceserial0, &USBD_StringSerial[2], 8);
        IntToUnicode(deviceserial1, &USBD_StringSerial[18], 4);
    }
}

/**
*\*\name   IntToUnicode.
*\*\fun    Convert Hex 32Bits value into char.
*\*\param  value: value to convert
*\*\param  pbuf: pointer to the buffer 
*\*\param  len: buffer length
*\*\return none.
*/
static void IntToUnicode(uint32_t value, uint8_t * pbuf, uint8_t len)
{
    uint8_t idx = 0;

    for (idx = 0; idx < len; idx++)
    {
        if (((value >> 28)) < 0xA)
        {
            pbuf[2 * idx] = (value >> 28) + '0';
        }
        else
        {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }
        value = value << 4;
        pbuf[2 * idx + 1] = 0;
    }
}

