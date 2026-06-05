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

/* USB Standard Device Descriptor */
const uint8_t Composite_DeviceDescriptor[COMPOSITE_SIZ_DEVICE_DESC] =
{
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x00,                       /*bcdUSB */
    0x02,
    0xEF,                       /* bDeviceClass */
    0x02,                       /* bDeviceSubClass */
    0x01,                       /* bDeviceProtocol */
    0x40,                       /*bMaxPacketSize40*/
    0xF5,                       /*idVendor (0x19F5)*/
    0x19,
    0x10,                       /*idProduct = 0x5610*/
    0x56,
    0x00,                       /*bcdDevice rel. 2.00*/
    0x02,
    1,                          /*Index of string descriptor describing
                                                                                        manufacturer */
    2,                          /*Index of string descriptor describing
                                                                                     product*/
    3,                          /*Index of string descriptor describing the
                                                                                     device serial number */
    0x01                        /*bNumConfigurations*/
}; /* COMPOSITE_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t Composite_ConfigDescriptor[COMPOSITE_SIZ_CONFIG_DESC] =
{
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    COMPOSITE_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x03,         /* bNumInterfaces: 2 interfaces */
    0x01,         /* bConfigurationValue: Configuration value */
    0x00,         /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0xC0,         /* bmAttributes: Self powered */
    0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x00,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    0x02,         /* bNumEndpoints */
    0x03,         /* bInterfaceClass: HID */
    0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,            /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID ********************/
    /* 18 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    CUSTOMHID_SIZ_REPORT_DESC,/* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

    0x81,          /* bEndpointAddress: Endpoint Address (IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    0x02,          /* wMaxPacketSize: 2 Bytes max */
    0x00,
    0x20,          /* bInterval: Polling Interval (32 ms) */
    /* 34 */

    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: */
            /* Endpoint descriptor type */
    0x01,   /* bEndpointAddress: */
            /* Endpoint Address (OUT) */
    0x03,   /* bmAttributes: Interrupt endpoint */
    0x02,   /* wMaxPacketSize: 2 Bytes max  */
    0x00,
    0x20,   /* bInterval: Polling Interval (20 ms) */
    /* 41 */
    
    /******** /IAD should be positioned just before the CDC interfaces ******
               IAD to associate the two CDC interfaces */
    0x08, /* bLength */
    0x0B, /* bDescriptorType */
    0x01, /* bFirstInterface */
    0x02, /* bInterfaceCount */
    0x02, /* bFunctionClass */
    0x02, /* bFunctionSubClass */
    0x01, /* bFunctionProtocol */
    0x00, /* iFunction (Index of string descriptor describing this function) */

    /*Interface Descriptor */
    0x09,   /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x01,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x01,   /* iInterface: */
    
    /*Header Functional Descriptor*/
    0x05,   /* bLength: Endpoint Descriptor size */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x00,   /* bDescriptorSubtype: Header Func Desc */
    0x10,   /* bcdCDC: spec release number */
    0x01,

    /*Call Management Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x01,   /* bDescriptorSubtype: Call Management Func Desc */
    0x00,   /* bmCapabilities: D0+D1 */
    0x02,   /* bDataInterface: 2 */

    /*ACM Functional Descriptor*/
    0x04,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
    0x02,   /* bmCapabilities */

    /*Union Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x06,   /* bDescriptorSubtype: Union func desc */
    0x01,   /* bMasterInterface: Communication class interface */
    0x02,   /* bSlaveInterface0: Data Class Interface */

    /*Endpoint 2 Descriptor*/
    0x07,                           /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x82,                           /* bEndpointAddress */
    0x03,                           /* bmAttributes: Interrupt */
    VIRTUAL_COM_PORT_INT_SIZE,      /* wMaxPacketSize: */
    0x00,
    0xFF,                           /* bInterval: */

    /*---------------------------------------------------------------------------*/

    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    0x02,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: Two endpoints used */
    0x0A,   /* bInterfaceClass: CDC */
    0x00,   /* bInterfaceSubClass: */
    0x00,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */

    /*Endpoint 3 OUT Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
    0x03,                              /* bEndpointAddress */
    0x02,                              /* bmAttributes: Bulk */
    VIRTUAL_COM_PORT_DATA_SIZE,        /* wMaxPacketSize: */
    0x00,
    0x00,                              /* bInterval: ignore for Bulk transfer */

    /*Endpoint 3 IN Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType: Endpoint */
    0x83,                             /* bEndpointAddress: (IN3) */
    0x02,                             /* bmAttributes: Bulk */
    VIRTUAL_COM_PORT_DATA_SIZE,       /* wMaxPacketSize: */
    0x00,
    0x00, 
    
}; /* Composite_ConfigDescriptor */


