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
 * @file sdcard.h
 * @author Nations
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
 */
#ifndef __SDCARD_H__
#define __SDCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x.h"

#define SD_CLK_PORT     GPIOB
#define SD_CLK_PIN      GPIO_PIN_2
#define SD_CLK_ENABLE   RCC_AHB_PERIPHEN_GPIOB
#define SD_CLK_AF       GPIO_AF10

#define SD_CMD_PORT     GPIOD
#define SD_CMD_PIN      GPIO_PIN_2
#define SD_CMD_ENABLE   RCC_AHB_PERIPHEN_GPIOD
#define SD_CMD_AF       GPIO_AF2

#define SD_D0_PORT      GPIOC
#define SD_D0_PIN       GPIO_PIN_8
#define SD_D0_ENABLE    RCC_AHB_PERIPHEN_GPIOC
#define SD_D0_AF        GPIO_AF2

#define SD_D1_PORT      GPIOC
#define SD_D1_PIN       GPIO_PIN_9
#define SD_D1_ENABLE    RCC_AHB_PERIPHEN_GPIOC
#define SD_D1_AF        GPIO_AF2

#define SD_D2_PORT      GPIOC
#define SD_D2_PIN       GPIO_PIN_10
#define SD_D2_ENABLE    RCC_AHB_PERIPHEN_GPIOC
#define SD_D2_AF        GPIO_AF2

#define SD_D3_PORT      GPIOC
#define SD_D3_PIN       GPIO_PIN_11
#define SD_D3_ENABLE    RCC_AHB_PERIPHEN_GPIOC
#define SD_D3_AF        GPIO_AF2

/** Card Specific Data: CSD Registers **/
typedef struct
{
    __IO uint8_t  CSDStruct;           /* CSD structure                          */
    __IO uint8_t  SysSpecVersion;      /* System specification version           */
    __IO uint8_t  Reserved1;           /* Reserved                               */
    __IO uint8_t  TAAC;                /* Data read access-time 1                */
    __IO uint8_t  NSAC;                /* Data read access-time 2 in CLK cycles  */
    __IO uint8_t  MaxBusClkFrec;       /* Max. bus clock frequency               */
    __IO uint16_t CardComdClasses;     /* Card command classes                   */
    __IO uint8_t  RdBlockLen;          /* Max. read data block length            */
    __IO uint8_t  PartBlockRead;       /* Partial blocks for read allowed        */
    __IO uint8_t  WrBlockMisalign;     /* Write block misalignment               */
    __IO uint8_t  RdBlockMisalign;     /* Read block misalignment                */
    __IO uint8_t  DSRImpl;             /* DSR implemented                        */
    __IO uint8_t  Reserved2;           /* Reserved                               */
    __IO uint32_t DeviceSize;          /* Device Size                            */
    __IO uint8_t  MaxRdCurrentVDDMin;  /* Max. read current @ VDD min            */
    __IO uint8_t  MaxRdCurrentVDDMax;  /* Max. read current @ VDD max            */
    __IO uint8_t  MaxWrCurrentVDDMin;  /* Max. write current @ VDD min           */
    __IO uint8_t  MaxWrCurrentVDDMax;  /* Max. write current @ VDD max           */
    __IO uint8_t  DeviceSizeMul;       /* Device size multiplier                 */
    __IO uint8_t  EraseGrSize;         /* Erase group size                       */
    __IO uint8_t  EraseGrMul;          /* Erase group size multiplier            */
    __IO uint8_t  WrProtectGrSize;     /* Write protect group size               */
    __IO uint8_t  WrProtectGrEnable;   /* Write protect group enable             */
    __IO uint8_t  ManDeflECC;          /* Manufacturer default ECC               */
    __IO uint8_t  WrSpeedFact;         /* Write speed factor                     */
    __IO uint8_t  MaxWrBlockLen;       /* Max. write data block length           */
    __IO uint8_t  WriteBlockPaPartial; /* Partial blocks for write allowed       */
    __IO uint8_t  Reserved3;           /* Reserded                               */
    __IO uint8_t  ContentProtectAppli; /* Content protection application         */
    __IO uint8_t  FileFormatGroup;     /* File format group                      */
    __IO uint8_t  CopyFlag;            /* Copy flag (OTP)                        */
    __IO uint8_t  PermWrProtect;       /* Permanent write protection             */
    __IO uint8_t  TempWrProtect;       /* Temporary write protection             */
    __IO uint8_t  FileFormat;          /* File Format                            */
    __IO uint8_t  ECC;                 /* ECC code                               */
    __IO uint8_t  CSD_CRC;             /* CSD CRC                                */
    __IO uint8_t  Reserved4;           /* always 1                               */
} SD_CSD_InfoType;

/** Card Specific Data: CID Registers **/
typedef struct
{
    __IO uint8_t  ManufacturerID; /* ManufacturerID         */
    __IO uint16_t OEM_AppliID;    /* OEM/Application ID     */
    __IO uint32_t ProdName1;      /* Product Name part1     */
    __IO uint8_t  ProdName2;      /* Product Name part2     */
    __IO uint8_t  ProdRev;        /* Product Revision       */
    __IO uint32_t ProdSN;         /* Product Serial Number  */
    __IO uint8_t  Reserved1;      /* Reserved1              */
    __IO uint16_t ManufactDate;   /* Manufacturing Date     */
    __IO uint8_t  CID_CRC;        /* CID CRC                */
    __IO uint8_t  Reserved2;      /* always 1               */
} SD_CID_InfoType;

