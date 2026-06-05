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
*/
#include "n32h47x_48x.h"
#include "main.h"
#include "log.h"

/* n32h47x_48x_StdPeriph_Examples */

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

#define BufferSize 32

const uint16_t I2S_MASTER_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C, 0x0D0E, 0x0F10,
                                                   0x1112, 0x1314, 0x1516, 0x1718, 0x191A, 0x1B1C, 0x1D1E, 0x1F20,
                                                   0x2122, 0x2324, 0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                                   0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C, 0x3D3E, 0x3F40
                                                  };
uint16_t I2S_SLAVE_Buffer_Rx[BufferSize];
volatile TestStatus TransferStatus = FAILED;

void GPIO_Configuration(void);
void DMA_Configuration(void);
void I2S_Configuration(void);
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none
**/
int main(void)
{

    log_init();
    log_info("\r\n This is I2S DMA demo!!\r\n");
    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration();

    /* DMA configuration ------------------------------------------------------*/
    DMA_Configuration();

    /* I2S configuration ------------------------------------------------------*/
    I2S_Configuration();


    /* Enable DMA1 Channel4 */
    DMA_EnableChannel(I2S_SLAVE_Rx_DMA_Channel, ENABLE);

    /* Enable DMA2 Channel2 */
    DMA_EnableChannel(I2S_MASTER_Tx_DMA_Channel, ENABLE);

    /* Wait for DMA2 channel2 transfer complete */
    while (!DMA_GetFlagStatus(I2S_MASTER_Tx_DMA_FLAG, I2S_MASTER_DMA))
        ;

    while (!DMA_GetFlagStatus(I2S_SLAVE_Rx_DMA_FLAG, I2S_SLAVE_DMA))
        ;

    /* Check the correctness of written data */
    TransferStatus = Buffercmp(I2S_SLAVE_Buffer_Rx, (uint16_t*)I2S_MASTER_Buffer_Tx, BufferSize);

    /* TransferStatus = PASSED, if the transmitted and received data
       are equal */
    /* TransferStatus = FAILED, if the transmitted and received data
       are different */
    if(TransferStatus == PASSED)
    {
        log_info("\r\n Test PASS!!\r\n");
    }
    else
    {
        log_info("\r\n Test fail!!\r\n");

    }

    while (1)
    {
    }
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

    /* GPIOC clocks enable */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
    /* Enable peripheral clocks ------------------------------------------------*/
    /* AFIO clocks enable */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure SPI2 pins: CK, WS and SD ---------------------------------*/
    GPIO_InitStructure.Pin            = I2S_SLAVE_PIN_CK;
    GPIO_InitStructure.GPIO_Pull      = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = I2S_SLAVE_AF_CK;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = I2S_SLAVE_PIN_SD;
    GPIO_InitStructure.GPIO_Pull      = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = I2S_SLAVE_AF_SD;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = I2S_SLAVE_PIN_WS;
    GPIO_InitStructure.GPIO_Pull      = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = I2S_SLAVE_AF_WS;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);


    /* Configure SPI3 pins: CK, WS and SD ------------------------------------*/
    GPIO_InitStructure.Pin            = I2S_MASTER_PIN_CK;
    GPIO_InitStructure.GPIO_Alternate = I2S_MASTER_AF_CK;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = I2S_MASTER_PIN_SD;
    GPIO_InitStructure.GPIO_Alternate = I2S_MASTER_AF_SD;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = I2S_MASTER_PIN_WS;
    GPIO_InitStructure.GPIO_Alternate = I2S_MASTER_AF_WS;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
}

