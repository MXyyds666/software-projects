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
#include "femc_nor.h"
#include "stdio.h"
#include "log.h"

/**
*\*\name    main.
*\*\fun     Main program.
*\*\param   none
*\*\return  none 
**/
int main(void)
{
    uint8_t block_index = 0;
    uint16_t tx_buffer[BUFFER_SIZE];
    uint16_t rx_buffer[BUFFER_SIZE];
    Status wr_status1 = PASSED, wr_status2 = PASSED;
    NOR_IDType Nor_id;
    uint32_t i;

    log_init();
    log_info("  this is a femc external norflash demo \r\n");

    /* FEMC Block select: 1:Block1; 2:Block2; 3:Block3; 4:Block */
    block_index = 1;

    /* Write/read to/from FEMC norflash memory */

    /*config femc ,external 16bit nor flash*/
    FEMC_Nor_Init(block_index);

    /* read nor flash id */
    FEMC_Nor_ReadID(&Nor_id, block_index);
    
    log_info(" FEMC external norflash Manufacturer Code = 0x%x, Device ID1 = 0x%x, Device ID2 = 0x%x, Device ID3 = 0x%x \r\n", 
                Nor_id.Manufacturer_Code, Nor_id.Device_Code1, Nor_id.Device_Code2, Nor_id.Device_Code3);
    
    /* Write data to NOR memory */
    /* Fill the buffer to send */
    Fill_Buffer_16(tx_buffer, BUFFER_SIZE, 0x0001);
    FEMC_Nor_Write_Buffer_Halfword(block_index, tx_buffer, WRITE_READ_ADDR, BUFFER_SIZE); // write buffer

    /* Read data from NOR memory */
    FEMC_Nor_Read_Buffer_Halfword(block_index, rx_buffer, WRITE_READ_ADDR, BUFFER_SIZE);
    
    /* Read back NOR memory and check content correctness */
    wr_status1 = Buffer16cmp(tx_buffer, rx_buffer, BUFFER_SIZE);

    /* Erase the NOR memory block to write on */
    FEMC_Nor_Erase_Block(block_index, WRITE_READ_ADDR);

    /* Read data from NOR memory */
    FEMC_Nor_Read_Buffer_Halfword(block_index, rx_buffer, WRITE_READ_ADDR, BUFFER_SIZE);
    
    for(i=0;i<BUFFER_SIZE;i++)
    {
        if(rx_buffer[i] != 0xFFFF)
        {
            wr_status2 = FAILED;
            break;
        }
    }

    if (wr_status1 == PASSED && wr_status2 == PASSED)
    {
        /* OK */
        log_info(" femc norflash read and write test pass \r\n");
    }
    else
    {
        /* fail*/
        log_info(" femc norflash read and write test fail \r\n");
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

/**
*\*\name    Fill_Buffer_16.
*\*\fun     Fill the 16bits global buffer
*\*\param   pBuffer: pointer on the Buffer to fill
*\*\param   BufferLenght: size of the buffer to fill
*\*\param   Offset: first value to fill on the Buffer
*\*\return  none
**/
void Fill_Buffer_16(uint16_t* pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++)
    {
        pBuffer[IndexTmp] = IndexTmp + Offset;
    }
}

/**
*\*\name    Fill_Buffer_8.
*\*\fun     Fill the 8bits global buffer
*\*\param   pBuffer: pointer on the Buffer to fill
*\*\param   BufferLenght: size of the buffer to fill
*\*\param   Offset: first value to fill on the Buffer
*\*\return  none
**/
void Fill_Buffer_8(uint8_t* pBuffer, uint16_t BufferLenght, uint32_t Offset)
{
    uint16_t IndexTmp = 0;

    /* Put in global buffer same values */
    for (IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++)
    {
        pBuffer[IndexTmp] = IndexTmp + Offset;
    }
}