/** SD Card Information Structure definition **/ 
typedef struct
{
    uint32_t CardType;      /* Specifies the card Type                         */
    uint32_t CardVersion;   /* Specifies the card version                      */
    uint32_t Class;         /* Specifies the class of the card class           */
    uint32_t RelCardAdd;    /* Specifies the Relative Card Address             */
    uint32_t BlockNbr;      /* Specifies the Card Capacity in blocks           */
    uint32_t BlockSize;     /* Specifies one block size in bytes               */
    uint32_t LogBlockNbr;   /* Specifies the Card logical Capacity in blocks   */
    uint32_t LogBlockSize;  /* Specifies logical block size in bytes           */
    SD_CSD_InfoType CSD;    /* Specifies the CSD infomation                    */
    SD_CID_InfoType CID;    /* Specifies the CID infomation                    */
}SD_CardInfoType;

/** SD Card Status **/
typedef struct
{
    __IO uint8_t  DataBusWidth;         /* Shows the currently defined data bus width                 */
    __IO uint8_t  SecuredMode;          /* Card is in secured mode of operation                       */
    __IO uint16_t CardType;             /* Carries information about card type                        */
    __IO uint32_t ProtectedAreaSize;    /* Carries information about the capacity of protected area   */
    __IO uint8_t  SpeedClass;           /* Carries information about the speed class of the card      */
    __IO uint8_t  PerformanceMove;      /* Carries information about the card's performance move      */
    __IO uint8_t  AllocationUnitSize;   /* Carries information about the card's allocation unit size  */
    __IO uint16_t EraseSize;            /* Determines the number of AUs to be erased in one operation */
    __IO uint8_t  EraseTimout;         /* Determines the timeout for any number of AU erase          */
    __IO uint8_t  EraseOffset;          /* Carries information about the erase offset                 */
} SD_CardStatusType;

/* SDIO clock divider define,SDIO_CLK = (HCLK/2)/(2 + CLK_DIV)*/
#define SD_INIT_CLK_DIV             (298U)  /* SDIO Intialization Frequency (400KHz max) */
#define SD_TRANSFER_CLK_DIV         (4U)    /* SDIO Data Transfer Frequency (25MHz max) */

/* SD Supported Memory Cards define */
#define CARD_SDSC                   (0x00000000U)   /* SD Standard Capacity <2G                          */
#define CARD_SDHC_SDXC              (0x00000001U)   /* SD High Capacity <32G, SD Extended Capacity <2T  */
#define CARD_SDIO                   (0x00000003U)
#define CARD_MMC_LC                 (0x00000004U)   /* MMC Card Capacity <=2Gbytes   */
#define CARD_MMC_HC                 (0x00000005U)   /* MMC Card Capacity >2Gbytes and <2Tbytes   */

/* SD Supported Version define */
#define CARD_V1_X                   (0x00000000U)
#define CARD_V2_X                   (0x00000001U)

/** SDIO Transfer state define  **/
#define SD_TRANSFER_OK                  (0U)
#define SD_TRANSFER_BUSY                (1U)
#define SD_TRANSFER_ERROR               (2U)

/* Block size is 512 bytes */
#define SD_BLOCKSIZE                (512U) 


void SD_DeInit(void);
void SD_PortInit(void);
uint32_t SD_Init(uint32_t ClockBypass, uint32_t ClkDiv, uint32_t BusWidth);
uint32_t SD_GetCardState(void);
uint32_t SD_PowerON(void);
uint32_t SD_InitCard(void);

uint32_t SD_GetCardCID(SD_CID_InfoType *pCID);
uint32_t SD_GetCardCSD(SD_CSD_InfoType *pCSD);
uint32_t SD_GetCardInfo(SD_CardInfoType* pCardInfo);

uint32_t SD_GetCardStatus(SD_CardStatusType *pCardSts);
uint32_t SD_CofigBusWidth(uint32_t WideMode);
uint32_t SD_ReadBlocks(uint8_t* ReadBuf, uint32_t BlockAddr, uint32_t NumberOfBlocks, uint32_t Timeout);
uint32_t SD_WriteBlocks(uint8_t* SrcBuf, uint32_t BlockAddr, uint32_t NumberOfBlocks, uint32_t Timeout);
uint32_t SD_Erase(uint32_t StrAddr, uint32_t EndAddr);
uint32_t SD_ClearPWD(uint8_t *buf, uint32_t BlockSize, uint32_t Timeout);

uint32_t SD_SendCardStatus(uint32_t* pCardStatus);
uint32_t SD_SendSDStatus(uint32_t* pSts);

uint32_t MMC_GetExtCSD(uint32_t *pExtCSD, uint32_t Timeout);
uint32_t MMC_ReadExtCSD(uint32_t *pData, uint16_t Index, uint32_t Timeout);
static uint32_t MMC_PwrClassUpdate(uint32_t WideMode);
static uint32_t MMC_SetWideBus(uint32_t WideMode);
static uint32_t SD_EnableWideBus(FunctionalState Cmd);
static uint32_t FindSCR(uint16_t rca, uint32_t* pScr);


#ifdef __cplusplus
}
#endif

#endif /* __SDCARD_H__ */