/**
*\*\name    DMA_Configuration.
*\*\fun     Configures I2S DMA.
*\*\param   none
*\*\return  none
**/
void DMA_Configuration(void)
{
    DMA_InitType DMA_InitStructure;
    RCC_EnableAHBPeriphClk( I2S_SLAVE_DMA_CLK | I2S_MASTER_DMA_CLK, ENABLE);

    /* I2S_SLAVE_Rx_DMA_Channel configuration ---------------------------------------------*/
    DMA_DeInit(I2S_SLAVE_Rx_DMA_Channel);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&I2S_SLAVE->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)I2S_SLAVE_Buffer_Rx;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = BufferSize;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(I2S_SLAVE_Rx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(I2S_SLAVE_Rx_DMA_Remap, I2S_SLAVE_Rx_DMA_Channel, ENABLE);


    /* I2S_MASTER_Tx_DMA_Channel configuration ---------------------------------------------*/
    DMA_DeInit(I2S_MASTER_Tx_DMA_Channel);
    DMA_InitStructure.PeriphAddr = (uint32_t)&I2S_MASTER->DAT;
    DMA_InitStructure.MemAddr    = (uint32_t)I2S_MASTER_Buffer_Tx;
    DMA_InitStructure.Direction  = DMA_DIR_PERIPH_DST;
    DMA_Init(I2S_MASTER_Tx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(I2S_MASTER_Tx_DMA_Remap, I2S_MASTER_Tx_DMA_Channel, ENABLE);

}

void I2S_Configuration(void)
{
    I2S_InitType I2S_InitStructure;
    RCC_ClocksType RCC_Clocks;

    /* SPI3 and SPI2 clocks enable */
    RCC_EnableAPB1PeriphClk(I2S_SLAVE_CLK | I2S_MASTER_CLK, ENABLE);

    SPI_I2S_DeInit(I2S_SLAVE);
    SPI_I2S_DeInit(I2S_MASTER);

    /* CLK source must be configured before I2S initialization */
    RCC_ConfigI2S2Clk(RCC_I2S2_CLKSRC_SYSCLK);  /* I2S2(I2S_SLAVE) clk source select */
    RCC_ConfigI2S3Clk(RCC_I2S3_CLKSRC_SYSCLK);  /* I2S3(I2S_MASTER) clk source select */

    /* I2S peripheral configuration */
    I2S_InitStruct(&I2S_InitStructure);
    I2S_InitStructure.Standard       = I2S_STD_PHILLIPS;
    I2S_InitStructure.DataFormat     = I2S_DATA_FMT_16BITS_EXTENDED;
    I2S_InitStructure.MCLKEnable     = I2S_MCLK_DISABLE;
    I2S_InitStructure.AudioFrequency = I2S_AUDIO_FREQ_48K;
    I2S_InitStructure.CLKPOL         = I2S_CLKPOL_LOW;

    /* I2S Clock source is System clock: Get System Clock frequency */
    RCC_GetClocksFreqValue(&RCC_Clocks);
    I2S_InitStructure.ClkSrcFrequency = RCC_Clocks.SysclkFreq;

    /* I2S3 Master Transmitter to I2S2 Slave Receiver communication -----------*/
    /* I2S2 configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_SlAVE_RX;
    I2S_Init(SPI2, &I2S_InitStructure);

    /* I2S3 configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_MASTER_TX;
    I2S_Init(SPI3, &I2S_InitStructure);

    /* Enable I2S_SLAVE Rx request */
    SPI_I2S_EnableDma(I2S_SLAVE, SPI_I2S_DMA_RX, ENABLE);
    /* Enable I2S_MASTER Tx request */
    SPI_I2S_EnableDma(I2S_MASTER, SPI_I2S_DMA_TX, ENABLE);

    /* Enable the I2S2 */
    I2S_Enable(I2S_SLAVE, ENABLE);

    /* Enable the I2S3 */
    I2S_Enable(I2S_MASTER, ENABLE);


}
/**
*\*\name    Buffercmp.
*\*\fun     Compares two buffers.
*\*\param   pBuffer1
*\*\param   pBuffer2
*\*\param   BufferLength
*\*\return  FAILED or PASSED
**/
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength)
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

#ifdef USE_FULL_ASSERT

/**
*\*\name    assert_failed.
*\*\fun     Reports the name of the source file and the source line number
*\*\                where the assert_param error has occurred
*\*\param   expr
*\*\param   file        pointer to the source file name
*\*\param   line        assert_param error line source number
*\*\return  none
**/
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}

#endif



