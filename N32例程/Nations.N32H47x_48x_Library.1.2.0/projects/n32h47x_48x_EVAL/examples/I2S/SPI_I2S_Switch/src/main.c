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
#include "n32h47x_48x_spi.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"
#include "n32h47x_48x_dma.h"
#include "main.h"
#include "misc.h"
#include "log.h"

/** n32h47x_48x_StdPeriph_Examples **/

/** SPI_I2S_Switch **/

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

#define BufferSize 32

I2S_InitType I2S_InitStructure;

uint16_t I2S3_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C, 0x0D0E, 0x0F10,
                                       0x1112, 0x1314, 0x1516, 0x1718, 0x191A, 0x1B1C, 0x1D1E, 0x1F20,
                                       0x2122, 0x2324, 0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                       0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C, 0x3D3E, 0x3F40
                                      };

uint16_t SPI3_Buffer_Tx[BufferSize] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C, 0x5D5E, 0x5F60,
                                       0x6162, 0x6364, 0x6566, 0x6768, 0x696A, 0x6B6C, 0x6D6E, 0x6F70,
                                       0x7172, 0x7374, 0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                                       0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C, 0x8D8E, 0x8F90
                                      };

__IO uint16_t I2S2_Buffer_Rx[BufferSize];
__IO uint16_t SPI2_Buffer_Rx[BufferSize];
__IO uint8_t TxIdx = 0, RxIdx = 0;
TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED, TransferStatus3 = FAILED;

void GPIO_Configuration(void);
void SPI_Configuration(void);
void I2S_Configuration_1(void);
void I2S_Configuration_2(void);
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);
void I2S3_TransmitData_funtion(void);
void I2S2_TransmitData_funtion(void);
void SPI_TransmitData_funtion(void);

