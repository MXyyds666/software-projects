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
*\*\file usbfsd_prop.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "hw_config.h"
#include "usbfsd_lib.h"
#include "usbfsd_conf.h"
#include "usbfsd_prop.h"
#include "usbfsd_desc.h"
#include "usbfsd_pwr.h"

uint32_t ProtocolValue;
__IO uint8_t EXTI_Enable;
__IO uint8_t Request = 0;
uint8_t Report_Buf[2];

typedef struct
{
    uint32_t    bitrate;
    uint8_t     format;
    uint8_t     paritytype;
    uint8_t     datatype;
}LINE_CODING;

LINE_CODING linecoding =
{
    115200,     /* baud rate*/
    0x00,       /* stop bits-1*/
    0x00,       /* parity - none*/
    0x08        /* no. of bits 8*/
};

uint8_t USB_Printer_IDString[103] = 
{
    0x00, 0x67,
    'M', 'A', 'N', 'U', 'F', 'A', 'C', 'T', 'U', 'R', 'E', 'R', ':',
    'N', 'S', '3', '2', ' ', 'D', 'E', 'V', 'I', 'C', 'E', '-', ';',
    'C', 'O', 'M', 'M', 'A', 'N', 'D', ' ', 'S', 'E', 'T', ':',
    'P', 'C', 'L', ',', 'M', 'P', 'L', ';',
    'M', 'O', 'D', 'E', 'L', ':',
    'L', 'a', 's', 'e', 'r', 'B', 'e', 'a', 'm', '?', ';',
    'C', 'O', 'M', 'M', 'E', 'N', 'T', ':',
    'G', 'o', 'o', 'd', ' ', '!', ';',
    'A', 'C', 'T', 'I', 'V', 'E', ' ', 'C', 'O', 'M', 'M', 'A', 'N', 'D', ' ', 'S', 'E', 'T', ':',
    'P', 'C', 'L', ';'
};



/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

USB_Device Device_Table = {EP_NUM, 1};


/**---------- Device call back function ----------*/
DEVICE_PROP Device_Property = 
{
    Printer_init,
    Printer_Reset,
    Printer_Status_In,
    Printer_Status_Out,
    Printer_Data_Setup,
    Printer_NoData_Setup,
    Printer_Get_Interface_Setting,
    Printer_GetDeviceDescriptor,
    Printer_GetConfigDescriptor,
    Printer_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
};

/**---------- USB Standard requests function ----------*/
USER_STANDARD_REQUESTS User_Standard_Requests = 
{
    Printer_GetConfiguration,
    Printer_SetConfiguration,
    Printer_GetInterface,
    Printer_SetInterface,
    Printer_GetStatus,
    Printer_ClearFeature,
    Printer_SetEndPointFeature,
    Printer_SetDeviceFeature,
    Printer_SetDeviceAddress
};

/**---------- ----------*/
USB_OneDescriptor Device_Descriptor = 
{
    (uint8_t*)Printer_DeviceDescriptor, 
    18
};

/**---------- ----------*/
USB_OneDescriptor Config_Descriptor = 
{
    (uint8_t*)Printer_ConfigDescriptor, 
    CONFIG_DESC_SIZE
};

/**---------- ----------*/
USB_OneDescriptor Printer_Descriptor = 
{
    (uint8_t*)Printer_ConfigDescriptor + Printer_OFF_DESC,
    Printer_SIZ_DESC
};

/**---------- ----------*/
USB_OneDescriptor String_Descriptor[4] = 
{
    {(uint8_t*)Printer_StringLangID,    Printer_SIZ_STRING_LANGID},
    {(uint8_t*)Printer_StringVendor,    Printer_SIZ_STRING_VENDOR},
    {(uint8_t*)Printer_StringProduct,   Printer_SIZ_STRING_PRODUCT},
    {(uint8_t*)Printer_StringSerial,    Printer_SIZ_STRING_SERIAL}
};



/*Printer_SetReport_Feature function prototypes*/
uint8_t* Printer_SetReport_Feature(uint16_t Length);


