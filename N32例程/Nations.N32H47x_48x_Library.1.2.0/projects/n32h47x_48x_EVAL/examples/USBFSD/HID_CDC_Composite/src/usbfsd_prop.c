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

LINE_CODING linecoding =
{
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* no. of bits 8*/
};

uint32_t ProtocolValue;
__IO uint8_t EXTI_Enable;
__IO uint8_t Request = 0;
uint8_t Report_Buf[2];

USB_Device Device_Table = {EP_NUM, 1};

DEVICE_PROP Device_Property = {
    Composite_init,
    Composite_Reset,
    Composite_Status_In,
    Composite_Status_Out,
    Composite_Data_Setup,
    Composite_NoData_Setup,
    Composite_Get_Interface_Setting,
    Composite_GetDeviceDescriptor,
    Composite_GetConfigDescriptor,
    Composite_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
};
USER_STANDARD_REQUESTS User_Standard_Requests = {Composite_GetConfiguration,
                                                 Composite_SetConfiguration,
                                                 Composite_GetInterface,
                                                 Composite_SetInterface,
                                                 Composite_GetStatus,
                                                 Composite_ClearFeature,
                                                 Composite_SetEndPointFeature,
                                                 Composite_SetDeviceFeature,
                                                 Composite_SetDeviceAddress};

USB_OneDescriptor Device_Descriptor = {(uint8_t*)Composite_DeviceDescriptor, COMPOSITE_SIZ_DEVICE_DESC};

USB_OneDescriptor Config_Descriptor = {(uint8_t*)Composite_ConfigDescriptor, COMPOSITE_SIZ_CONFIG_DESC};

USB_OneDescriptor CustomHID_Report_Descriptor = {(uint8_t *)CustomHID_ReportDescriptor, CUSTOMHID_SIZ_REPORT_DESC};

USB_OneDescriptor Composite_Descriptor = {(uint8_t*)Composite_ConfigDescriptor + CUSTOMHID_OFF_HID_DESC, CUSTOMHID_SIZ_HID_DESC};

USB_OneDescriptor String_Descriptor[4] = {{(uint8_t*)Composite_StringLangID, COMPOSITE_SIZ_STRING_LANGID},
                                          {(uint8_t*)Composite_StringVendor, COMPOSITE_SIZ_STRING_VENDOR},
                                          {(uint8_t*)Composite_StringProduct, COMPOSITE_SIZ_STRING_PRODUCT},
                                          {(uint8_t*)Composite_StringSerial, COMPOSITE_SIZ_STRING_SERIAL}};


/*CustomHID_SetReport_Feature function prototypes*/
uint8_t* CustomHID_SetReport_Feature(uint16_t Length);


