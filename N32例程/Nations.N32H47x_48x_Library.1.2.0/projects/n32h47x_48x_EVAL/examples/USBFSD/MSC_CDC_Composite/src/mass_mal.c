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
*\*\file mass_mal.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/


#include "mass_mal.h"
#include "n32h47x_48x_flash.h"
#include "spi_flash.h"


uint32_t Mass_Memory_Size[2];
uint32_t Mass_Block_Size[2];
uint32_t Mass_Block_Count[2];

/* logic unit count; the first is 0 */
uint32_t Max_Lun = 0;

/**
*\*\name    MAL_Init.
*\*\fun     Initializes the Media on the Nations.
*\*\param   lun: logical unit.
*\*\return  MAL_OK. 
**/
uint16_t MAL_Init(uint8_t lun)
{
    uint16_t status = MAL_OK;

    switch (lun)
    {
        case 0:
            sFLASH_Init();
            
            if(sFLASH_ReadID() == sFLASH_W25Q128_ID)
            {
                status = MAL_OK;
            }
            else 
            {
                status = MAL_FAIL;
            }
            break;
            
        case 1:
            break;
            
        default:
            return MAL_FAIL;
    }
    return status;
}

/**
*\*\name    MAL_Write.
*\*\fun     Write sectors.
*\*\param   lun: logical unit.
*\*\param   Memory_Offset: memory offset.
*\*\param   Writebuff: write buff pointer.
*\*\param   Transfer_Length: transfer data length.
*\*\return  MAL_OK or MAL_FAIL.
**/
uint16_t MAL_Write(uint8_t lun, uint32_t Memory_Offset, uint32_t *Writebuff, uint16_t Transfer_Length)
{
    switch (lun)
    {
        case 0:
            if(Memory_Offset%4096 == 0)
            {
                sFLASH_EraseSector(Memory_Offset);
            }
            sFLASH_WriteBuffer((uint8_t *)Writebuff, Memory_Offset, Transfer_Length);
            break;
        
        case 1:
            break;
        
        default:
            return MAL_FAIL;
    }
    return MAL_OK;
}

/**
*\*\name    MAL_Read.
*\*\fun     Read sectors.
*\*\param   lun: logical unit.
*\*\param   Memory_Offset: memory offset.
*\*\param   Readbuff: read buff pointer.
*\*\param   Transfer_Length: transfer data length.
*\*\return  MAL_OK or MAL_FAIL.
**/
uint16_t MAL_Read(uint8_t lun, uint32_t Memory_Offset, uint32_t *Readbuff, uint16_t Transfer_Length)
{
    switch (lun)
    {
        case 0:
            sFLASH_ReadBuffer((uint8_t *)Readbuff, Memory_Offset, Transfer_Length);
            break;
        
        case 1:
            break;
        
        default:
            return MAL_FAIL;
    }
    return MAL_OK;
}

/**
*\*\name    MAL_GetStatus.
*\*\fun     Get status.
*\*\param   lun: logical unit.
*\*\return  MAL_OK or MAL_FAIL.
**/
uint16_t MAL_GetStatus (uint8_t lun)
{
    uint16_t status = MAL_OK;
    switch (lun)
    { 
        case 0:
            Mass_Memory_Size[0] = 16*1024*1024;     /* W25Q128 16M */
            Mass_Block_Size[0]  = 512;
            Mass_Block_Count[0] = Mass_Memory_Size[0] / Mass_Block_Size[0];
            status = MAL_OK;
        break;
        case 1:
        break;
        
        default:
          status = MAL_FAIL;
        
    }
    return status;
}
