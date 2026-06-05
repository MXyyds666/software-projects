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
*\*\file      main.c
*\*\author    Nations
*\*\version   v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved. 
**/
#include "main.h"

#define BUFSIZE 256
uint32_t XSPI_Wbuffer[BUFSIZE]={0};
uint32_t XSPI_Rbuffer[BUFSIZE]={0};

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

void Delay(__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;
TestStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength);

/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none 
**/
int main(void)
{   
    /*SystemInit() function has been called by startup file startup_n32h4xx.s*/
    uint16_t i = 0x00;
    uint16_t Index = 0;
    
    log_init();
    log_info("\nSpi_flash write and read demo start - quad mode\r\n");
    
    /* System Clocks Configuration */
    RCC_Configuration();
    
    /* Configure the GPIO ports */
    GPIO_Configuration();

    /*Set QE*/
    sFLASH_QuadEnable();
    
    for(i=0;i<BUFSIZE;i++)
    {
        XSPI_Rbuffer[i] = 0x00000000;
    }

    /* Read data from SPI FLASH memory */
    if(sFLASH_ReadBuffer(XSPI_Rbuffer, FLASH_ReadAddress, BUFSIZE) != SUCCESS_STATUS)
    {
        log_info("Spi_flash read buffer failed \r\n");
    }
    
    /*Load Send Data*/  
    for(i = 0; i < BUFSIZE; i++)
    {
        XSPI_Wbuffer[i] = 0xCC660000+i;
    }
    
    if(sFLASH_EraseSector(FLASH_SectorToErase) != SUCCESS_STATUS)
    {
        log_info("Spi_flash erase sector failed \r\n");
    }
    
    /* Write XSPI_Wbuffer data to SPI FLASH memory */
    if(sFLASH_WriteBuffer(XSPI_Wbuffer, FLASH_ReadAddress, BUFSIZE) != SUCCESS_STATUS)
    {
        log_info("Spi_flash write buffer failed \r\n");
    }
    
    if(sFLASH_ReadBuffer(XSPI_Rbuffer, FLASH_ReadAddress, BUFSIZE) != SUCCESS_STATUS)
    {
        log_info("Spi_flash read buffer failed \r\n");
    }
    
    TransferStatus1 = Buffercmp(XSPI_Wbuffer, XSPI_Rbuffer, BUFSIZE);
    
    if(sFLASH_EraseChip() != SUCCESS_STATUS)
    {
        log_info("Spi_flash erase chip failed \r\n");
    }

    /* Read data from SPI FLASH memory */
    if(sFLASH_ReadBuffer(XSPI_Rbuffer, FLASH_ReadAddress, BUFSIZE) != SUCCESS_STATUS)
    {
        log_info("Spi_flash read buffer failed \r\n");
    }
    
    /* Check the correctness of erasing operation dada */
    for (Index = 0; Index < BUFSIZE; Index++)
    {
        if (XSPI_Rbuffer[Index] != 0xFFFFFFFF)
        {
            TransferStatus2 = FAILED;
            break;
        }
    }

    if(TransferStatus1 != PASSED || TransferStatus2 != PASSED)
    {
        log_info("Spi_flash write and read demo test failed \r\n");
    }
    else
    {
        log_info("Spi_flash write and read demo test success \r\n");
    }

    while (1)
    {
     
    }
}


/**
*\*\name    Buffercmp.
*\*\fun     Compares two buffers.
*\*\param   pBuffer1
*\*\param   pBuffer2
*\*\param   BufferLength
*\*\return  FAILED or PASSED
**/
TestStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength)
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


