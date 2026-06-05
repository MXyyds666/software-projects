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
*\*\file usbfsd_bot.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

/* Includes ------------------------------------------------------------------*/
#include "usbfsd_scsi.h"
#include "hw_config.h"
#include "usbfsd_regs.h"
#include "usbfsd_mem.h"
#include "usbfsd_conf.h"
#include "usbfsd_bot.h"
#include "memory.h"
#include "usbfsd_lib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Bot_State;
uint8_t Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
uint16_t Data_Len;
Bulk_Only_CBW CBW;
Bulk_Only_CSW CSW;
uint32_t SCSI_LBA , SCSI_BlkLen;
extern uint32_t Max_Lun;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
*\*\name    Mass_Storage_In.
*\*\fun     Mass Storage IN transfer.
*\*\param   none
*\*\return  none 
**/
void Mass_Storage_In (void)
{
    switch (Bot_State)
    {
        case BOT_CSW_Send:
        case BOT_ERROR:
            Bot_State = BOT_IDLE;
            SetEPRxStatus(ENDP1, EP_RX_VALID);/* enable the Endpoint to receive the next cmd*/
            if (USB_GetEpRxSts(EP1_OUT) == EP_RX_STALL)
            {
                SetEPRxStatus(EP1_OUT, EP_RX_VALID);/* enable the Endpoint to receive the next cmd*/
            }
            break;
        case BOT_DATA_IN:
            switch (CBW.CB[0])
            {
                case SCSI_READ10:
                    SCSI_Read10_Cmd(CBW.bLUN , SCSI_LBA , SCSI_BlkLen);
                    break;
            }
            break;
        case BOT_DATA_IN_LAST:
            Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
            SetEPRxStatus(ENDP1, EP_RX_VALID);
            break;
        default:
          break;
    }
}

/**
*\*\name    Mass_Storage_Out.
*\*\fun     Mass Storage OUT transfer.
*\*\param   none
*\*\return  none 
**/
void Mass_Storage_Out (void)
{
    uint8_t CMD;
    CMD = CBW.CB[0];

    Data_Len = USB_SilRead(EP1_OUT, Bulk_Data_Buff);

    switch (Bot_State)
    {
        case BOT_IDLE:
            CBW_Decode();
            break;
        case BOT_DATA_OUT:
            if (CMD == SCSI_WRITE10)
            {
                SCSI_Write10_Cmd(CBW.bLUN , SCSI_LBA , SCSI_BlkLen);
                break;
            }
            Bot_Abort(DIR_OUT);
            Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
            Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
            break;
        default:
            Bot_Abort(BOTH_DIR);
            Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
            Set_CSW (CSW_PHASE_ERROR, SEND_CSW_DISABLE);
            break;
    }
}

