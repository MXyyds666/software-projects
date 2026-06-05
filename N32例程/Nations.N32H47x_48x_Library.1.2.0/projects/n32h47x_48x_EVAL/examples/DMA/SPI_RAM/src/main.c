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
 */
#include <string.h>
#include "n32h47x_48x.h"
#include "misc.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_spi.h"
#include "main.h"

uint8_t SPI_Master_Rx_Buffer[BUFFER_SIZE], SPI_Slave_Rx_Buffer[BUFFER_SIZE];
volatile Status TransferStatus1 = FAILED, TransferStatus2 = FAILED; 

uint8_t SPI_Master_Tx_Buffer[BUFFER_SIZE] = {0xf1, 0x2e, 0xf1, 0xaf, 0x08, 0xa2, 0x41, 0xb4, 0xc8, 0x19, 0x38,
                                             0xb7, 0xd9, 0xdb, 0x9a, 0x64, 0x1f, 0xd6, 0x99, 0x9c, 0x0f, 0xae,
                                             0x84, 0xab, 0xda, 0x12, 0x95, 0x6c, 0xdb, 0xec, 0x06, 0x08};

uint8_t SPI_Slave_Tx_Buffer[BUFFER_SIZE] = {0x2c, 0x99, 0xd7, 0x26, 0xb0, 0xe5, 0xb2, 0xfc, 0xee, 0x88, 0x3f,
                                            0xde, 0xa4, 0x37, 0x87, 0xc9, 0xb2, 0x9c, 0xce, 0xc8, 0x2c, 0x22,
                                            0x6b, 0xfe, 0xba, 0x49, 0x94, 0x0a, 0x47, 0x5a, 0xb7, 0x89};

void GPIO_Configuration(void);
void SPI_Configuration(void);
void DMA_Configuration(void);
Status Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength);
static void Delay(__IO uint32_t nCount);

int main(void)
{
    /* GPIO configuration */
    GPIO_Configuration();

    /* SPI configuration */
    SPI_Configuration();
    
    /*clear dir buffer*/
    memset(SPI_Master_Rx_Buffer, 0, BUFFER_SIZE);
    memset(SPI_Slave_Rx_Buffer, 0, BUFFER_SIZE);
    
    /* DMA configuration */
    DMA_Configuration();

    /* Enable SPI_MASTER DMA Tx request */
    SPI_I2S_EnableDma(SPI_MASTER, SPI_I2S_DMA_TX, ENABLE);
    /* Enable SPI_MASTER DMA Rx request */
    SPI_I2S_EnableDma(SPI_MASTER, SPI_I2S_DMA_RX, ENABLE);
    /* Enable SPI_SLAVE DMA Tx request */
    SPI_I2S_EnableDma(SPI_SLAVE, SPI_I2S_DMA_TX, ENABLE);
    /* Enable SPI_SLAVE DMA Rx request */
    SPI_I2S_EnableDma(SPI_SLAVE, SPI_I2S_DMA_RX, ENABLE);

    /* Enable SPI_MASTER CRC calculation */
    SPI_EnableCalculateCrc(SPI_MASTER, ENABLE);
    /* Enable SPI_SLAVE CRC calculation */
    SPI_EnableCalculateCrc(SPI_SLAVE, ENABLE);

    /* Enable SPI_SLAVE */
    SPI_Enable(SPI_SLAVE, ENABLE);
    /* Enable SPI_MASTER */
    SPI_Enable(SPI_MASTER, ENABLE);

    /* Enable DMAx channels x*/
    DMA_EnableChannel(SPI_MASTER_RX_DMA_CHANNEL, ENABLE);
    DMA_EnableChannel(SPI_SLAVE_RX_DMA_CHANNEL, ENABLE);
    DMA_EnableChannel(SPI_SLAVE_TX_DMA_CHANNEL, ENABLE);
    DMA_EnableChannel(SPI_MASTER_TX_DMA_CHANNEL, ENABLE);

    /* Transfer complete */
    while (!DMA_GetFlagStatus(SPI_MASTER_RX_DMA_FLAG, SPI_MASTER_DMA))
        ;
    while (!DMA_GetFlagStatus(SPI_SLAVE_RX_DMA_FLAG, SPI_SLAVE_DMA))
        ;
    while (!DMA_GetFlagStatus(SPI_SLAVE_TX_DMA_FLAG, SPI_SLAVE_DMA))
        ;
    while (!DMA_GetFlagStatus(SPI_MASTER_TX_DMA_FLAG, SPI_MASTER_DMA))
        ;


    /* Wait for SPI_MASTER data reception: CRC transmitted by SPI_SLAVE */
    while (SPI_I2S_GetStatus(SPI_MASTER, SPI_I2S_RNE_FLAG) == RESET)
        ;
    /* Wait for SPI_SLAVE data reception: CRC transmitted by SPI_MASTER */
    while (SPI_I2S_GetStatus(SPI_SLAVE, SPI_I2S_RNE_FLAG) == RESET)
        ;
    
    /* Check the correctness of written dada */
    TransferStatus1 = Buffercmp(SPI_Slave_Rx_Buffer, SPI_Master_Tx_Buffer, BUFFER_SIZE);
    TransferStatus2 = Buffercmp(SPI_Master_Rx_Buffer, SPI_Slave_Tx_Buffer, BUFFER_SIZE);

    /* Test on the SPI_MASTER CRCR ERROR flag */
    if ((SPI_I2S_GetStatus(SPI_MASTER, SPI_CRCERR_FLAG)) != RESET)
    {
        TransferStatus1 = FAILED;
    }
    /* Test on the SPI_SLAVE CRCR ERROR flag */
    if ((SPI_I2S_GetStatus(SPI_SLAVE, SPI_CRCERR_FLAG)) != RESET)
    {
        TransferStatus2 = FAILED;
    }
    
    GPIO_ResetBits(LED_GPIO, LED_PIN_1);
    GPIO_ResetBits(LED_GPIO, LED_PIN_2);
    Delay(0x2FFFFF);
    
    while (1)
    {
        /* TransferStatus1, TransferStatus2 == PASSED, if the data transmitted and
           received are correct */
        /* TransferStatus1, TransferStatus2 == FAILED, if the data transmitted and
           received are different */
        if (TransferStatus1 != FAILED)
        {
            GPIO_SetBits(LED_GPIO, LED_PIN_1);
            Delay(0x2FFFFF);
            GPIO_ResetBits(LED_GPIO, LED_PIN_1);
            Delay(0x2FFFFF);
        }

        if (TransferStatus2 != FAILED)
        {
            GPIO_SetBits(LED_GPIO, LED_PIN_2);
            Delay(0x2FFFFF);
            GPIO_ResetBits(LED_GPIO, LED_PIN_2);
            Delay(0x2FFFFF);
        }
    }
}

