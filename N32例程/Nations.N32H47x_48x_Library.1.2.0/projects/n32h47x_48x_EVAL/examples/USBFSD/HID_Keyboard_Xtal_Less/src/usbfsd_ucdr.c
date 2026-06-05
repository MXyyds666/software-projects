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
*\*\file usbfsd_ucdr.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "usbfsd_ucdr.h"
#include "usbfsd_lib.h"
#include "usbfsd_desc.h"
#include "usbfsd_pwr.h"


/**
*\*\name    UCDR_Enable.
*\*\fun     Enable UCDR.
*\*\param   none
*\*\return  none 
**/
void UCDR_Enable()
{
    _UCDR_En();
}

/**
*\*\name    UCDR_Disable.
*\*\fun     Disable UCDR.
*\*\param   none
*\*\return  none 
**/
void UCDR_Disable()
{
    _UCDR_Dis();
}

/**
*\*\name    UCDR_SetDpDlySel.
*\*\fun     Set UCDR dp delay.
*\*\param   delaycount: delay count
*\*\return  none 
**/
void UCDR_SetDpDlySel(UCDR_DPDelayType  delaycount)
{
    _UCDR_SetDpDelayCount(delaycount);
}

/**
*\*\name    UCDR_GetDlyCntDtc.
*\*\fun     Get UCDR dp delay.
*\*\param   none
*\*\return  delay count 
**/
uint8_t UCDR_GetDlyCntDtc()
{
    return _UCDR_ReadDelayCntVal();
}

/**
*\*\name    UCDR_GetCkCntDtc.
*\*\fun     Get CK cnt delay.
*\*\param   none
*\*\return  CK count 
**/
uint8_t UCDR_GetCkCntDtc()
{
    return _UCDR_ReadCkCntDtc();
}

/**
*\*\name    UCDR_GetCkUnitDtc.
*\*\fun     Get CK Unit cnt.
*\*\param   none
*\*\return  delay count 
**/
uint8_t UCDR_GetCkUnitDtc()
{
    return _UCDR_ReadCkUnitDtc();
}

/**
*\*\name    UCDR_SetDlyCntAdj.
*\*\fun     Set delay count adjust
*\*\param   adj_dir adjust direction, it can be positive adjust or negative adjust
*\*\param   adj_value adjust value
*\*\return  none
**/
void UCDR_SetDlyCntAdj(UCDR_DlyCntAdj_Dir adj_dir, uint8_t adj_value)
{
    if(adj_value > 0x1F)
    {
        adj_value = 0x1F;
    }
    _UCDR_SetDlyCntAdj(adj_dir, adj_value);
}

/**
*\*\name    UCDR_EnErrInt.
*\*\fun     Enable UCDR error interrup
*\*\param   none
*\*\return  none
**/
void UCDR_EnErrInt()
{
    _UCDR_EnErrInt();
}

/**
*\*\name    UCDR_DisErrInt.
*\*\fun     Disable UCDR error interrup
*\*\param   none
*\*\return  none
**/
void UCDR_DisErrInt()
{
    _UCDR_DisErrInt();
}

/**
*\*\name    UCDR_GetErrFlag.
*\*\fun     Checks whether the ucdr error flag has occurred or not.
*\*\param   ucdr_errflag source to check
*\*\return  SET or RESET.
**/
FlagStatus UCDR_GetErrFlag(uint8_t ucdr_errflag)
{
    FlagStatus bitstatus = RESET;
    uint16_t pos = 0, mask = 0, errflagstatus = 0;
    
    /* Get the UCDR ERROR flag index */
    pos = 0x01 << (ucdr_errflag & 0x07);
    
    /* Set the flag mask */
    mask = 0x01 << ucdr_errflag;

    /* Get the ucdr_errflag bit status */
    errflagstatus = (_UCDR_ReadErrFlag() & mask);
    
     /* Check the status of the UCDR ERROR flag */
    if (((_UCDR_ReadErrFlag() & pos) != (uint16_t)RESET) && errflagstatus)
    {
        /* ucdr_errflag is set */
        bitstatus = SET;
    }
    else
    {
        /* ucdr_errflag is reset */
        bitstatus = RESET;
    }
    
    /* Return the ucdr_errflag status */
    return bitstatus;    
}

/**
*\*\name    UCDR_ClrErrFlag.
*\*\fun     Clears the ucdr error flag.
*\*\param   ucdr_errflag source to check
*\*\          -  DLYCNTDIASERRFLAG delay count bias error flag
*\*\          -  BITWIDTHCNTBIADERRFLAG bit width count bias error flag
*\*\          -  DTCERRFLAG SYNC detect error flag
*\*\return  none.
**/
void UCDR_ClrErrFlag(uint8_t ucdr_errflag)
{
    uint8_t pos = 0;
    
    /* Get the UCDR ERROR flag index */
    pos = 0x01 << (ucdr_errflag & 0x07);
    /* Get UCDR error int enable bit */
    pos |= (_UCDR_ReadErrFlag() & (1<<3));
    
    /* Clear the selected UCDR Error flag */
    _UCDR_ClrErrFlag(pos);
}