/**
*\*\name    Printer_init.
*\*\fun     Printer init routine.
*\*\param   none
*\*\return  none 
**/
void Printer_init(void)
{
    /* Update the serial number string descriptor with the data from the unique
    ID*/
    Get_SerialNum();


    pInformation->CurrentConfiguration = 0;
    /* Connect the device */
    PowerOn();

    /* Perform basic device initialization operations */
    USB_SilInit();
    
    /*Pull up DP*/
    _EnPortPullup();

    bDeviceState = UNCONNECTED;
}

/**
*\*\name    Printer_Reset.
*\*\fun     Printer reset routine.
*\*\param   none
*\*\return  none 
**/
void Printer_Reset(void)
{
    /* Set Printer_DEVICE as not configured */
    pInformation->CurrentConfiguration = 0;
    pInformation->CurrentInterface     = 0; /*the default Interface*/

    /* Current Feature initialization */
    pInformation->CurrentFeature = Printer_ConfigDescriptor[7];

    USB_SetBuftab(BTABLE_ADDRESS);

    /* Initialize Endpoint 0 */
    USB_SetEpType(ENDP0, EP_CONTROL);
    SetEPTxStatus(ENDP0, EP_TX_STALL);
    USB_SetEpRxAddr(ENDP0, ENDP0_RXADDR);
    USB_SetEpTxAddr(ENDP0, ENDP0_TXADDR);
    USB_ClrStsOut(ENDP0);
    USB_SetEpRxCnt(ENDP0, Device_Property.MaxPacketSize);
    USB_SetEpRxValid(ENDP0);

    /* Initialize Endpoint 2 */
    USB_SetEpType(ENDP2, EP_BULK);
    USB_SetEpTxAddr(ENDP2, ENDP2_TXADDR);
    USB_SetEpRxAddr(ENDP2, ENDP2_RXADDR);
    USB_SetEpTxCnt(ENDP2, 0);
    USB_SetEpRxCnt(ENDP2, EP2_OUT_SIZE);
    SetEPRxStatus(ENDP2, EP_RX_VALID);
    SetEPTxStatus(ENDP2, EP_TX_VALID);

    /* Set this device to response on default address */
    USB_SetDeviceAddress(0);
    bDeviceState = ATTACHED;
}

/**
*\*\name    Printer_SetConfiguration.
*\*\fun     Update the device state to configured.
*\*\param   none
*\*\return  none 
**/
void Printer_SetConfiguration(void)
{
    if (pInformation->CurrentConfiguration != 0)
    {
        /* Device configured */
        bDeviceState = CONFIGURED;
    }
}

/**
*\*\name    Printer_SetDeviceAddress.
*\*\fun     Update the device state to addressed.
*\*\param   none
*\*\return  none 
**/
void Printer_SetDeviceAddress(void)
{
    bDeviceState = ADDRESSED;
}

/**
*\*\name    Printer_Status_In.
*\*\fun     Printer status IN routine.
*\*\param   none
*\*\return  none 
**/
void Printer_Status_In(void)
{
}

/**
*\*\name    Printer_Status_Out.
*\*\fun     Printer status OUT routine.
*\*\param   none
*\*\return  none 
**/
void Printer_Status_Out(void)
{
}

/**
*\*\name    Printer_Data_Setup.
*\*\fun     Handle the data class specific requests.
*\*\param   RequestNo
*\*\            - GET_DESCRIPTOR
*\*\            - GET_PROTOCOL
*\*\            - SET_REPORT
*\*\return  USB_Result
*\*\            - Success
*\*\            - UnSupport
**/
USB_Result Printer_Data_Setup(uint8_t RequestNo)
{
    uint8_t    *(*CopyRoutine)(uint16_t);

    CopyRoutine = NULL;

    if(Type_Recipient == ((CLASS_REQUEST & (REQUEST_TYPE | RECIPIENT)) | INTERFACE_RECIPIENT))
    {
        if(RequestNo == GET_DEVICE_ID)
        {
            CopyRoutine = USB_Printer_ID;
        }
        else if (RequestNo == GET_PORT_STATUS)
        {
        }
        else if (RequestNo == SOFT_RESET)
        {
        }
    }

    if (CopyRoutine == NULL)
    {
        return UnSupport;
    }

    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return Success;
}

