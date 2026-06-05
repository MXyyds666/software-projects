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
#include "n32h47x_48x_i2c.h"
#include "main.h"

volatile Status TransferStatus1 = FAILED; 
uint8_t I2C1_Tx_Buffer[BUFFER_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
uint8_t I2C2_Rx_Buffer[BUFFER_SIZE];

void GPIO_Configuration(void);
void I2C_Configuration(void);
void DMA_Configuration(void);
Status Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength);
static void Delay(__IO uint32_t nCount);

int main(void)
{
    /* GPIO configuration */
    GPIO_Configuration();

    /* I2C configuration */
    I2C_Configuration();
    
    /*clear dir buffer*/
    memset(I2C2_Rx_Buffer, 0, BUFFER_SIZE);
    
    /* DMA configuration */
    DMA_Configuration();

    /* Enable I2Cx */
    I2C_Enable(USER1_I2C, ENABLE);
    I2C_Enable(USER2_I2C, ENABLE);

    while (I2C_GetFlag(USER1_I2C, I2C_FLAG_BUSY));
        
    /*----- Transmission Phase -----*/
    /* Send I2C1 START condition */
    I2C_GenerateStart(USER1_I2C, ENABLE);


    /* Test on I2C1 EV5 and clear it */
    while (!I2C_CheckEvent(USER1_I2C, I2C_EVT_MASTER_MODE_FLAG));
    /* Send I2C1 slave Address for write */
    I2C_SendAddr7bit(USER1_I2C, I2C2_ADDRESS7, I2C_DIRECTION_SEND);
    /* Test on I2C2 EV1 and clear it */
    while (!I2C_CheckEvent(USER2_I2C, I2C_EVT_SLAVE_RECV_ADDR_MATCHED));
    /* Test on I2C1 EV6 and clear it */
    while (!I2C_CheckEvent(USER1_I2C, I2C_EVT_MASTER_TXMODE_FLAG));

    /* Enable I2C2 DMA */
    I2C_EnableDMA(USER2_I2C, ENABLE);
    /* Enable I2C1 DMA */
    I2C_EnableDMA(USER1_I2C, ENABLE);

    /* Enable DMA Channelx */
    DMA_EnableChannel(USER1_I2C_TX_DMA_CHANNEL, ENABLE);
    DMA_EnableChannel(USER2_I2C_RX_DMA_CHANNEL, ENABLE);

    /* DMA Channelx transfer complete test */
    while (!DMA_GetFlagStatus(USER1_I2C_TX_DMA_FLAG, USER1_I2C_DMA))
        ;
    /* DMA Channelx transfer complete test */
    while (!DMA_GetFlagStatus(USER2_I2C_RX_DMA_FLAG, USER2_I2C_DMA))
        ;

    /* Send I2C1 STOP Condition */
    I2C_GenerateStop(USER1_I2C, ENABLE);
    /* Test on I2C2 EV4 */
    while (!I2C_CheckEvent(USER2_I2C, I2C_EVT_SLAVE_STOP_RECVD))
        ;
    /* Clear I2C2 STOPF flag: read operation to I2C_STS1 followed by a
    write operation to I2C_CTRL1 */
    (void)(I2C_GetFlag(USER2_I2C, I2C_FLAG_STOPF));
    
    /* Check the correctness of written dada */
    TransferStatus1 = Buffercmp(I2C2_Rx_Buffer, I2C1_Tx_Buffer, BUFFER_SIZE);
    
    GPIO_ResetBits(LED_GPIO, LED_PIN);
    Delay(0x2FFFFF);
    
    while (1)
    {
        /* TransferStatus1 == PASSED, if the data transmitted and
           received are correct */
        /* TransferStatus1 == FAILED, if the data transmitted and
           received are different */
        if (TransferStatus1 != FAILED)
        {
            GPIO_SetBits(LED_GPIO, LED_PIN);
            Delay(0x2FFFFF);
            GPIO_ResetBits(LED_GPIO, LED_PIN);
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
    RCC_EnableAHB1PeriphClk(LED_GPIO_CLK | USER1_I2C_GPIO_CLK | USER2_I2C_GPIO_CLK, ENABLE);

    /* Configure LED pin */
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = LED_PIN;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(LED_GPIO, &GPIO_InitStructure);  

    /* Configure I2C1 pins: SCL and SDA */
    GPIO_InitStructure.Pin            = USER1_I2C_PIN_SCL | USER1_I2C_PIN_SDA;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStructure.GPIO_Alternate = USER1_I2C_PIN_AF;
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitPeripheral(USER1_I2C_GPIO, &GPIO_InitStructure);

    /* Configure I2C2 pins: SCL and SDA */
    GPIO_InitStructure.Pin = USER2_I2C_PIN_SCL | USER2_I2C_PIN_SDA;
    GPIO_InitStructure.GPIO_Alternate = USER2_I2C_PIN_AF;
    GPIO_InitPeripheral(USER2_I2C_GPIO, &GPIO_InitStructure);
}

/**
*\*\name    I2C_Configuration.
*\*\fun     Configure I2C1/I2C2.
*\*\param   none 
*\*\return  none 
**/
void I2C_Configuration(void)
{
    I2C_InitType I2C_InitStructure;
    
    /* Enable I2Cx clock */
    RCC_EnableAPB1PeriphClk(USER1_I2C_CLK | USER2_I2C_CLK, ENABLE);

    Delay(0x1F);  
    I2C_DeInit(USER1_I2C);
    /* I2Cx configuration ------------------------------------------------------*/
    I2C_InitStruct(&I2C_InitStructure);
    I2C_InitStructure.BusMode     = I2C_BUSMODE_I2C;
    I2C_InitStructure.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    I2C_InitStructure.OwnAddr1    = I2C1_ADDRESS7;
    I2C_InitStructure.AckEnable   = I2C_ACKEN;
    I2C_InitStructure.AddrMode    = I2C_ADDR_MODE_7BIT;
    I2C_InitStructure.ClkSpeed    = I2C_CLK_SPEED;
    I2C_Init(USER1_I2C, &I2C_InitStructure);
    
    I2C_DeInit(USER2_I2C);
    I2C_InitStructure.OwnAddr1 = I2C2_ADDRESS7;
    I2C_Init(USER2_I2C, &I2C_InitStructure);
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
    RCC_EnableAHBPeriphClk(USER1_I2C_DMA_CLK | USER2_I2C_DMA_CLK, ENABLE);

    /* DMAx channel x configuration */
    DMA_DeInit(USER1_I2C_TX_DMA_CHANNEL);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&I2C1->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)I2C1_Tx_Buffer;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST; 
    DMA_InitStructure.BufSize        = BUFFER_SIZE;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.MemoryInc     = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_BYTE;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(USER1_I2C_TX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_RequestRemap(DMA_REMAP_I2C1_TX, USER1_I2C_TX_DMA_CHANNEL, ENABLE);

    /* DMAx channel x configuration */
    DMA_DeInit(USER2_I2C_RX_DMA_CHANNEL);
    DMA_InitStructure.PeriphAddr = (uint32_t)&I2C2->DAT;
    DMA_InitStructure.MemAddr    = (uint32_t)I2C2_Rx_Buffer;
    DMA_InitStructure.Direction  = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.Priority   = DMA_PRIORITY_VERY_HIGH;
    DMA_Init(USER2_I2C_RX_DMA_CHANNEL, &DMA_InitStructure);
    DMA_RequestRemap(DMA_REMAP_I2C2_RX, USER2_I2C_RX_DMA_CHANNEL, ENABLE);
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