/**
*\*\name    GPIO_Configuration.
*\*\fun     GPIO init.
*\*\param   none 
*\*\return  none 
**/
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(LED_GPIO_CLK | SPI_MASTER_GPIO_CLK | SPI_SLAVE_GPIO_CLK, ENABLE);

    /* Configure LED pin */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = LED_PIN_1 | LED_PIN_2;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(LED_GPIO, &GPIO_InitStructure);  

    /* Configure SPI_MASTER pins: SCK and MOSI */
    GPIO_InitStructure.Pin        = SPI_MASTER_PIN_SCK | SPI_MASTER_PIN_MOSI;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = SPI_MASTER_PIN_AF;
    GPIO_InitPeripheral(SPI_MASTER_GPIO, &GPIO_InitStructure);
    
    /* Configure SPI_MASTER pins:  MISO */
    GPIO_InitStructure.Pin       = SPI_MASTER_PIN_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Alternate = SPI_MASTER_PIN_AF;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(SPI_MASTER_GPIO, &GPIO_InitStructure);

    /* Configure SPI_SLAVE pins: SCK */
    GPIO_InitStructure.Pin       = SPI_SLAVE_PIN_SCK;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Alternate = SPI_SLAVE_SCK_PIN_AF;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(SPI_SLAVE_GPIO, &GPIO_InitStructure);
    
    /* Configure SPI_SLAVE pins: MOSI */
    GPIO_InitStructure.Pin       = SPI_SLAVE_PIN_MOSI;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Alternate = SPI_SLAVE_PIN_AF;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(SPI_SLAVE_GPIO, &GPIO_InitStructure);
    
    /* Configure SPI_SLAVE pins: MISO */
    GPIO_InitStructure.Pin       = SPI_SLAVE_PIN_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = SPI_SLAVE_PIN_AF;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(SPI_SLAVE_GPIO, &GPIO_InitStructure);
}

