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
#include "usbfsd_bot.h"
#include "hw_config.h"
#include "memory.h"
#include "mass_mal.h"
#include "usbfsd_prop.h"

extern uint32_t Max_Lun;

USB_Device Device_Table =
{
    EP_NUM,
    1
};

DEVICE_PROP Device_Property =
{
    MASS_init,
    MASS_Reset,
    MASS_Status_In,
    MASS_Status_Out,
    MASS_Data_Setup,
    MASS_NoData_Setup,
    MASS_Get_Interface_Setting,
    MASS_GetDeviceDescriptor,
    MASS_GetConfigDescriptor,
    MASS_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
    Mass_Storage_GetConfiguration,
    Mass_Storage_SetConfiguration,
    Mass_Storage_GetInterface,
    Mass_Storage_SetInterface,
    Mass_Storage_GetStatus,
    Mass_Storage_ClearFeature,
    Mass_Storage_SetEndPointFeature,
    Mass_Storage_SetDeviceFeature,
    Mass_Storage_SetDeviceAddress
};

USB_OneDescriptor Device_Descriptor =
{
    (uint8_t*)MASS_DeviceDescriptor,
    MASS_SIZ_DEVICE_DESC
};

USB_OneDescriptor Config_Descriptor =
{
    (uint8_t*)MASS_ConfigDescriptor,
    MASS_SIZ_CONFIG_DESC
};

USB_OneDescriptor String_Descriptor[5] =
{
    {(uint8_t*)MASS_StringLangID, MASS_SIZ_STRING_LANGID},
    {(uint8_t*)MASS_StringVendor, MASS_SIZ_STRING_VENDOR},
    {(uint8_t*)MASS_StringProduct, MASS_SIZ_STRING_PRODUCT},
    {(uint8_t*)MASS_StringSerial, MASS_SIZ_STRING_SERIAL},
    {(uint8_t*)MASS_StringInterface, MASS_SIZ_STRING_INTERFACE},
};

/* Extern variables ----------------------------------------------------------*/
extern unsigned char Bot_State;
extern Bulk_Only_CBW CBW;

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
*\*\name    MASS_init.
*\*\fun     Mass Storage init routine.
*\*\param   none
*\*\return  none 
**/
void MASS_init()
{
    pInformation->CurrentConfiguration = 0;

    /* Connect the device */
    PowerOn();

    /* USB interrupts initialization */
    /* clear pending interrupts */
    _SetISTR(0);
    wInterrupt_Mask = IMR_MSK;
    /* set interrupts mask */
    _SetCNTR(wInterrupt_Mask);
    
    /*Pull up DP*/
    _EnPortPullup();

    bDeviceState = UNCONNECTED;
}

/**
*\*\name    MASS_Reset.
*\*\fun     Mass Storage reset routine.
*\*\param   none
*\*\return  none 
**/
void MASS_Reset()
{
    /* Set the device as not configured */
    Device_Info.CurrentConfiguration = 0;

    /* Current Feature initialization */
    pInformation->CurrentFeature = MASS_ConfigDescriptor[7];

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
    USB_SetEpType(ENDP1, EP_BULK);
    USB_SetEpTxAddr(ENDP1, ENDP1_TXADDR);
    SetEPTxStatus(ENDP1, EP_TX_NAK);
    SetEPRxStatus(ENDP1, EP_RX_DIS);

    /* Initialize Endpoint 2 */
    USB_SetEpType(ENDP2, EP_BULK);
    USB_SetEpRxAddr(ENDP2, ENDP2_RXADDR);
    USB_SetEpRxCnt(ENDP2, Device_Property.MaxPacketSize);
    SetEPRxStatus(ENDP2, EP_RX_VALID);
    SetEPTxStatus(ENDP2, EP_TX_DIS);

    /* Set the device to response on default address */
    USB_SetDeviceAddress(0);

    bDeviceState = ATTACHED;

    CBW.dSignature = BOT_CBW_SIGNATURE;
    Bot_State = BOT_IDLE;
}

/**
*\*\name    Mass_Storage_SetConfiguration.
*\*\fun     Handle the SetConfiguration request.
*\*\param   none
*\*\return  none 
**/
void Mass_Storage_SetConfiguration(void)
{
    if (pInformation->CurrentConfiguration != 0)
    {
        /* Device configured */
        bDeviceState = CONFIGURED;

        USB_ClrDattogTx(ENDP1);
        USB_ClrDattogRx(ENDP2);
        Bot_State = BOT_IDLE; /* set the Bot state machine to the IDLE state */
    }
}

/**
*\*\name    Mass_Storage_ClearFeature.
*\*\fun     Handle the ClearFeature request.
*\*\param   none
*\*\return  none 
**/
void Mass_Storage_ClearFeature(void)
{
    /* when the host send a CBW with invalid signature or invalid length the two
       Endpoints (IN & OUT) shall stall until receiving a Mass Storage Reset     */
    if (CBW.dSignature != BOT_CBW_SIGNATURE)
        Bot_Abort(BOTH_DIR);
}

