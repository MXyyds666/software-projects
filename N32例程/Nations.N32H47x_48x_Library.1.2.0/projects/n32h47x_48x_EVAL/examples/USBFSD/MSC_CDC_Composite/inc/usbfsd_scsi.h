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
*\*\file usbfsd_scsi.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBFSD_SCSI_H__
#define __USBFSD_SCSI_H__

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usbfsd_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* SCSI Commands */
#define SCSI_FORMAT_UNIT                            0x04
#define SCSI_INQUIRY                                0x12
#define SCSI_MODE_SELECT6                           0x15
#define SCSI_MODE_SELECT10                          0x55
#define SCSI_MODE_SENSE6                            0x1A
#define SCSI_MODE_SENSE10                           0x5A
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E
#define SCSI_READ6                                  0x08
#define SCSI_READ10                                 0x28
#define SCSI_READ12                                 0xA8
#define SCSI_READ16                                 0x88

#define SCSI_READ_CAPACITY10                        0x25
#define SCSI_READ_CAPACITY16                        0x9E

#define SCSI_REQUEST_SENSE                          0x03
#define SCSI_START_STOP_UNIT                        0x1B
#define SCSI_TEST_UNIT_READY                        0x00
#define SCSI_WRITE6                                 0x0A
#define SCSI_WRITE10                                0x2A
#define SCSI_WRITE12                                0xAA
#define SCSI_WRITE16                                0x8A

#define SCSI_VERIFY10                               0x2F
#define SCSI_VERIFY12                               0xAF
#define SCSI_VERIFY16                               0x8F

#define SCSI_SEND_DIAGNOSTIC                        0x1D
#define SCSI_READ_FORMAT_CAPACITIES                 0x23

#define NO_SENSE                                0
#define RECOVERED_ERROR                         1
#define NOT_READY                               2
#define MEDIUM_ERROR                            3
#define HARDWARE_ERROR                          4
#define ILLEGAL_REQUEST                         5
#define UNIT_ATTENTION                          6
#define DATA_PROTECT                            7
#define BLANK_CHECK                             8
#define VENDOR_SPECIFIC                         9
#define COPY_ABORTED                            10
#define ABORTED_COMMAND                         11
#define VOLUME_OVERFLOW                         13
#define MISCOMPARE                              14


#define INVALID_COMMAND                             0x20
#define INVALID_FIELED_IN_COMMAND                   0x24
#define PARAMETER_LIST_LENGTH_ERROR                 0x1A
#define INVALID_FIELD_IN_PARAMETER_LIST             0x26
#define ADDRESS_OUT_OF_RANGE                        0x21
#define MEDIUM_NOT_PRESENT                          0x3A
#define MEDIUM_HAVE_CHANGED                         0x28

#define READ_FORMAT_CAPACITY_DATA_LEN               0x0C
#define READ_CAPACITY10_DATA_LEN                    0x08
#define MODE_SENSE10_DATA_LEN                       0x08
#define MODE_SENSE6_DATA_LEN                        0x04
#define REQUEST_SENSE_DATA_LEN                      0x12
#define STANDARD_INQUIRY_DATA_LEN                   0x24
#define BLKVFY                                      0x04

extern  uint8_t Page00_Inquiry_Data[];
extern  uint8_t Standard_Inquiry_Data[];
extern  uint8_t Standard_Inquiry_Data1[];
extern  uint8_t Mode_Sense6_data[];
extern  uint8_t Mode_Sense10_data[];
extern  uint8_t Scsi_Sense_Data[];
extern  uint8_t ReadCapacity10_Data[];
extern  uint8_t ReadFormatCapacity_Data [];

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SCSI_Inquiry_Cmd(uint8_t lun);
void SCSI_ReadFormatCapacity_Cmd(uint8_t lun);
void SCSI_ReadCapacity10_Cmd(uint8_t lun);
void SCSI_RequestSense_Cmd (uint8_t lun);
void SCSI_Start_Stop_Unit_Cmd(uint8_t lun);
void SCSI_ModeSense6_Cmd (uint8_t lun);
void SCSI_ModeSense10_Cmd (uint8_t lun);
void SCSI_Write10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr);
void SCSI_Read10_Cmd(uint8_t lun , uint32_t LBA , uint32_t BlockNbr);
void SCSI_Verify10_Cmd(uint8_t lun);

void SCSI_Invalid_Cmd(uint8_t lun);
void SCSI_Valid_Cmd(uint8_t lun);
bool SCSI_Address_Management(uint8_t lun , uint8_t Cmd , uint32_t LBA , uint32_t BlockNbr);

void Set_Scsi_Sense_Data(uint8_t lun , uint8_t Sens_Key, uint8_t Asc);
void SCSI_TestUnitReady_Cmd (uint8_t lun);
void SCSI_Format_Cmd (uint8_t lun);

//#define SCSI_TestUnitReady_Cmd           SCSI_Valid_Cmd
#define SCSI_Prevent_Removal_Cmd         SCSI_Valid_Cmd

/* Invalid (Unsupported) commands */
#define SCSI_READ_CAPACITY16_Cmd         SCSI_Invalid_Cmd
//#define SCSI_FormatUnit_Cmd              SCSI_Invalid_Cmd
#define SCSI_Write6_Cmd                  SCSI_Invalid_Cmd
#define SCSI_Write16_Cmd                 SCSI_Invalid_Cmd
#define SCSI_Write12_Cmd                 SCSI_Invalid_Cmd
#define SCSI_Read6_Cmd                   SCSI_Invalid_Cmd
#define SCSI_Read12_Cmd                  SCSI_Invalid_Cmd
#define SCSI_Read16_Cmd                  SCSI_Invalid_Cmd
#define SCSI_Send_Diagnostic_Cmd         SCSI_Invalid_Cmd
#define SCSI_Mode_Select6_Cmd            SCSI_Invalid_Cmd
#define SCSI_Mode_Select10_Cmd           SCSI_Invalid_Cmd
#define SCSI_Verify12_Cmd                SCSI_Invalid_Cmd
#define SCSI_Verify16_Cmd                SCSI_Invalid_Cmd

#endif /* __USBFSD_SCSI_H__ */


