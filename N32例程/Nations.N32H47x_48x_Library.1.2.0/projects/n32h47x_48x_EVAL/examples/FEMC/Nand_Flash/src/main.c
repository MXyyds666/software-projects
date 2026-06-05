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
*\*\file main.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "main.h"
#include "femc_nand.h"
#include "stdio.h"
#include "log.h"
#include "n32h47x_48x_femc.h"

#define NAND_BUFFER_SIZE     0x800
#define NAND_FLASH_ZONE_NUM  0x00
#define NAND_FLASH_BLOCK_NUM 0x00
#define NAND_FALSH_PAGE_NUM  0x00

/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none
*\*\return  none 
**/
int main(void)
{
    uint8_t test_result = 0;
    NAND_IDModule nand_id;
    NAND_ADDRESS access_addr;
    uint32_t page_number = 1;
    uint8_t nand_flash_width;
    uint8_t bank_id;
    uint32_t i, j;
    uint8_t status;
    uint8_t transfer_buffer[NAND_BUFFER_SIZE];
    uint32_t bank_address;

    log_init();
    log_info("this is a femc external nand flash demo\r\n");

    nand_flash_width = 8; // config memory bus width
    bank_id          = 3; // config band index,2:bank2;3:bank3,must config FEMC_BANK_NAND in femc_nand.c.
    bank_address     = BANK3_NAND_ADDR;
    
    FEMC_Nand_Init(bank_id, nand_flash_width);
    log_info(" nand flash test start \r\n");
    FEMC_Nand_ReadID(bank_address, &nand_id);

    /*set NAND Flash write address*/
    access_addr.Zone  = NAND_FLASH_ZONE_NUM;
    access_addr.Block = NAND_FLASH_BLOCK_NUM;
    access_addr.Page  = NAND_FALSH_PAGE_NUM;

    /*write and read nand flash with AHB width = 8*/
    /* Fill the buffer to send */
    for (i = 0; i < NAND_BUFFER_SIZE; i++)
    {
        transfer_buffer[i] = i;
    }
    /*erase blocks*/
    status = FEMC_Nand_Erase_Block(bank_address, access_addr);
    
    if(status == NAND_READY)
    {
        log_info(" erase block successfully!\r\n");
    }
    else
    {
        log_info(" erase block fail!\r\n");
    }

    /*write data to nand flash*/
    status = FEMC_Nand_WritePage_Byte(bank_address, transfer_buffer, access_addr, page_number);

    if(status == NAND_READY)
    {
        log_info(" write page successfully!\r\n");
    }
    else
    {
        log_info(" write page fail!\r\n");
    }

    /*read data from nand flash and save to transfer_buffer*/
    for (i = 0; i < NAND_BUFFER_SIZE; i++)
    {
        transfer_buffer[i] = 0;
    }
    status = FEMC_Nand_ReadPage_Byte(bank_address, transfer_buffer, access_addr, page_number);

    if(status == NAND_READY)
    {
        log_info(" read page successfully!\r\n");
    }
    else
    {
        log_info(" read page fail!\r\n");
    }
    
    /*compare the read-back data with the written data*/
    for (j = 0; j < NAND_BUFFER_SIZE; j++)
    {
        if (transfer_buffer[j] != (uint8_t)j)
        {
            test_result = 1;
            break;
        }
    }
    if (test_result == 0)
    {
        /* OK */
        log_info("femc external 8bit nandflash test pass\r\n");
        test_result = PASSED;
    }
    else
    {
        /* fail */
        log_info("femc external 8bit nandflash test fail\r\n");
        test_result = FAILED;
    }

    while (1)
    {
    }
}


/**
*\*\name    Buffer16cmp.
*\*\fun     Compare two 16bits strings to see if they are the same
*\*\param   pBuffer1: Compared string 1 pointer
*\*\param   pBuffer2: Compared string 2 pointer
*\*\param   BufferLength : Compared string length
*\*\return  PASSED or FAILED
**/
Status Buffer16cmp(const uint16_t* pBuffer1, uint16_t* pBuffer2, uint32_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }
        pBuffer1++;
        pBuffer2++;
    }
    return PASSED;
}

/**
*\*\name    Buffer8cmp.
*\*\fun     Compare two 8bits strings to see if they are the same
*\*\param   pBuffer1: Compared string 1 pointer
*\*\param   pBuffer2: Compared string 2 pointer
*\*\param   BufferLength : Compared string length
*\*\return  PASSED or FAILED
**/
Status Buffer8cmp(const uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return FAILED;
        }
        pBuffer1++;
        pBuffer2++;
    }

    return PASSED;
}

