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


uint32_t XSPI_Wbuffer[256]={0};
uint32_t XSPI_Rbuffer[256]={0};

void Delay(__IO uint32_t nCount)	 
{
	for(; nCount != 0; nCount--);
}

/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none 
**/
int main(void)
{   
    /*SystemInit() function has been called by startup file startup_n32h4xx.s*/
	unsigned char bufw[4] = {0xff,0xff};
	unsigned char bufr[4] = {0xff,0xff};
    u32 flash_addr=0x00,i = 0x00;
    
    log_init();
    log_info("\nSpi_flash write and read demo start - octal mode\r\n");
    
    /* System Clocks Configuration */
    RCC_Configuration();
    
    /* Configure the GPIO ports */
    GPIO_Configuration();
    
    /* XSPI Configuration */
    XSPI_Configuration(STANDRD_SPI_FORMAT,TX_AND_RX,DFS_08_BIT,CTRL1_NDF_255,50);
	
          
	/*==========================Erase flash===========================*/
	do/*write enable*/
	{
		bufw[0] = SPIFLASH_Write_Enable;
		XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_WRITE);
		bufw[0] = SPIFLASH_Read_Reg1;
		XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_READ);
	}while((bufr[1]&0x03) != 0x02);
	/*Erase flash*/
	bufw[0] = SPIFLASH_Block_Erase32KB;
	bufw[1] = (flash_addr & 0xff0000) >> 16;
	bufw[2] = (flash_addr & 0xff00) >> 8;
	bufw[3] = flash_addr & 0xff;
	XSPI_FlashREG_WR(bufw,bufr,4,TYPE_REG_WRITE);
    log_info("============Erase=================\r\n");
	
    do/*Waiting for flash idle*/
	{
        bufw[0] = SPIFLASH_Read_Reg1;
		XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_READ);
	}while((bufr[1]&0x03) != 0x00);	
    	
    /*==========================Read flash===========================*/
    XSPI_Wbuffer[0]=SPIFLASH_Read_Data;
    XSPI_Wbuffer[1]=(flash_addr & 0xff0000) >> 16;;
    XSPI_Wbuffer[2]=(flash_addr & 0xff00) >> 8;;
    XSPI_Wbuffer[3]=flash_addr & 0xff;;
    XSPI_FlashSendData(XSPI_Wbuffer,XSPI_Rbuffer,0x104);
    log_info("============Read:XSPI->BAUDR=%d==========\r\n",XSPI->BAUD);
    for(i = 4; i < (TEST_WR_BYTE_LEN+4); i++)
    {
        if(XSPI_Rbuffer[i] != 0xFF)
        {
            printf("Erase flash fail,i=0X%X\r\n",i);
            break;
        }
    }
        
	/*============================Write flash==========================*/	
    for(i = 0; i < TEST_WR_BYTE_LEN; i++)
    {
        XSPI_Wbuffer[i] = i;
    }
	do/*write enable*/
	{
		bufw[0] = SPIFLASH_Write_Enable;
		XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_WRITE);
		bufw[0] = SPIFLASH_Read_Reg1;
		XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_READ);
	}while((bufr[1]&0x03) != 0x02);
    
	XSPI_Configuration(OCTAL_SPI_FORMAT,TX_ONLY,DFS_08_BIT,CTRL1_NDF_255,TEST_WR_CLKDIV);
	XSPI_FlashWrite(SPIFLASH_OCTALPage_Pro,flash_addr,TEST_WR_BYTE_LEN,XSPI_Wbuffer);
	log_info("============Write:XSPI->BAUDR=%d=========\r\n",XSPI->BAUD);
	
    
	XSPI_Configuration(STANDRD_SPI_FORMAT,TX_AND_RX,DFS_08_BIT,CTRL1_NDF_255,50);
	do//Waiting for flash idle
	{
        bufw[0] = SPIFLASH_Read_Reg1;
		XSPI_FlashREG_WR(bufw,bufr,1,TYPE_REG_READ);
	}while((bufr[1]&0x01) != 0x00);	
    
	/*============================Read flash===========================*/	
	XSPI_Configuration(OCTAL_SPI_FORMAT,RX_ONLY,DFS_08_BIT,CTRL1_NDF_255,TEST_WR_CLKDIV);
	XSPI_FlashRead(SPIFLASH_Read_OCTAL,flash_addr,TEST_WR_BYTE_LEN,XSPI_Rbuffer);	
	log_info("============Read:XSPI->BAUDR=%d==========\r\n",XSPI->BAUD);
	
    /*========================Compare read and write data=====================*/	
	for(i = 0; i < TEST_WR_BYTE_LEN; i++)
	{
		if(XSPI_Rbuffer[i] != XSPI_Wbuffer[i])
		{
			log_info("FALSH WR fail,i=0X%X,XSPI_Rbuffer[i]=0X%X,XSPI_Wbuffer[i]=0X%X\r\n",i,XSPI_Rbuffer[i],XSPI_Wbuffer[i]);
			break;
		}
	}
	log_info("Spi_flash write and read demo end - octal mode\r\n");

    while (1)
    {      
     
    }
}