/**
*\*\name    Mass_Storage_SetDeviceAddres.
*\*\fun     Update the device state to addressed.
*\*\param   none
*\*\return  none 
**/
void Mass_Storage_SetDeviceAddress (void)
{
    bDeviceState = ADDRESSED;
}

/**
*\*\name    MASS_Status_In.
*\*\fun     Mass Storage Status IN routine.
*\*\param   none
*\*\return  none 
**/
void MASS_Status_In(void)
{
    return;
}

/**
*\*\name    MASS_Status_Out.
*\*\fun     Mass Storage Status OUT routine.
*\*\param   none
*\*\return  none 
**/
void MASS_Status_Out(void)
{
    return;
}

/**
*\*\name    MASS_Data_Setup.
*\*\fun     Handle the data class specific requests.
*\*\param   RequestNo
*\*\            - GET_DESCRIPTOR
*\*\            - GET_PROTOCOL
*\*\            - SET_REPORT
*\*\return  USB_Result
*\*\            - Success
*\*\            - UnSupport
**/
USB_Result MASS_Data_Setup(uint8_t RequestNo)
{
    uint8_t    *(*CopyRoutine)(uint16_t);

    CopyRoutine = NULL;
    if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
            && (RequestNo == GET_MAX_LUN) && (pInformation->USBwValue == 0)
            && (pInformation->USBwIndex == 0) && (pInformation->USBwLength == 0x01))
    {
        CopyRoutine = Get_Max_Lun;
    }
    else
    {
        return UnSupport;
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
*\*\name    MASS_NoData_Setup.
*\*\fun     handle the no data class specific requests.
*\*\param   RequestNo: request number.
*\*\return  USB_Result
*\*\            - UnSupport
*\*\            - Success
**/
USB_Result MASS_NoData_Setup(uint8_t RequestNo)
{
    if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
            && (RequestNo == MASS_STORAGE_RESET) && (pInformation->USBwValue == 0)
            && (pInformation->USBwIndex == 0) && (pInformation->USBwLength == 0x00))
    {
        /* Initialize Endpoint 1 */
        USB_ClrDattogTx(ENDP1);

        /* Initialize Endpoint 2 */
        USB_ClrDattogRx(ENDP2);

        /*intialise the CBW signature to enable the clear feature*/
        CBW.dSignature = BOT_CBW_SIGNATURE;
        Bot_State = BOT_IDLE;

        return Success;
    }
    return UnSupport;
}

/**
 * @brief  Test the interface and the alternate setting according to the supported one.
 * @param  Interface: interface.
 * @param  AlternateSetting: alternate Setting.
 * @return UnSupport or Success.
 */
/**
*\*\name    MASS_Get_Interface_Setting.
*\*\fun     Test the interface and the alternate setting according to the supported one.
*\*\param   Interface: interface.
*\*\param   AlternateSetting: alternate Setting.
*\*\return  USB_Result
*\*\            - UnSupport
*\*\            - Success
**/
USB_Result MASS_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
    if (AlternateSetting > 0)
    {
        return UnSupport;/* in this application we don't have AlternateSetting*/
    }
    else if (Interface > 0)
    {
        return UnSupport;/*in this application we have only 1 interfaces*/
    }
    return Success;
}

/**
*\*\name    MASS_GetDeviceDescriptor.
*\*\fun     Gets the device descriptor.
*\*\param   Length: data length.
*\*\return  The address of the device descriptor.
**/
uint8_t *MASS_GetDeviceDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Device_Descriptor );
}

/**
*\*\name    MASS_GetConfigDescriptor.
*\*\fun     Gets the configuration descriptor.
*\*\param   Length: data length.
*\*\return  The address of the configuration descriptor.
**/
uint8_t *MASS_GetConfigDescriptor(uint16_t Length)
{
    return Standard_GetDescriptorData(Length, &Config_Descriptor );
}

/**
*\*\name    MASS_GetStringDescriptor.
*\*\fun     Gets the string descriptors according to the needed index.
*\*\param   Length: data length.
*\*\return  The address of the string descriptor.
**/
uint8_t *MASS_GetStringDescriptor(uint16_t Length)
{
    uint8_t wValue0 = pInformation->USBwValue0;

    if (wValue0 > 4)
    {
        return NULL;
    }
    else
    {
        return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
    }
}

/**
*\*\name    Get_Max_Lun.
*\*\fun     Handle the Get Max Lun request.
*\*\param   Length: length.
*\*\return  max lun.
**/
uint8_t *Get_Max_Lun(uint16_t Length)
{
    if (Length == 0)
    {
        pInformation->Ctrl_Info.Usb_wLength = LUN_DATA_LENGTH;
        return 0;
    }
    else
    {
        return((uint8_t*)(&Max_Lun));
    }
}

