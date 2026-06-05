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
*\*\file usbfsd_ucdr.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __USBFSD_UCDR_H__
#define __USBFSD_UCDR_H__

#include "usbfsd_type.h"
#include "n32h47x_48x.h"

#define UCDR_CTRL           (( uint8_t*)(UCDR_BASE))
#define UCDR_DTCMODESEL     ((uint8_t*)(UCDR_BASE+0x08))
#define UCDR_DLYCNTDTC      ((uint8_t*)(UCDR_BASE+0x10))
#define UCDR_DLYCNTADJ      ((uint8_t*)(UCDR_BASE+0x14))
#define UCDR_CKCNTDTC       ((uint8_t*)(UCDR_BASE+0x1C))
#define UCDR_CKUNITDTC      ((uint8_t*)(UCDR_BASE+0x2C))
#define UCDR_ERRFLAGREG     ((uint8_t*)(UCDR_BASE+0x40))

#define _UCDR_En()                              (*UCDR_CTRL = (*UCDR_CTRL) | 0x01)
#define _UCDR_Dis()                             (*UCDR_CTRL = (*UCDR_CTRL) | 0xFE)
#define _UCDR_SetDpDelayCount(delaycount)       (*UCDR_DTCMODESEL = (((*UCDR_DTCMODESEL) & 0x1F) | ((uint8_t)(delaycount<<5))))
#define _UCDR_ReadDelayCntVal()                 (uint8_t)(*UCDR_DLYCNTDTC)
#define _UCDR_SetDlyCntAdj(adj_dir, adj_value)  (*UCDR_DLYCNTADJ = (uint8_t)((adj_dir<<5) | adj_value))
#define _UCDR_ReadCkCntDtc()                    (uint8_t)(*UCDR_CKCNTDTC)
#define _UCDR_ReadCkUnitDtc()                   (uint8_t)(*UCDR_CKUNITDTC)
#define _UCDR_ReadErrFlag()                     (uint8_t)(*UCDR_ERRFLAGREG)
#define _UCDR_ClrErrFlag(flag)                  (*UCDR_ERRFLAGREG = (*UCDR_ERRFLAGREG) & flag)
#define _UCDR_EnErrInt()                        (*UCDR_ERRFLAGREG = *UCDR_ERRFLAGREG | (0x01<<3))
#define _UCDR_DisErrInt()                       (*UCDR_ERRFLAGREG = *UCDR_ERRFLAGREG & ~(0x01<<3))

typedef enum
{
    NO_DELAY = 0,
    ONE_CYCLE_DELAY = 1,
    TWO_CYCLE_DELAY ,
    THRESS_CYCLE_DELAY
}UCDR_DPDelayType;

typedef enum
{
    NEGATIVE_ADJ = 0,
    POSITIVE_ADJ = 1
}UCDR_DlyCntAdj_Dir;

typedef enum
{
    DLYCNTDIASERRFLAG = 0,
    BITWIDTHCNTBIADERRFLAG = 1,
    DTCERRFLAG = 2,
}UCDR_ErrFlag;

#define IS_UCDR_GET_FLAG(FLAG)  (((FLAG) == DLYCNTDIASERRFLAG) || ((FLAG) == BITWIDTHCNTBIADERRFLAG) || ((FLAG) == DTCERRFLAG))

void UCDR_Enable(void);
void UCDR_Disable(void);
void UCDR_SetDpDlySel(UCDR_DPDelayType  delaycount);
uint8_t UCDR_GetDlyCntDtc(void);
uint8_t UCDR_GetCkCntDtc(void);
uint8_t UCDR_GetCkUnitDtc(void);
void UCDR_SetDlyCntAdj(UCDR_DlyCntAdj_Dir adj_dir, uint8_t adj_value);
void UCDR_EnErrInt(void);
void UCDR_DisErrInt(void);
FlagStatus UCDR_GetErrFlag(uint8_t ucdr_errflag);
void UCDR_ClrErrFlag(uint8_t ucdr_errflag);

#endif /*__USBFSD_UCDR_H__ */