/**
*\*\name    Composite_init.
*\*\fun     Composite init routine.
*\*\param   none
*\*\return  none 
**/
void Composite_init(void)
{
    /* Update the serial number string descriptor with the data from the unique
    ID*/
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
*\*\name    Composite_Reset.
*\*\fun     Composite reset routine.
*\*\param   none
*\*\return  none 
**/
void Composite_Reset(void)
{
    /* Set Composite_DEVICE as not configured */
    pInformation->CurrentConfiguration = 0;
    pInformation->CurrentInterface     = 0; /*the default Interface*/

    /* Current Feature initialization */
    pInformation->CurrentFeature = Composite_ConfigDescriptor[7];

    USB_SetBuftab(BTABLE_ADDRESS);

    /* Initialize Endpoint 0 */
    USB_SetEpType(ENDP0, EP_CONTROL);
    SetEPTxStatus(ENDP0, EP_TX_STALL);
    USB_SetEpRxAddr(ENDP0, ENDP0_RXADDR);
    USB_SetEpTxAddr(ENDP0, ENDP0_TXADDR);
    USB_ClrStsOut(ENDP0);
    USB_SetEpRxCnt(ENDP0, Device_Property.MaxPacketSize);
    USB_SetEpRxValid(ENDP0);

    /* Initialize Endpoint 1 */
    USB_SetEpType(ENDP1, EP_INTERRUPT);
    USB_SetEpTxAddr(ENDP1, ENDP1_TXADDR);
    USB_SetEpRxAddr(ENDP1, ENDP1_RXADDR);
    USB_SetEpTxCnt(ENDP1, 2);
    USB_SetEpRxCnt(ENDP1, 2);
    SetEPRxStatus(ENDP1, EP_RX_VALID);
    SetEPTxStatus(ENDP1, EP_TX_NAK);
    
    /* Initialize Endpoint 2 */
    USB_SetEpType(ENDP2, EP_INTERRUPT);
    USB_SetEpTxAddr(ENDP2, ENDP2_TXADDR);
    SetEPRxStatus(ENDP2, EP_RX_DIS);
    SetEPTxStatus(ENDP2, EP_TX_NAK);

    /* Initialize Endpoint 3 */
    USB_SetEpType(ENDP3, EP_BULK);
    USB_SetEpTxAddr(ENDP3, ENDP3_TXADDR);
    USB_SetEpRxAddr(ENDP3, ENDP3_RXADDR);
    USB_SetEpTxCnt(ENDP3, VIRTUAL_COM_PORT_DATA_SIZE);
    USB_SetEpRxCnt(ENDP3, VIRTUAL_COM_PORT_DATA_SIZE);
    SetEPRxStatus(ENDP3, EP_RX_VALID);
    SetEPTxStatus(ENDP3, EP_TX_NAK);

    /* Set this device to response on default address */
    USB_SetDeviceAddress(0);
    bDeviceState = ATTACHED;
}

/**
*\*\name    Composite_SetConfiguration.
*\*\fun     Update the device state to configured.
*\*\param   none
*\*\return  none 
**/
void Composite_SetConfiguration(void)
{
    if (pInformation->CurrentConfiguration != 0)
    {
        /* Device configured */
        bDeviceState = CONFIGURED;
    }
}

/**
*\*\name    Composite_SetDeviceAddress.
*\*\fun     Update the device state to addressed.
*\*\param   none
*\*\return  none 
**/
void Composite_SetDeviceAddress(void)
{
    bDeviceState = ADDRESSED;
}

/**
*\*\name    Composite_Status_In.
*\*\fun     Composite status IN routine.
*\*\param   none
*\*\return  none 
**/
void Composite_Status_In(void)
{
    if (Request == SET_LINE_CODING)
    {
        USART_Config();
        Request = 0;
    }
}

/**
*\*\name    Composite_Status_Out.
*\*\fun     Composite status OUT routine.
*\*\param   none
*\*\return  none 
**/
void Composite_Status_Out(void)
{
}

/**
*\*\name    Composite_Data_Setup.
*\*\fun     Handle the data class specific requests.
*\*\param   RequestNo
*\*\            - GET_DESCRIPTOR
*\*\            - GET_PROTOCOL
*\*\            - SET_REPORT
*\*\return  USB_Result
*\*\            - Success
*\*\            - UnSupport
**/
USB_Result Composite_Data_Setup(uint8_t RequestNo)
{
    uint8_t* (*CopyRoutine)(uint16_t);

    CopyRoutine = NULL;

    if (RequestNo == GET_LINE_CODING)
    {
        if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
        {
            CopyRoutine = Virtual_Com_Port_GetLineCoding;
        }
    }
    else if (RequestNo == SET_LINE_CODING)
    {
        if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
        {
            CopyRoutine = Virtual_Com_Port_SetLineCoding;
        }
        Request = SET_LINE_CODING;
    }
    else if ((RequestNo == GET_DESCRIPTOR) && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)))
    {
        if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
        {
            CopyRoutine = CustomHID_GetReportDescriptor;
        }
        else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
        {
            CopyRoutine = Composite_GetDescriptor;
        }

    }
    /*** GET_PROTOCOL, GET_REPORT, SET_REPORT ***/
    else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        switch (RequestNo)
        {
            case GET_PROTOCOL:
                CopyRoutine = CustomHID_GetProtocolValue;
                break;
            case SET_REPORT:
                CopyRoutine = CustomHID_SetReport_Feature;
                Request     = SET_REPORT;
                break;
            default:
                break;
        }
    }

    if (CopyRoutine == NULL)
    {
        return UnSupport;
    }

    pInformation->Ctrl_Info.CopyData    = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return Success;
}