/**
*\*\name    SPI_Configuration.
*\*\fun     Configure SPI1/SPI2.
*\*\param   none 
*\*\return  none 
**/
void SPI_Configuration(void)
{
    SPI_InitType SPI_InitStructure;
    
    /* Enable SPI_MASTER and SPI_SLAVE clk*/
    SPI_MASTER_CLOCK_ENABLE(SPI_MASTER_CLK, ENABLE);
    SPI_SLAVE_CLOCK_ENABLE(SPI_SLAVE_CLK, ENABLE);

    SPI_I2S_DeInit(SPI_MASTER);
    SPI_InitStruct(&SPI_InitStructure);
    /* SPI_MASTER configuration */
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_8;
    SPI_InitStructure.FirstBit      = SPI_FB_MSB;
    SPI_InitStructure.CRCPoly       = CRC_POLYNOMIAL;
    SPI_Init(SPI_MASTER, &SPI_InitStructure);
    
    SPI_I2S_DeInit(SPI_SLAVE);
    /* SPI_SLAVE configuration */
    SPI_InitStructure.SpiMode = SPI_MODE_SLAVE;
    SPI_Init(SPI_SLAVE, &SPI_InitStructure);
}

/**
*\*\name    DMA_Configuration.
*\*\fun     Configure the DMA controller.
*\*\param   none 
*\*\return  none 
**/
void DMA_Configuration(void)
{
    DMA_InitType DMA_InitStructure;
    
    /* Enable DMAx clock */
    RCC_EnableAHBPeriphClk(SPI_MASTER_DMA_CLK | SPI_SLAVE_DMA_CLK, ENABLE);

    /* DMA1 channel x configuration */
    DMA_DeInit(SPI_MASTER_RX_DMA_CHANNEL);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&SPI_MASTER->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)SPI_Master_Rx_Buffer;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC; 
    DMA_InitStructure.BufSize        = BUFFER_SIZE;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(SPI_MASTER_RX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_RequestRemap(SPI_MASTER_RX_DMA_REMP, SPI_MASTER_RX_DMA_CHANNEL, ENABLE);

    /* DMA1 channel x configuration */
    DMA_DeInit(SPI_MASTER_TX_DMA_CHANNEL);
    DMA_InitStructure.PeriphAddr = (uint32_t)&SPI_MASTER->DAT;
    DMA_InitStructure.MemAddr    = (uint32_t)SPI_Master_Tx_Buffer;
    DMA_InitStructure.Direction  = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.Priority   = DMA_PRIORITY_LOW;
    DMA_Init(SPI_MASTER_TX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_RequestRemap(SPI_MASTER_TX_DMA_REMP, SPI_MASTER_TX_DMA_CHANNEL, ENABLE);

    /* DMA1 channel x configuration */
    DMA_DeInit(SPI_SLAVE_RX_DMA_CHANNEL);
    DMA_InitStructure.PeriphAddr = (uint32_t)&SPI_SLAVE->DAT;
    DMA_InitStructure.MemAddr    = (uint32_t)SPI_Slave_Rx_Buffer;
    DMA_InitStructure.Direction  = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.Priority   = DMA_PRIORITY_VERY_HIGH;
    DMA_Init(SPI_SLAVE_RX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_RequestRemap(SPI_SLAVE_RX_DMA_REMP, SPI_SLAVE_RX_DMA_CHANNEL, ENABLE);

    /* DMA1 channel x configuration */
    DMA_DeInit(SPI_SLAVE_TX_DMA_CHANNEL);
    DMA_InitStructure.PeriphAddr = (uint32_t)&SPI_SLAVE->DAT;
    DMA_InitStructure.MemAddr    = (uint32_t)SPI_Slave_Tx_Buffer;
    DMA_InitStructure.Direction  = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.Priority   = DMA_PRIORITY_MEDIUM;
    DMA_Init(SPI_SLAVE_TX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_RequestRemap(SPI_SLAVE_TX_DMA_REMP, SPI_SLAVE_TX_DMA_CHANNEL, ENABLE);
}

/**
*\*\name    Buffercmp.
*\*\fun     This function handles is DMA interrupt request.
*\*\param   none 
*\*\return  none 
**/
Status Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer != *pBuffer1)
        {
            return FAILED;
        }

        pBuffer++;
        pBuffer1++;
    }

    return PASSED;
}

static void Delay(__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}


