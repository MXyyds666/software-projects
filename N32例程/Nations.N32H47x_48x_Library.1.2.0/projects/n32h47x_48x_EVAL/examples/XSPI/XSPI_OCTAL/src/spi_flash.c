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
*\*\file      spi_flash.c
*\*\author    Nations
*\*\version   v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved. 
*/
#include    "n32h47x_48x_xspi.h"
#include    "spi_flash.h"
#include    <stdio.h>
#include    <string.h>

/** User application include */




uint8_t XSPI_FlashREG_WR(uint8_t *cmdbuf,uint8_t *rbuf,uint8_t count,uint8_t type)
{
	uint32_t wtime = 1000,i;
	if(type == TYPE_REG_READ)/*read register*/
	{
		rbuf[0] = 0xff;
		rbuf[1] = 0xff;
		if(XSPI->STS&XSPI_FLAG_TFNF)/*wait tx FIFO not full flag set*/
		{
			XSPI->DAT0 = cmdbuf[0];
		}
		if(XSPI->STS&XSPI_FLAG_TFNF)/*wait tx FIFO not full flag set*/
		{
			XSPI->DAT0 = (0xff);
			if(XSPI->STS&XSPI_FLAG_RFNE)/*wait RX FIFO Not Empty flag set*/
			{
				rbuf[1] = XSPI->DAT0;
			}
		}
		while(((XSPI->STS&XSPI_FLAG_TFE)==RESET)&&wtime)/*wait tx FIFO Empty*/
		{wtime--;}
		wtime = 100;
		do
		{
			if(XSPI->STS&XSPI_FLAG_RFNE)/*wait RX FIFO Not Empty flag set*/
			{
				rbuf[1] = XSPI->DAT0;
			}
			wtime--;
		}
		while(((XSPI->STS&XSPI_FLAG_BUSY)||(XSPI->STS&XSPI_FLAG_RFNE))&&wtime);/*wait XSPI idle*/
	}
	else if(type == TYPE_REG_WRITE)/*write register*/
	{
		for(i = 0;i < count;i++)
		{
			if(XSPI->STS&XSPI_FLAG_TFNF)/*wait tx FIFO not full flag set*/
			{
				XSPI->DAT0 = cmdbuf[i];
			}
		}
		while(((XSPI->STS&XSPI_FLAG_TFE)==RESET)&&wtime)/*wait tx FIFO Empty*/
		{wtime--;} 

		while(XSPI->STS&XSPI_FLAG_BUSY);
	}
	return (!wtime);
}

void XSPI_FlashSendData( uint32_t *pSrcData,  uint32_t *pDstData, uint32_t cnt)
{
	uint32_t num = 0;

	while(num < cnt)/*Tx Fifo not full*/
	{
		while(!(XSPI->STS&XSPI_FLAG_TFNF));/*wait tx FIFO not full flag set*/
		XSPI->DAT0 = (*(pSrcData++));/*write data register*/
		if((XSPI->STS&XSPI_FLAG_RFNE))/*Rx Fifo not empty set*/
		{
			*(pDstData++) = XSPI->DAT0;/*read data register*/
		}
		num++;
	}
    do
	{		
		if((XSPI->STS&XSPI_FLAG_RFNE))/*Rx Fifo not empty set*/
		{
			*(pDstData++) = XSPI->DAT0;/*read data register*/
		}
    }while((XSPI->STS&XSPI_FLAG_BUSY)==SET);
}

void XSPI_FlashWrite( uint32_t PrgCmd,uint32_t addr, uint16_t count,uint32_t *Wbuf)
{
	uint32_t num=0;

	XSPI->DAT0 = (PrgCmd);
	XSPI->DAT0 = (addr);
	num = 0;
	while(num<count)
	{
		if((XSPI->STS&XSPI_FLAG_TFNF)==XSPI_FLAG_TFNF)/*TX FIFO not Full flag set */
		{
			XSPI->DAT0 = (Wbuf[num++]);
		}
	}
	while((XSPI->STS&XSPI_FLAG_TFE)==RESET);/*tx FIFO Empty flag set*/
	while(XSPI->STS&XSPI_FLAG_BUSY);/*wait xspi not busy*/
	
}

int XSPI_FlashRead(uint32_t PrgCmd, uint32_t  StrAddr, uint32_t Len, uint32_t *DstBuf)
{
	u32 num=0;
	XSPI->DAT0 = PrgCmd;
	XSPI->DAT0 = StrAddr;
	num=0;
	while(num<Len)
	{
		if((XSPI->STS & XSPI_FLAG_RFNE))/*RX FIFO not empty*/
		{
			DstBuf[num++] = XSPI->DAT0;
		}
	}
	return 0;
}





