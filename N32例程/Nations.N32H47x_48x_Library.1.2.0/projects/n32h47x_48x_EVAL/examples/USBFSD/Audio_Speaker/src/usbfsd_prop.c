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
/* Includes ------------------------------------------------------------------*/
#include "usbfsd_lib.h"
#include "usbfsd_desc.h"
#include "usbfsd_pwr.h"
#include "hw_config.h"
#include "usbfsd_prop.h"

uint32_t MUTE_DATA = 0;

USB_Device Device_Table =
{
    EP_NUM,
    1
};

DEVICE_PROP Device_Property =
{
    Speaker_init,
    Speaker_Reset,
    Speaker_Status_In,
    Speaker_Status_Out,
    Speaker_Data_Setup,
    Speaker_NoData_Setup,
    Speaker_Get_Interface_Setting,
    Speaker_GetDeviceDescriptor,
    Speaker_GetConfigDescriptor,
    Speaker_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
    Speaker_GetConfiguration,
    Speaker_SetConfiguration,
    Speaker_GetInterface,
    Speaker_SetInterface,
    Speaker_GetStatus,
    Speaker_ClearFeature,
    Speaker_SetEndPointFeature,
    Speaker_SetDeviceFeature,
    Speaker_SetDeviceAddress
};

USB_OneDescriptor Device_Descriptor = {(uint8_t*)Speaker_DeviceDescriptor, SPEAKER_SIZ_DEVICE_DESC};

USB_OneDescriptor Config_Descriptor = {(uint8_t*)Speaker_ConfigDescriptor, SPEAKER_SIZ_CONFIG_DESC};

USB_OneDescriptor String_Descriptor[4] = {{(uint8_t*)Speaker_StringLangID, SPEAKER_SIZ_STRING_LANGID},
                                           {(uint8_t*)Speaker_StringVendor, SPEAKER_SIZ_STRING_VENDOR},
                                           {(uint8_t*)Speaker_StringProduct, SPEAKER_SIZ_STRING_PRODUCT},
                                           {(uint8_t*)Speaker_StringSerial, SPEAKER_SIZ_STRING_SERIAL},};