/**
*\*\name    Printer_SetReport_Feature.
*\*\fun     Set Feature request handling.
*\*\param   Length: data length.
*\*\return  buf pointer
**/
uint8_t* Printer_SetReport_Feature(uint16_t Length)
{
    if (Length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = 2;
        return NULL;
    }
    else
    {
        return Report_Buf;
    }
}

/**
*\*\name    Printer_NoData_Setup.
*\*\fun     handle the no data class specific requests.
*\*\param   RequestNo: request number.
*\*\return  USB_Result
*\*\            - UnSupport
*\*\            - Success
**/
USB_Result Printer_NoData_Setup(uint8_t RequestNo)
{
    if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
        if (RequestNo == SET_COMM_FEATURE)
        {
            return Success;
        }
        else if (RequestNo == SET_CONTROL_LINE_STATE)
        {
            return Success;
        }
    }

  return UnSupport;
}

/**
*\*\name    Printer_GetDeviceDescriptor.
*\*\fun     Gets the device descriptor.
*\*\param   Length: data length.
*\*\return  The address of the device descriptor.
**/
uint8_t* Printer_GetDeviceDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
*\*\name    Printer_GetConfigDescriptor.
*\*\fun     Gets the configuration descriptor.
*\*\param   Length: data length.
*\*\return  The address of the configuration descriptor.
**/
uint8_t* Printer_GetConfigDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
*\*\name    Printer_GetStringDescriptor.
*\*\fun     Gets the string descriptors according to the needed index.
*\*\param   Length: data length.
*\*\return  The address of the string descriptor.
**/
uint8_t* Printer_GetStringDescriptor(uint16_t Length)
{
    uint8_t wValue0 = pInformation->USBwValue0;
    if (wValue0 >= 4)
    {
        return NULL;
    }
    else
    {
        return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
    }
}

/**
*\*\name    Printer_GetDescriptor.
*\*\fun     Gets the descriptor.
*\*\param   Length: data length.
*\*\return  The address of the descriptor.
**/
uint8_t* Printer_GetDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Printer_Descriptor);
}

/**
*\*\name    Printer_Get_Interface_Setting.
*\*\fun     tests the interface and the alternate setting according to the supported one.
*\*\param   Interface: interface number.
*\*\param   AlternateSetting: Alternate Setting number.
*\*\return  USB_Result
*\*\            - Success 
*\*\            - UnSupport.
**/
USB_Result Printer_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
    if (AlternateSetting > 0)
    {
        return UnSupport;
    }
    else if (Interface > 0)
    {
        return UnSupport;
    }
    return Success;
}


/**
*\*\name    Printer_SetProtocol.
*\*\fun     Printer Set Protocol request routine.
*\*\param   none
*\*\return  Success
**/
USB_Result Printer_SetProtocol(void)
{
    uint8_t wValue0 = pInformation->USBwValue0;
    ProtocolValue   = wValue0;
    return Success;
}

/**
*\*\name    Printer_GetProtocolValue.
*\*\fun     get the protocol value.
*\*\param   Length: data length.
*\*\return  address of the protocol value.
**/
uint8_t* Printer_GetProtocolValue(uint16_t Length)
{
    if (Length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = 1;
        return NULL;
    }
    else
    {
        return (uint8_t*)(&ProtocolValue);
    }
}

/**
*\*\name    USB_Printer_ID.
*\*\fun     send the USB Printer ID .
*\*\param   Length: LineCoding length
*\*\return  address of the printer ID string.
**/
uint8_t *USB_Printer_ID(uint16_t Length)
{
    uint32_t wOffset;

    wOffset = pInformation->Ctrl_Info.Usb_wOffset;
    if (Length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_Printer_IDString) - wOffset;
        return NULL;
    }
    return(uint8_t *)&USB_Printer_IDString + wOffset;
}