/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none
**/
int main(void)
{

    /* SPI3 and SPI2 clocks enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI3 | RCC_APB1_PERIPH_SPI2, ENABLE);

    log_init();
    log_info("\r\n This is SPI I2S switch demo!!\r\n");
    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration();

    I2S3_TransmitData_funtion();

    /* Reset TxIdx, RxIdx indexes */
    TxIdx = 0;
    RxIdx = 0;

    SPI_TransmitData_funtion();

    /* Reset TxIdx, RxIdx indexes and receive table values */
    for (TxIdx = 0; TxIdx < BufferSize; TxIdx++)
    {
        I2S2_Buffer_Rx[TxIdx] = 0;
    }

    TxIdx = 0;
    RxIdx = 0;

    I2S2_TransmitData_funtion();

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
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = I2S_SLAVE_PIN_SD;
    GPIO_InitStructure.GPIO_Pull      = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = I2S_SLAVE_AF_SD;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = I2S_SLAVE_PIN_WS;
    GPIO_InitStructure.GPIO_Pull      = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = I2S_SLAVE_AF_WS;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
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

void I2S_Configuration_1(void)
{
    RCC_ClocksType RCC_Clocks;

    SPI_I2S_DeInit(SPI3);
    SPI_I2S_DeInit(SPI2);

    /* CLK source must be configured before I2S initialization */
    RCC_ConfigI2S2Clk(RCC_I2S2_CLKSRC_SYSCLK);  /* I2S2 clk source select */
    RCC_ConfigI2S3Clk(RCC_I2S3_CLKSRC_SYSCLK);  /* I2S3 clk source select */

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
    /* I2S3 configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_MASTER_TX;
    I2S_Init(SPI3, &I2S_InitStructure);

    /* I2S2 configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_SlAVE_RX;
    I2S_Init(SPI2, &I2S_InitStructure);

    /* Enable the I2S3 */
    I2S_Enable(SPI3, ENABLE);
    /* Enable the I2S2 */
    I2S_Enable(SPI2, ENABLE);

}

void I2S_Configuration_2(void)
{
    RCC_ClocksType RCC_Clocks;

    SPI_I2S_DeInit(SPI3);
    SPI_I2S_DeInit(SPI2);

    /* CLK source must be configured before I2S initialization */
    RCC_ConfigI2S2Clk(RCC_I2S2_CLKSRC_SYSCLK);  /* I2S2 clk source select */
    RCC_ConfigI2S3Clk(RCC_I2S3_CLKSRC_SYSCLK);  /* I2S3 clk source select */

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
    /* I2S3 configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_SlAVE_TX;
    I2S_Init(SPI3, &I2S_InitStructure);

    /* I2S2 configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_MASTER_RX;
    I2S_Init(SPI2, &I2S_InitStructure);

    /* Enable the I2S3 */
    I2S_Enable(SPI3, ENABLE);
    /* Enable the I2S2 */
    I2S_Enable(SPI2, ENABLE);

}

void SPI_Configuration(void)
{
    SPI_InitType SPI_InitStructure;
    /* Switch to SPI mode communication ----------------------------------------*/
    /* SPI3 configuration */
    SPI_I2S_DeInit(SPI3);
    SPI_I2S_DeInit(SPI2);

    SPI_InitStruct(&SPI_InitStructure);
    SPI_InitStructure.DataDirection = SPI_DIR_SINGLELINE_TX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_16BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_4;
    SPI_InitStructure.FirstBit      = SPI_FB_MSB;
    SPI_InitStructure.CRCPoly       = 7;
    SPI_Init(SPI3, &SPI_InitStructure);

    /* SPI2 configuration ------------------------------------------------------*/
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_RONLY;
    SPI_InitStructure.SpiMode       = SPI_MODE_SLAVE;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI2 */
    SPI_Enable(SPI2, ENABLE);
    /* Enable SPI3 */
    SPI_Enable(SPI3, ENABLE);

}

void I2S3_TransmitData_funtion(void)
{

    /* I2S configuration ------------------------------------------------------*/
    I2S_Configuration_1();

    while (RxIdx < BufferSize)
    {
        /* Wait the Tx buffer to be empty */
        while (SPI_I2S_GetStatus(SPI3, SPI_I2S_TE_FLAG) == RESET)
        {
        }

        /* Send a data from I2S3 */
        SPI_I2S_TransmitData(SPI3, I2S3_Buffer_Tx[TxIdx++]);

        /* Wait the Rx buffer to be full */
        while (SPI_I2S_GetStatus(SPI2, SPI_I2S_RNE_FLAG) == RESET)
        {
        }

        /* Store the I2S2 received data in the relative data table */
        I2S2_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
    }

    TransferStatus1 = Buffercmp((uint16_t*)I2S2_Buffer_Rx, I2S3_Buffer_Tx, BufferSize);
    /* TransferStatus1 = PASSED, if the data transmitted from I2S3 and received by
                                 I2S2 are the same
       TransferStatus1 = FAILED, if the data transmitted from I2S3 and received by
                                 I2S2 are different */

    /* Disable the I2S3 */
    I2S_Enable(SPI3, DISABLE);

    /* Disable the I2S2 */
    I2S_Enable(SPI2, DISABLE);

    if(TransferStatus1 == PASSED)
    {
        log_info("\r\n 1 Test PASS!!\r\n");
    }
    else
    {
        log_info("\r\n 1 Test fail!!\r\n");

    }

}

void I2S2_TransmitData_funtion(void)
{
    I2S_Configuration_2();

    /* Begin the communication in I2S mode */
    while (RxIdx < BufferSize)
    {
        /* Wait the Tx buffer to be empty */
        while (SPI_I2S_GetStatus(SPI3, SPI_I2S_TE_FLAG) == RESET)
        {
        }

        /* Send a data from I2S3 */
        SPI_I2S_TransmitData(SPI3, I2S3_Buffer_Tx[TxIdx++]);

        /* Wait the Rx buffer to be full */
        while (SPI_I2S_GetStatus(SPI2, SPI_I2S_RNE_FLAG) == RESET)
        {
        }

        /* Store the I2S2 received data in the relative data table */
        I2S2_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
    }

    TransferStatus3 = Buffercmp((uint16_t*)I2S2_Buffer_Rx, I2S3_Buffer_Tx, BufferSize);
    /* TransferStatus3 = PASSED, if the data transmitted from I2S2 and received by
                                 I2S3 are the same
       TransferStatus3 = FAILED, if the data transmitted from I2S2 and received by
                                 I2S3 are different */

    /* Disable the I2S3 */
    I2S_Enable(SPI3, DISABLE);

    /* Disable the I2S2 */
    I2S_Enable(SPI2, DISABLE);

    if(TransferStatus3 == PASSED)
    {
        log_info("\r\n 3 Test PASS!!\r\n");
    }
    else
    {
        log_info("\r\n 3 Test fail!!\r\n");

    }

}

void SPI_TransmitData_funtion(void)
{
    /* SPI configuration ------------------------------------------------------*/
    SPI_Configuration();

    /* Begin the communication in SPI mode */
    while (RxIdx < BufferSize)
    {
        /* Wait the Tx buffer to be empty */
        while (SPI_I2S_GetStatus(SPI3, SPI_I2S_TE_FLAG) == RESET)
        {
        }

        /* Send a data from SPI3 */
        SPI_I2S_TransmitData(SPI3, SPI3_Buffer_Tx[TxIdx++]);

        /* Wait the Rx buffer to be full */
        while (SPI_I2S_GetStatus(SPI2, SPI_I2S_RNE_FLAG) == RESET)
        {
        }

        /* Store the SPI2 received data in the relative data table */
        SPI2_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
    }

    TransferStatus2 = Buffercmp((uint16_t*)SPI2_Buffer_Rx, SPI3_Buffer_Tx, BufferSize);
    /* TransferStatus2 = PASSED, if the data transmitted from SPI3 and received by
                                 SPI2 are the same
       TransferStatus2 = FAILED, if the data transmitted from SPI3 and received by
                                 SPI2 are different */

    /* Disable the SPI3 */
    SPI_Enable(SPI3, DISABLE);

    /* Disable the SPI2 */
    SPI_Enable(SPI2, DISABLE);


    if(TransferStatus2 == PASSED)
    {
        log_info("\r\n 2 Test PASS!!\r\n");
    }
    else
    {
        log_info("\r\n 2 Test fail!!\r\n");

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
*\*\        where the assert_param error has occurred
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