/**
*\*\name    Speaker_init.
*\*\fun     Speaker init routine.
*\*\param   none
*\*\return  none 
**/
void Speaker_init()
{
    /* Update the serial number string descriptor with the data from the unique ID*/
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
*\*\name    Speaker_Reset.
*\*\fun     Speaker reset routine.
*\*\param   none
*\*\return  none 
**/
void Speaker_Reset()
{
    /* Set Speaker device as not configured state */
    pInformation->CurrentConfiguration = 0;

    /* Current Feature initialization */
    pInformation->CurrentFeature = Speaker_ConfigDescriptor[7];

    USB_SetBuftab(BTABLE_ADDRESS);

    /* Initialize Endpoint 0 */
    USB_SetEpType(ENDP0, EP_CONTROL);
    SetEPTxStatus(ENDP0, EP_TX_NAK);
    USB_SetEpRxAddr(ENDP0, ENDP0_RXADDR);
    USB_SetEpRxCnt(ENDP0, Device_Property.MaxPacketSize);
    USB_SetEpTxAddr(ENDP0, ENDP0_TXADDR);
    USB_ClrStsOut(ENDP0);
    USB_SetEpRxValid(ENDP0);

    /* Initialize Endpoint 1 */
    USB_SetEpType(ENDP1, EP_ISOCHRONOUS);
    USB_SetEpDblBuferAddr(ENDP1, ENDP1_BUF0Addr, ENDP1_BUF1Addr);
    USB_SetEpDblBuferCnt(ENDP1, EP_DBUF_OUT, 0x40);
    USB_ClrDattogRx(ENDP1);
    USB_ClrDattogTx(ENDP1);
    USB_DattogTx(ENDP1);
    SetEPRxStatus(ENDP1, EP_RX_VALID);
    SetEPTxStatus(ENDP1, EP_TX_DIS);

    USB_SetEpRxValid(ENDP0);
    /* Set this device to response on default address */
    USB_SetDeviceAddress(0);

    bDeviceState = ATTACHED;

}

/**
*\*\name    Speaker_SetConfiguration.
*\*\fun     Handle the SetConfiguration request.
*\*\param   none
*\*\return  none 
**/
void Speaker_SetConfiguration(void)
{
    if (pInformation->CurrentConfiguration != 0)
    {
        /* Device configured */
        bDeviceState = CONFIGURED;
    }
}

/**
*\*\name    Speaker_SetDeviceAddres.
*\*\fun     Update the device state to addressed.
*\*\param   none
*\*\return  none 
**/
void Speaker_SetDeviceAddress (void)
{
    bDeviceState = ADDRESSED;
}

/**
*\*\name    Speaker_Status_In.
*\*\fun     Speaker Status IN routine.
*\*\param   none
*\*\return  none 
**/
void Speaker_Status_In(void)
{
    return;
}

/**
*\*\name    Speaker_Status_Out.
*\*\fun     Speaker Status OUT routine.
*\*\param   none
*\*\return  none 
**/
void Speaker_Status_Out(void)
{
    return;
}

/**
*\*\name    Speaker_Data_Setup.
*\*\fun     Handle the data class specific requests.
*\*\param   RequestNo
*\*\            - GET_DESCRIPTOR
*\*\            - GET_PROTOCOL
*\*\            - SET_REPORT
*\*\return  USB_Result
*\*\            - Success
*\*\            - UnSupport
**/
USB_Result Speaker_Data_Setup(uint8_t RequestNo)
{
    uint8_t *(*CopyRoutine)(uint16_t);
    CopyRoutine = NULL;

    if ((RequestNo == GET_CUR) || (RequestNo == SET_CUR))
    {
        CopyRoutine = Mute_Command;
    }

    else
    {
        return UnSupport;
    }

    pInformation->Ctrl_Info.CopyData = CopyRoutine;
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    (*CopyRoutine)(0);
    return Success;

}

/**
*\*\name    Speaker_NoData_Setup.
*\*\fun     handle the no data class specific requests.
*\*\param   RequestNo: request number.
*\*\return  USB_Result
*\*\            - UnSupport
*\*\            - Success
**/
USB_Result Speaker_NoData_Setup(uint8_t RequestNo)
{
    return UnSupport;
}

/**
 * @brief  Test the interface and the alternate setting according to the supported one.
 * @param  Interface: interface.
 * @param  AlternateSetting: alternate Setting.
 * @return UnSupport or Success.
 */
/**
*\*\name    Speaker_Get_Interface_Setting.
*\*\fun     Test the interface and the alternate setting according to the supported one.
*\*\param   Interface: interface.
*\*\param   AlternateSetting: alternate Setting.
*\*\return  USB_Result
*\*\            - UnSupport
*\*\            - Success
**/
USB_Result Speaker_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
    if (AlternateSetting > 1)
    {
        return UnSupport;/* in this application we don't have AlternateSetting*/
    }
    else if (Interface > 1)
    {
        return UnSupport;/*in this application we have only 1 interfaces*/
    }
    return Success;
}

/**
*\*\name    Speaker_GetDeviceDescriptor.
*\*\fun     Gets the device descriptor.
*\*\param   Length: data length.
*\*\return  The address of the device descriptor.
**/
uint8_t *Speaker_GetDeviceDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
*\*\name    Speaker_GetConfigDescriptor.
*\*\fun     Gets the configuration descriptor.
*\*\param   Length: data length.
*\*\return  The address of the configuration descriptor.
**/
uint8_t *Speaker_GetConfigDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
*\*\name    Speaker_GetStringDescriptor.
*\*\fun     Gets the string descriptors according to the needed index.
*\*\param   Length: data length.
*\*\return  The address of the string descriptor.
**/
uint8_t *Speaker_GetStringDescriptor(uint16_t Length)
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
*\*\name    Mute_Command.
*\*\fun     Handle the GET MUTE and SET MUTE command.
*\*\param   Length: data length.
*\*\return  MUTE data.
**/
uint8_t *Mute_Command(uint16_t Length)
{
    if (Length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = pInformation->wLengths.w;
        return NULL;
    }
    else
    {
        return((uint8_t*)(&MUTE_DATA));
    }
}

