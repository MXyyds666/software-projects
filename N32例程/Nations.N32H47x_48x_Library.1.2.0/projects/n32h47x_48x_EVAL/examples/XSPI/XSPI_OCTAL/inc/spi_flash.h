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
*\*\file      spi_flash.h
*\*\author    Nations
*\*\version   v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved. 
*/
#ifndef     _SPI_FLASH__
#define     _SPI_FLASH__
#include    <stdint.h>

#define TEST_ADDR                    (0x000000)

// IS25WX256 flash
#define SPIFLASH_Write_Enable	    (0x06)
#define SPIFLASH_Block_Erase4KB  	(0x20)
#define SPIFLASH_Block_Erase32KB  	(0x52)
#define SPIFLASH_Read_Reg1          (0x05)
#define SPIFLASH_Write_Reg1         (0x01)
#define SPIFLASH_Read_Data 			(0x03)
#define SPIFLASH_Read_Data1 		(0x0B)
#define SPIFLASH_Read_OCTAL         (0x8B)
#define SPIFLASH_Read_OCTAL1        (0xCB)
#define SPIFLASH_Read_DDROCTAL      (0x9D)
#define SPIFLASH_Read_DDROCTAL1     (0xFD)
#define SPIFLASH_Page_Pro           (0x02)
#define SPIFLASH_OCTALPage_Pro      (0x82)

//FLASH WR type
#define TYPE_REG_READ (0x01)
#define TYPE_REG_WRITE (0x02)

#define XSPI_FLAG_BUSY            ((uint32_t)0x00000001)
#define XSPI_FLAG_TFNF            ((uint32_t)0x00000002)
#define XSPI_FLAG_TFE             ((uint32_t)0x00000004)
#define XSPI_FLAG_RFNE            ((uint32_t)0x00000008)
#define XSPI_FLAG_RFF             ((uint32_t)0x00000010)
#define XSPI_FLAG_TXE             ((uint32_t)0x00000020)
#define XSPI_FLAG_DCOL            ((uint32_t)0x00000040)

extern uint8_t XSPI_FlashREG_WR(uint8_t *cmdbuf,uint8_t *rbuf,uint8_t count,uint8_t type);
extern void XSPI_FlashSendData( uint32_t *pSrcData,  uint32_t *pDstData, uint32_t cnt);
extern void XSPI_FlashWrite( uint32_t PrgCmd,uint32_t addr, uint16_t count,uint32_t *Wbuf);
extern int XSPI_FlashRead(uint32_t PrgCmd, uint32_t  StrAddr, uint32_t Len, uint32_t *DstBuf);




#endif


