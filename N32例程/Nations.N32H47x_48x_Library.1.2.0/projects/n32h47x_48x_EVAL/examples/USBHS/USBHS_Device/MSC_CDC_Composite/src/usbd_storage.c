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
*\*\file usbd_storage_template.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include "usbd_msc_mem.h"
#include "n32h47x_48x.h"
#include "spi_flash.h"
#include "n32h47x_48x_sdio.h"

#define STORAGE_LUN_NBR             1

int8_t STORAGE_Init (uint8_t lun);

int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size);

int8_t  STORAGE_IsReady (uint8_t lun);

int8_t  STORAGE_IsWriteProtected (uint8_t lun);

int8_t STORAGE_Read (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);

int8_t STORAGE_Write (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);

int8_t STORAGE_GetMaxLun (void);

/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] = 
{//36
    /* LUN 0 */
    0x00,
    0x80,
    0x02,
    0x02,
    (USBD_STD_INQUIRY_LENGTH - 4u),
    0x00,
    0x00,
    0x00,
    /* Vendor Identification */
    'N', 'a', 't', 'i', 'o', 'n', 's', ' ',         // 8 bytes
    /* Product Identification */
    'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',    //16 bytes
    ' ', 'D', 'i', 's', 'k', ' ', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' ',                             //4 bytes
    
    /* LUN 1 */
    0x00,
    0x80,
    0x02,
    0x02,
    (USBD_STD_INQUIRY_LENGTH - 4u),
    0x00,
    0x00,
    0x00,
    /* Vendor Identification */
    'N', 'a', 't', 'i', 'o', 'n', 's', ' ',         // 8 bytes
    /* Product Identification */
    'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',    //16 bytes
    'D', 'i', 's', 'k', ' ', ' ', ' ',
    /* Product Revision Level */
    '1', '.', '0', ' ',                             //4 bytes
}; 

USBD_STORAGE_cb_TypeDef USBD_Flash_fops =
{
    STORAGE_Init,
    STORAGE_GetCapacity,
    STORAGE_IsReady,
    STORAGE_IsWriteProtected,
    STORAGE_Read,
    STORAGE_Write,
    STORAGE_GetMaxLun,
    (int8_t *)STORAGE_Inquirydata,
  
};

USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = &USBD_Flash_fops;

/**
*\*\name   STORAGE_Init.
*\*\fun    Initialize the storage.
*\*\param  lun : logical unit number
*\*\return Status.
**/
int8_t STORAGE_Init (uint8_t lun)
{
    sFLASH_Init();
    if(sFLASH_W25Q128_ID != sFLASH_ReadID())
    {
        return (-1);
    }
    return (0);
}

/**
*\*\name   STORAGE_GetCapacity.
*\*\fun    Get Storage capacity.
*\*\param  lun : logical unit number
*\*\param  block_num : number of physical block
*\*\param  block_size : size of a physical block
*\*\return Status.
**/
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
    *block_size = 512u;
    *block_num  = 1024u * 1024u * 16u / 512u;

  return (0);
}

/**
*\*\name   STORAGE_IsReady.
*\*\fun    Check whether the storage is ready.
*\*\param  lun : logical unit number
*\*\return Status.
**/
int8_t  STORAGE_IsReady (uint8_t lun)
{
    sFLASH_Init();
    if(sFLASH_W25Q128_ID != sFLASH_ReadID())
    {
        return (-1);
    }
    return (0);
}

/**
*\*\name   STORAGE_IsWriteProtected.
*\*\fun    check whether the storage is write-protected.
*\*\param  lun : logical unit number
*\*\return Status.
**/
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
    return  0;
}

/**
*\*\name   STORAGE_Read.
*\*\fun    Read data from the storage device.
*\*\param  lun: logical unit number
*\*\param  buf: Pointer to the buffer to save data
*\*\param  blk_addr: address of 1st block to be read
*\*\param  blk_len: nmber of blocks to be read
*\*\return Status.
**/
int8_t STORAGE_Read (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    sFLASH_ReadBuffer(buf, blk_addr * 512u, blk_len * 512u);
    return 0;
}

/**
*\*\name   STORAGE_Write.
*\*\fun    Write data to the storage device.
*\*\param  lun: logical unit number
*\*\param  buf: Pointer to the buffer to write from
*\*\param  blk_addr: address of 1st block to be written
*\*\param  blk_len: nmber of blocks to be write
*\*\return Status.
**/
int8_t STORAGE_Write (uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    if(blk_addr%8 == 0)
    {
        sFLASH_EraseSector(blk_addr*512);
    }
    sFLASH_WriteBuffer(buf, blk_addr * 512u, blk_len * 512u);
    return (0);
}

/**
*\*\name   STORAGE_GetMaxLun.
*\*\fun    Return number of supported logical unit.
*\*\param  none
*\*\return number of logical unit.
**/
int8_t STORAGE_GetMaxLun (void)
{
  return (STORAGE_LUN_NBR - 1);
}
