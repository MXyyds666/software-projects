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
*\*\file usbfsd_scsi.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usbfsd_scsi.h"
#include "mass_mal.h"
#include "usbfsd_bot.h"
#include "usbfsd_regs.h"
#include "memory.h"
#include "usbfsd_lib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern uint8_t Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
extern uint8_t Bot_State;
extern Bulk_Only_CBW CBW;
extern Bulk_Only_CSW CSW;
extern uint32_t Mass_Memory_Size[2];
extern uint32_t Mass_Block_Size[2];
extern uint32_t Mass_Block_Count[2];

/**
*\*\name    SCSI_Inquiry_Cmd.
*\*\fun     SCSI Inquiry Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_Inquiry_Cmd(uint8_t lun)
{
    uint8_t* Inquiry_Data;
    uint16_t Inquiry_Data_Length;

    if (CBW.CB[1] & 0x01)/*Evpd is set*/
    {
        Inquiry_Data = Page00_Inquiry_Data;
        Inquiry_Data_Length = 5;
    }
    else
    {
        if ( lun == 0)
        {
            Inquiry_Data = Standard_Inquiry_Data;
        }
        else
        {
            Inquiry_Data = Standard_Inquiry_Data1;
        }
        if (CBW.CB[4] <= STANDARD_INQUIRY_DATA_LEN)
            Inquiry_Data_Length = CBW.CB[4];
        else
            Inquiry_Data_Length = STANDARD_INQUIRY_DATA_LEN;
    }
    Transfer_Data_Request(Inquiry_Data, Inquiry_Data_Length);
}

/**
*\*\name    SCSI_ReadFormatCapacity_Cmd.
*\*\fun     SCSI ReadFormatCapacity Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_ReadFormatCapacity_Cmd(uint8_t lun)
{
    if (MAL_GetStatus(lun) != 0 )
    {
        Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
        Bot_Abort(DIR_IN);
        return;
    }
    ReadFormatCapacity_Data[4] = (uint8_t)(Mass_Block_Count[lun] >> 24);
    ReadFormatCapacity_Data[5] = (uint8_t)(Mass_Block_Count[lun] >> 16);
    ReadFormatCapacity_Data[6] = (uint8_t)(Mass_Block_Count[lun] >>  8);
    ReadFormatCapacity_Data[7] = (uint8_t)(Mass_Block_Count[lun]);

    ReadFormatCapacity_Data[9] = (uint8_t)(Mass_Block_Size[lun] >>  16);
    ReadFormatCapacity_Data[10] = (uint8_t)(Mass_Block_Size[lun] >>  8);
    ReadFormatCapacity_Data[11] = (uint8_t)(Mass_Block_Size[lun]);
    Transfer_Data_Request(ReadFormatCapacity_Data, READ_FORMAT_CAPACITY_DATA_LEN);
}

/**
*\*\name    SCSI_ReadCapacity10_Cmd.
*\*\fun     SCSI ReadCapacity10 Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_ReadCapacity10_Cmd(uint8_t lun)
{
    if (MAL_GetStatus(lun))
    {
        Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
        Bot_Abort(DIR_IN);
        return;
    }

    ReadCapacity10_Data[0] = (uint8_t)((Mass_Block_Count[lun] - 1) >> 24);
    ReadCapacity10_Data[1] = (uint8_t)((Mass_Block_Count[lun] - 1) >> 16);
    ReadCapacity10_Data[2] = (uint8_t)((Mass_Block_Count[lun] - 1) >>  8);
    ReadCapacity10_Data[3] = (uint8_t)(Mass_Block_Count[lun] - 1);

    ReadCapacity10_Data[4] = (uint8_t)(Mass_Block_Size[lun] >>  24);
    ReadCapacity10_Data[5] = (uint8_t)(Mass_Block_Size[lun] >>  16);
    ReadCapacity10_Data[6] = (uint8_t)(Mass_Block_Size[lun] >>  8);
    ReadCapacity10_Data[7] = (uint8_t)(Mass_Block_Size[lun]);
    Transfer_Data_Request(ReadCapacity10_Data, READ_CAPACITY10_DATA_LEN);
}

/**
*\*\name    SCSI_ModeSense6_Cmd.
*\*\fun     SCSI ModeSense6 Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_ModeSense6_Cmd (uint8_t lun)
{
    Transfer_Data_Request(Mode_Sense6_data, MODE_SENSE6_DATA_LEN);
}

/**
*\*\name    SCSI_ModeSense10_Cmd.
*\*\fun     SCSI ModeSense10 Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_ModeSense10_Cmd (uint8_t lun)
{
    Transfer_Data_Request(Mode_Sense10_data, MODE_SENSE10_DATA_LEN);
}

/**
*\*\name    SCSI_RequestSense_Cmd.
*\*\fun     SCSI RequestSense Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_RequestSense_Cmd (uint8_t lun)
{
    uint8_t Request_Sense_data_Length;

    if (CBW.CB[4] <= REQUEST_SENSE_DATA_LEN)
    {
        Request_Sense_data_Length = CBW.CB[4];
    }
    else
    {
        Request_Sense_data_Length = REQUEST_SENSE_DATA_LEN;
    }
    Transfer_Data_Request(Scsi_Sense_Data, Request_Sense_data_Length);
}

/**
*\*\name    Set_Scsi_Sense_Data.
*\*\fun     Set Scsi Sense Data routine.
*\*\param   lun: logical unit.
*\*\param   Sens_Key: sens key.
*\*\param   Asc: asc.
*\*\return  none 
**/
void Set_Scsi_Sense_Data(uint8_t lun, uint8_t Sens_Key, uint8_t Asc)
{
    Scsi_Sense_Data[2] = Sens_Key;
    Scsi_Sense_Data[12] = Asc;
}