/**
*\*\name    CustomHID_SetReport_Feature.
*\*\fun     Set Feature request handling.
*\*\param   Length: data length.
*\*\return  Report_Buf or NULL
**/
uint8_t* CustomHID_SetReport_Feature(uint16_t Length)
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
*\*\name    Composite_NoData_Setup.
*\*\fun     handle the no data class specific requests.
*\*\param   RequestNo: request number.
*\*\return  USB_Result
*\*\            - UnSupport
*\*\            - Success
**/
USB_Result Composite_NoData_Setup(uint8_t RequestNo)
{
    if(Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
    {
        if (RequestNo == SET_COMM_FEATURE)
        {
            return Success;
        }
        else if(RequestNo == SET_CONTROL_LINE_STATE)
        {
            return Success;
        }
        else if(RequestNo == SET_PROTOCOL)
        {
            return CustomHID_SetProtocol();
        }
        else
        {
            return UnSupport;
        }
    }
    else
    {
        return UnSupport;
    }
}

/**
*\*\name    Composite_GetDeviceDescriptor.
*\*\fun     Gets the device descriptor.
*\*\param   Length: data length.
*\*\return  The address of the device descriptor.
**/
uint8_t* Composite_GetDeviceDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
*\*\name    Composite_GetConfigDescriptor.
*\*\fun     Gets the configuration descriptor.
*\*\param   Length: data length.
*\*\return  The address of the configuration descriptor.
**/
uint8_t* Composite_GetConfigDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
*\*\name    Composite_GetStringDescriptor.
*\*\fun     Gets the string descriptors according to the needed index.
*\*\param   Length: data length.
*\*\return  The address of the string descriptor.
**/
uint8_t* Composite_GetStringDescriptor(uint16_t Length)
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
*\*\name    CustomHID_GetReportDescriptor.
*\*\fun     Gets the HID report descriptor.
*\*\param   Length: data length.
*\*\return  The address of the report descriptor.
**/
uint8_t* CustomHID_GetReportDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &CustomHID_Report_Descriptor);
}

/**
*\*\name    Composite_GetHIDDescriptor.
*\*\fun     Gets the HID descriptor.
*\*\param   Length: data length.
*\*\return  The address of the descriptor descriptor.
**/
uint8_t* Composite_GetDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Composite_Descriptor);
}

/**
*\*\name    Composite_Get_Interface_Setting.
*\*\fun     tests the interface and the alternate setting according to the supported one.
*\*\param   Interface: interface number.
*\*\param   AlternateSetting: Alternate Setting number.
*\*\return  USB_Result
*\*\            - Success 
*\*\            - UnSupport.
**/
USB_Result Composite_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
    if(Interface == 0)
    {
        if (AlternateSetting > 0)
        {
            return UnSupport;
        }
    }
    else
    {
        if (AlternateSetting > 0)
        {
            return UnSupport;
        }
        else if (Interface > 1)
        {
            return UnSupport;
        }
    }
    return Success;
}

/**
*\*\name    CustomHID_SetProtocol.
*\*\fun     Set Protocol request routine.
*\*\param   none
*\*\return  USB_Result
*\*\            - Success 
*\*\            - UnSupport.
**/
USB_Result CustomHID_SetProtocol(void)
{
    uint8_t wValue0 = pInformation->USBwValue0;
    ProtocolValue   = wValue0;
    return Success;
}

/**
*\*\name    CustomHID_GetProtocolValue.
*\*\fun     get the protocol value.
*\*\param   Length: data length.
*\*\return  address of the protocol value.
**/
uint8_t* CustomHID_GetProtocolValue(uint16_t Length)
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
*\*\name    Virtual_Com_Port_GetLineCoding.
*\*\fun     Send the linecoding structure to the PC host.
*\*\param   Length: length.
*\*\param   AlternateSetting: Alternate Setting number.
*\*\return  Linecoding structure base address.
**/
uint8_t *Virtual_Com_Port_GetLineCoding(uint16_t Length)
{
    if (Length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
        return NULL;
    }
    return(uint8_t *)&linecoding;
}

/**
*\*\name    Virtual_Com_Port_SetLineCoding.
*\*\fun     Set the linecoding structure fields.
*\*\param   Length: length.
*\*\param   AlternateSetting: Alternate Setting number.
*\*\return  Linecoding structure base address.
**/
uint8_t *Virtual_Com_Port_SetLineCoding(uint16_t Length)
{
    if (Length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
        return NULL;
    }
    return(uint8_t *)&linecoding;
}