const uint8_t CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] =
{                    
    0x06, 0xFF, 0x00,      /* USAGE_PAGE (Vendor Page: 0xFF00) */
    0x09, 0x01,            /* USAGE (Demo Kit)               */
    0xa1, 0x01,            /* COLLECTION (Application)       */
    /* 6 */
    
    /* Led 1 */        
    0x85, 0x01,            /*     REPORT_ID (1)              */
    0x09, 0x01,            /*     USAGE (LED 1)              */
    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
    0x75, 0x08,            /*     REPORT_SIZE (8)            */
    0x95, 0x01,            /*     REPORT_COUNT (1)           */
    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */

    0x85, 0x01,            /*     REPORT_ID (1)              */
    0x09, 0x01,            /*     USAGE (LED 1)              */
    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
    /* 26 */
    
    /* Led 2 */
    0x85, 0x02,            /*     REPORT_ID 2                */
    0x09, 0x02,            /*     USAGE (LED 2)              */
    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
    0x75, 0x08,            /*     REPORT_SIZE (8)            */
    0x95, 0x01,            /*     REPORT_COUNT (1)           */
    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */

    0x85, 0x02,            /*     REPORT_ID (2)              */
    0x09, 0x02,            /*     USAGE (LED 2)              */
    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
    /* 46 */
    
    /* Led 3 */        
    0x85, 0x03,            /*     REPORT_ID (3)              */
    0x09, 0x03,            /*     USAGE (LED 3)              */
    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
    0x75, 0x08,            /*     REPORT_SIZE (8)            */
    0x95, 0x01,            /*     REPORT_COUNT (1)           */
    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */

    0x85, 0x03,            /*     REPORT_ID (3)              */
    0x09, 0x03,            /*     USAGE (LED 3)              */
    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
    /* 66 */
    
    /* Led 4 */
    0x85, 0x04,            /*     REPORT_ID (4)               */
    0x09, 0x04,            /*     USAGE (LED 4)              */
    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
    0x75, 0x08,            /*     REPORT_SIZE (8)            */
    0x95, 0x01,            /*     REPORT_COUNT (1)           */
    0xB1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */

    0x85, 0x04,            /*     REPORT_ID (4)              */
    0x09, 0x04,            /*     USAGE (LED 4)              */
    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
    /* 86 */
    
    /* key Push Button */  
    0x85, 0x05,            /*     REPORT_ID (5)              */
    0x09, 0x05,            /*     USAGE (Push Button)        */
    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */
    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */
    0x75, 0x01,            /*     REPORT_SIZE (1)            */
    0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */
    
    0x09, 0x05,            /*     USAGE (Push Button)        */
    0x75, 0x01,            /*     REPORT_SIZE (1)            */
    0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */
         
    0x75, 0x07,            /*     REPORT_SIZE (7)            */
    0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */
    0x85, 0x05,            /*     REPORT_ID (2)              */
                    
    0x75, 0x07,            /*     REPORT_SIZE (7)            */
    0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */
    /* 114 */

    0xc0                   /*     END_COLLECTION            */
}; /* CustomHID_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t Composite_StringLangID[COMPOSITE_SIZ_STRING_LANGID] =
{
    COMPOSITE_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
}; /* LangID = 0x0409: U.S. English */

const uint8_t Composite_StringVendor[COMPOSITE_SIZ_STRING_VENDOR] = 
{
    COMPOSITE_SIZ_STRING_VENDOR, /* Size of manufaturer string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "NATIONS" */
    'N',0,'A',0,'T',0,'I',0,'O',0,'N',0,'S',0
};

const uint8_t Composite_StringProduct[COMPOSITE_SIZ_STRING_PRODUCT] = 
{
    COMPOSITE_SIZ_STRING_PRODUCT, /* bLength */
    USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
    'N',0,'3',0,'2',0,'H',0,'4',0,'x',0,'x', 0, ' ', 0,'C', 0, 'D', 0, 'C', 0, '+', 0, 'H', 0, 'I', 0, 'D', 0
};

uint8_t Composite_StringSerial[COMPOSITE_SIZ_STRING_SERIAL] = 
{
    COMPOSITE_SIZ_STRING_SERIAL, /* bLength */
    USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
    'N',0,'3',0,'2',0,'H',0,'4',0,'x',0,'x',0
};

