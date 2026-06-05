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
*\*\file      qspi_cfg.c
*\*\author    Nations
*\*\version   v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved. 
*/
#include "n32h47x_48x_xspi.h" 
#include "xspi_cfg.h"

/**
*\*\name    RCC_Configuration.
*\*\fun     Configures the different system clocks.
*\*\param   none
*\*\return  none
**/
void RCC_Configuration(void)
{
    /* GPIOA, GPIOD clock enable */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD|RCC_AHB_PERIPHEN_GPIOF ,ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
    
    /* XSPI clock enable */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_XSPI, ENABLE);
}


/**
*\*\name    GPIO_Configuration.
*\*\fun     Configures the different GPIO ports.
*\*\param   none
*\*\return  none
**/
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);
  
		
    /* Confugure NSS0 pins as Alternate Function Push Pull */
    GPIO_InitStructure.Pin       = GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SR_SLOW_SLEW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF9;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_12mA;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
			
    /* Confugure SCK\IO0\IO1\IO2\IO3 pin as Input Floating  */
    GPIO_InitStructure.Pin       = GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3| GPIO_PIN_4| GPIO_PIN_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SR_SLOW_SLEW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF9;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_12mA;
    GPIO_InitPeripheral(GPIOF, &GPIO_InitStructure);
    
    /* Confugure IO4\IO5\IO6\IO7 pin as Input Floating  */
    GPIO_InitStructure.Pin       = GPIO_PIN_4 | GPIO_PIN_5| GPIO_PIN_6| GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SR_SLOW_SLEW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF9;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_12mA;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
    
    /* Confugure RXDS pin as Input Floating  */
    GPIO_InitStructure.Pin       = GPIO_PIN_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SR_SLOW_SLEW;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF8;
    GPIO_InitStructure.GPIO_Current = GPIO_DC_12mA;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
	
}

/**
*\*\name    XSPI_Configuration.
*\*\fun     Configures the xspi register.
*\*\param  frame_format:
*\*\        -STANDRD_SPI_FORMAT
*\*\        -DUAL_SPI_FORMAT
*\*\        -QUAD_SPI_FORMAT
*\*\        -OCTAL_SPI_FORMAT
*\*\param  transfer_mode:
*\*\        -TX_AND_RX
*\*\        -TX_ONLY
*\*\        -RX_ONLY
*\*\        -EEPROM_READ
*\*\param  data_frame_size:
*\*\        -DFS_04_BIT
*\*\        -DFS_08_BIT
*\*\        -DFS_16_BIT
*\*\        -DFS_24_BIT
*\*\        -DFS_32_BIT
*\*\param  number_data_frame:
*\*\        -0-0xffff
*\*\param  baudr:
*\*\        -0-0x7fff
*\*\return  none
*\*\note default:INST_8BIT,ADDR_24BIT,waitcycle_8
**/
void XSPI_Configuration(uint32_t frame_format,uint32_t transfer_mode,uint32_t data_frame_size,uint16_t number_data_frame,uint16_t baudr)
{
    XSPI_DeInit();
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_XSPI, ENABLE);
	/*disable XSPI*/
    XSPI->SLAVE_EN = (0x0);
	XSPI_Cmd(DISABLE);
	
	/*set MST, SPI_FRF, TMOD ,SCPOL, SCPH, DFS, SSTE*/
	XSPI->CTRL0 = (XSPI_Mode_Master|frame_format|transfer_mode|SCPOL_LOW|START_BIT|data_frame_size|SSTE_BIT0);
	/*set NDF*/
	XSPI->CTRL1 = (number_data_frame);
	/*set CLKDIV */
	XSPI->BAUD = ((uint32_t)baudr<<1);
    
	/*set TRANSTYPE, ADDRLEN, INSTL, WAITCYCLES*/
	XSPI->ENH_CTRL0 = (TRANS_TYPE_STANDRD|ENSPI_ADDR_24BIT|ENSPI_INST_L_08BIT|ENSPI_WAIT_8CYCLES);
	if(transfer_mode == TX_ONLY || transfer_mode == RX_ONLY)
	{
        XSPI->RX_DELAY = (XSPI->BAUD)/2;
		XSPI->ENH_CTRL0 |=CLK_STRETCH_EN;
	}
	/*set TXFTTEI*/
	XSPI->TXFT = ((0x00<<16)|0x00)&0x1f001f;/* level = 0*/
    /*set TXFTTEI*/
	XSPI->RXFT = (0x08&0x1f);/* level = 8*/
	
	/*enable XSPI*/
    XSPI->SLAVE_EN = (0x1);
	XSPI_Cmd(ENABLE);
}