/**
*\*\name    SCSI_Start_Stop_Unit_Cmd.
*\*\fun     SCSI Start_Stop_Unit Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_Start_Stop_Unit_Cmd(uint8_t lun)
{
    Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
}

/**
*\*\name    SCSI_Read10_Cmd.
*\*\fun     SCSI Read10 Command routine.
*\*\param   lun: logical unit.
*\*\param   LBA: memory offset.
*\*\param   BlockNbr: block number.
*\*\return  none 
**/
void SCSI_Read10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr)
{
    if (Bot_State == BOT_IDLE)
    {
        if (!(SCSI_Address_Management(CBW.bLUN, SCSI_READ10, LBA, BlockNbr)))/*address out of range*/
        {
            return;
        }

        if ((CBW.bmFlags & 0x80) != 0)
        {
            Bot_State = BOT_DATA_IN;
            Read_Memory(lun, LBA , BlockNbr);
        }
        else
        {
            Bot_Abort(BOTH_DIR);
            Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
            Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
        }
        return;
    }
    else if (Bot_State == BOT_DATA_IN)
    {
        Read_Memory(lun , LBA , BlockNbr);
    }
}

/**
*\*\name    SCSI_Write10_Cmd.
*\*\fun     SCSI Write10 Command routine.
*\*\param   lun: logical unit.
*\*\param   LBA: memory offset.
*\*\param   BlockNbr: block number.
*\*\return  none 
**/
void SCSI_Write10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr)
{
    if (Bot_State == BOT_IDLE)
    {
        if (!(SCSI_Address_Management(CBW.bLUN, SCSI_WRITE10 , LBA, BlockNbr)))/*address out of range*/
        {
            return;
        }

        if ((CBW.bmFlags & 0x80) == 0)
        {
            Bot_State = BOT_DATA_OUT;
            SetEPRxStatus(ENDP1, EP_RX_VALID);
        }
        else
        {
            Bot_Abort(DIR_IN);
            Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
            Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        }
        return;
    }
    else if (Bot_State == BOT_DATA_OUT)
    {
        Write_Memory(lun , LBA , BlockNbr);
    }
}

/**
*\*\name    SCSI_Verify10_Cmd.
*\*\fun     SCSI Verify10 Command routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_Verify10_Cmd(uint8_t lun)
{
    if ((CBW.dDataLength == 0) && !(CBW.CB[1] & BLKVFY))/* BLKVFY not set*/
    {
        Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
    }
    else
    {
        Bot_Abort(BOTH_DIR);
        Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
}

/**
*\*\name    SCSI_Valid_Cmd.
*\*\fun     Valid Commands routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_Valid_Cmd(uint8_t lun)
{
    if (CBW.dDataLength != 0)
    {
        Bot_Abort(BOTH_DIR);
        Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
    else
        Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
}

/**
*\*\name    SCSI_TestUnitReady_Cmd.
*\*\fun     Valid Commands routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_TestUnitReady_Cmd(uint8_t lun)
{
    if (MAL_GetStatus(lun))
    {
        Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
        Bot_Abort(DIR_IN);
        return;
    }
    else
    {
        Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
    }
}

/**
*\*\name    SCSI_Format_Cmd.
*\*\fun     Format Commands routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_Format_Cmd(uint8_t lun)
{
    if (MAL_GetStatus(lun))
    {
        Set_Scsi_Sense_Data(CBW.bLUN, NOT_READY, MEDIUM_NOT_PRESENT);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_ENABLE);
        Bot_Abort(DIR_IN);
        return;
    }
}

/**
*\*\name    SCSI_Invalid_Cmd.
*\*\fun     Invalid Commands routine.
*\*\param   lun: logical unit.
*\*\return  none 
**/
void SCSI_Invalid_Cmd(uint8_t lun)
{
    if (CBW.dDataLength == 0)
    {
        Bot_Abort(DIR_IN);
    }
    else
    {
        if ((CBW.bmFlags & 0x80) != 0)
        {
            Bot_Abort(DIR_IN);
        }
        else
        {
            Bot_Abort(BOTH_DIR);
        }
    }
    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
}

/**
*\*\name    SCSI_Address_Management.
*\*\fun     Test the received address.
*\*\param   lun: logical unit.
*\*\param   Cmd: the command can be SCSI_READ10 or SCSI_WRITE10.
*\*\param   LBA: memory offset.
*\*\param   BlockNbr: block number.
*\*\return  true or false. 
**/
bool SCSI_Address_Management(uint8_t lun , uint8_t Cmd , uint32_t LBA , uint32_t BlockNbr)
{
    if ((LBA + BlockNbr) > Mass_Block_Count[lun] )
    {
        if (Cmd == SCSI_WRITE10)
        {
            Bot_Abort(BOTH_DIR);
        }
        Bot_Abort(DIR_IN);
        Set_Scsi_Sense_Data(lun, ILLEGAL_REQUEST, ADDRESS_OUT_OF_RANGE);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        return (false);
    }

    if (CBW.dDataLength != BlockNbr * Mass_Block_Size[lun])
    {
        if (Cmd == SCSI_WRITE10)
        {
            Bot_Abort(BOTH_DIR);
        }
        else
        {
            Bot_Abort(DIR_IN);
        }
        Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        return (false);
    }
    return (true);
}