/**
*\*\name    CBW_Decode.
*\*\fun     Decode the received CBW and call the related SCSI command.
*\*\param   none
*\*\return  none 
**/
void CBW_Decode(void)
{
    uint32_t Counter;

    for (Counter = 0; Counter < Data_Len; Counter++)
    {
        *((uint8_t *)&CBW + Counter) = Bulk_Data_Buff[Counter];
    }
    CSW.dTag = CBW.dTag;
    CSW.dDataResidue = CBW.dDataLength;
    if (Data_Len != BOT_CBW_PACKET_LENGTH)
    {
        Bot_Abort(BOTH_DIR);
        /* reset the CBW.dSignature to disable the clear feature until receiving a Mass storage reset*/
        CBW.dSignature = 0;
        Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, PARAMETER_LIST_LENGTH_ERROR);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        return;
    }

    if ((CBW.CB[0] == SCSI_READ10 ) || (CBW.CB[0] == SCSI_WRITE10 ))
    {
        /* Calculate Logical Block Address */
        SCSI_LBA = (CBW.CB[2] << 24) | (CBW.CB[3] << 16) | (CBW.CB[4] <<  8) | CBW.CB[5];
        /* Calculate the Number of Blocks to transfer */
        SCSI_BlkLen = (CBW.CB[7] <<  8) | CBW.CB[8];
    }

    if (CBW.dSignature == BOT_CBW_SIGNATURE)
    {
        /* Valid CBW */
        if ((CBW.bLUN > Max_Lun) || (CBW.bCBLength < 1) || (CBW.bCBLength > 16))
        {
            Bot_Abort(BOTH_DIR);
            Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_FIELED_IN_COMMAND);
            Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
        }
        else
        {
            switch (CBW.CB[0])
            {
                case SCSI_REQUEST_SENSE:
                    SCSI_RequestSense_Cmd (CBW.bLUN);
                    break;
                case SCSI_INQUIRY:
                    SCSI_Inquiry_Cmd(CBW.bLUN);
                    break;
                case SCSI_START_STOP_UNIT:
                    SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
                    break;
                case SCSI_ALLOW_MEDIUM_REMOVAL:
                    SCSI_Start_Stop_Unit_Cmd(CBW.bLUN);
                    break;
                case SCSI_MODE_SENSE6:
                    SCSI_ModeSense6_Cmd (CBW.bLUN);
                    break;
                case SCSI_MODE_SENSE10:
                    SCSI_ModeSense10_Cmd (CBW.bLUN);
                    break;
                case SCSI_READ_FORMAT_CAPACITIES:
                    SCSI_ReadFormatCapacity_Cmd(CBW.bLUN);
                    break;
                case SCSI_READ_CAPACITY10:
                    SCSI_ReadCapacity10_Cmd(CBW.bLUN);
                    break;
                case SCSI_TEST_UNIT_READY:
                    SCSI_TestUnitReady_Cmd(CBW.bLUN);
                    break;
                case SCSI_READ10:
                    SCSI_Read10_Cmd(CBW.bLUN, SCSI_LBA , SCSI_BlkLen);
                    break;
                case SCSI_WRITE10:
                    SCSI_Write10_Cmd(CBW.bLUN, SCSI_LBA , SCSI_BlkLen);
                    break;
                case SCSI_VERIFY10:
                    SCSI_Verify10_Cmd(CBW.bLUN);
                    break;
                case SCSI_FORMAT_UNIT:
                    SCSI_Format_Cmd(CBW.bLUN);
                    break;
                /*Unsupported command*/
                case SCSI_MODE_SELECT10:
                    SCSI_Mode_Select10_Cmd(CBW.bLUN);
                    break;
                case SCSI_MODE_SELECT6:
                    SCSI_Mode_Select6_Cmd(CBW.bLUN);
                    break;

                case SCSI_SEND_DIAGNOSTIC:
                    SCSI_Send_Diagnostic_Cmd(CBW.bLUN);
                    break;
                case SCSI_READ6:
                    SCSI_Read6_Cmd(CBW.bLUN);
                    break;
                case SCSI_READ12:
                    SCSI_Read12_Cmd(CBW.bLUN);
                    break;
                case SCSI_READ16:
                    SCSI_Read16_Cmd(CBW.bLUN);
                    break;
                case SCSI_READ_CAPACITY16:
                    SCSI_READ_CAPACITY16_Cmd(CBW.bLUN);
                    break;
                case SCSI_WRITE6:
                    SCSI_Write6_Cmd(CBW.bLUN);
                    break;
                case SCSI_WRITE12:
                    SCSI_Write12_Cmd(CBW.bLUN);
                    break;
                case SCSI_WRITE16:
                    SCSI_Write16_Cmd(CBW.bLUN);
                    break;
                case SCSI_VERIFY12:
                    SCSI_Verify12_Cmd(CBW.bLUN);
                    break;
                case SCSI_VERIFY16:
                    SCSI_Verify16_Cmd(CBW.bLUN);
                    break;

                default:
                {
                    Bot_Abort(BOTH_DIR);
                    Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
                    Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
                }
            }
        }
    }
    else
    {
        /* Invalid CBW */
        Bot_Abort(BOTH_DIR);
        Set_Scsi_Sense_Data(CBW.bLUN, ILLEGAL_REQUEST, INVALID_COMMAND);
        Set_CSW (CSW_CMD_FAILED, SEND_CSW_DISABLE);
    }
}

/**
*\*\name    Transfer_Data_Request.
*\*\fun     Send the request response to the PC HOST.
*\*\param   Data_Pointer: point to the data to transfer.
*\*\param   Data_Len: the number of Bytes to transfer.
*\*\return  none 
**/
void Transfer_Data_Request(uint8_t* Data_Pointer, uint16_t Data_Len)
{
    USB_SilWrite(EP1_IN, Data_Pointer, Data_Len);

    SetEPTxStatus(ENDP1, EP_TX_VALID);

    Bot_State = BOT_DATA_IN_LAST;
    CSW.dDataResidue -= Data_Len;
    CSW.bStatus = CSW_CMD_PASSED;
}

/**
*\*\name    Set_CSW.
*\*\fun     Set the SCW with the needed fields.
*\*\param   CSW_Status:
*\*\            - CSW_CMD_PASSED
*\*\            - CSW_CMD_FAILED
*\*\            - CSW_PHASE_ERROR.
*\*\param   Send_Permission: send permission.
*\*\return  none 
**/
void Set_CSW (uint8_t CSW_Status, uint8_t Send_Permission)
{
    CSW.dSignature = BOT_CSW_SIGNATURE;
    CSW.bStatus = CSW_Status;

    USB_SilWrite(EP1_IN, ((uint8_t *)& CSW), CSW_DATA_LENGTH);

    Bot_State = BOT_ERROR;
    if (Send_Permission)
    {
        Bot_State = BOT_CSW_Send;
        SetEPTxStatus(ENDP1, EP_TX_VALID);
    }
}

/**
*\*\name    Bot_Abort.
*\*\fun     Stall the needed Endpoint according to the selected direction.
*\*\param   Direction:
*\*\            - DIR_IN
*\*\            - DIR_OUT
*\*\            - BOTH_DIR.
*\*\return  none 
**/
void Bot_Abort(uint8_t Direction)
{
    switch (Direction)
    {
        case DIR_IN :
            SetEPTxStatus(ENDP1, EP_TX_STALL);
            break;
        case DIR_OUT :
            SetEPRxStatus(ENDP1, EP_RX_STALL);
            break;
        case BOTH_DIR :
            SetEPTxStatus(ENDP1, EP_TX_STALL);
            SetEPRxStatus(ENDP1, EP_RX_STALL);
            break;
        default:
            break;
    }
}

