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
*\*\file qspi_cfg.h
*\*\author Nations 
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#ifndef  _XSPI_CFG__
#define  _XSPI_CFG__

#include "main.h"


//#define CLOCK_DIVIDER                                6
#define CTRL1_NDF_CNT                               1024

 
#define STANDRD_SPI_FORMAT               ((uint32_t)0x00000000)
#define DUAL_SPI_FORMAT                  ((uint32_t)0x00400000)
#define QUAD_SPI_FORMAT                  ((uint32_t)0x00800000)
#define OCTAL_SPI_FORMAT                 ((uint32_t)0x00C00000)

#define TX_AND_RX                       ((uint32_t)0x00000000)
#define TX_ONLY                         ((uint32_t)0x00000400)
#define RX_ONLY                         ((uint32_t)0x00000800)
#define EEPROM_READ                     ((uint32_t)0x00000c00)

#define SCPOL_LOW                    ((uint32_t)0x00000000)
#define SCPOL_HIGH                   ((uint32_t)0x00000200)

#define START_BIT                   ((uint32_t)0x00000000)
#define MIDDLE_BIT                  ((uint32_t)0x00000100)

#define SSTE_BIT1                   ((uint32_t)0x00004000)
#define SSTE_BIT0                   ((uint32_t)0x00000000)

#define TRANS_TYPE_STANDRD          ((uint32_t)0x00000000)
#define TRANS_TYPE_STANDRD_FRF      ((uint32_t)0x00000001)
#define TRANS_TYPE_FRF              ((uint32_t)0x00000002)

#define ENSPI_ADDR_NONE          ((uint32_t)0x00000000)
#define ENSPI_ADDR_04BIT         ((uint32_t)0x00000004)
#define ENSPI_ADDR_08BIT         ((uint32_t)0x00000008)
#define ENSPI_ADDR_16BIT         ((uint32_t)0x00000010)
#define ENSPI_ADDR_24BIT         ((uint32_t)0x00000018)
#define ENSPI_ADDR_28BIT         ((uint32_t)0x0000001c)
#define ENSPI_ADDR_32BIT         ((uint32_t)0x00000020)
#define ENSPI_ADDR_40BIT         ((uint32_t)0x00000028)
#define ENSPI_ADDR_48BIT         ((uint32_t)0x00000030)
#define ENSPI_ADDR_52BIT         ((uint32_t)0x00000034)
#define ENSPI_ADDR_56BIT         ((uint32_t)0x00000038)
#define ENSPI_ADDR_60BIT         ((uint32_t)0x0000003C)

#define ENSPI_INST_L_NONE          ((uint32_t)0x00000000)
#define ENSPI_INST_L_04BIT         ((uint32_t)0x00000100)
#define ENSPI_INST_L_08BIT         ((uint32_t)0x00000200)
#define ENSPI_INST_L_16BIT         ((uint32_t)0x00000300)

#define ENSPI_WAIT_8CYCLES	 ((uint32_t)0x00004000) //WAIT_CYCLES
#define ENSPI_WAIT_4CYCLES	 ((uint32_t)0x00002000) 
#define ENSPI_WAIT_2CYCLES	 ((uint32_t)0x00001000)

#define CLK_STRETCH_EN		   ((uint32_t)0x40000000)//Enables clock stretching capability in SPI transfers.
#define CLK_STRETCH_DIS   	   ((uint32_t)0x00000000)//Enables clock stretching capability in SPI transfers.

#define DMA_TX 	0x10
#define DMA_RX  0x01

void RCC_Configuration(void);
void GPIO_Configuration(void);
void XSPI_Configuration(uint32_t frame_format,uint32_t transfer_mode,uint32_t data_frame_size,uint16_t number_data_frame,uint16_t baudr);
void DMA_Configuration(uint32_t PeripheralAddr, uint32_t MemoryAddr, uint16_t Len, uint8_t TxRx, FunctionalState NewState);
#endif
