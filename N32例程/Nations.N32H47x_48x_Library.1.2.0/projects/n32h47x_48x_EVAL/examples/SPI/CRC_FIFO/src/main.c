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

/* n32h47x_48x_StdPeriph_Examples */


#if defined (N32H475)

    #define SPI_Master SPI3

#else

    #define SPI_Master SPI5

#endif

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

#define BufferSize 32
#define SPICRC

SPI_InitType SPI_InitStructure;
uint16_t SPI_Master_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C, 0x0D0E, 0x0F10,
                                             0x1112, 0x1314, 0x1516, 0x1718, 0x191A, 0x1B1C, 0x1D1E, 0x1F20,
                                             0x2122, 0x2324, 0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                             0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C, 0x3D3E, 0x3F40
                                            };
uint16_t SPI2_Buffer_Tx[BufferSize] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C, 0x5D5E, 0x5F60,
                                       0x6162, 0x6364, 0x6566, 0x6768, 0x696A, 0x6B6C, 0x6D6E, 0x6F70,
                                       0x7172, 0x7374, 0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                                       0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C, 0x8D8E, 0x8F90
                                      };
uint16_t SPI_Master_Buffer_Rx[BufferSize], SPI2_Buffer_Rx[BufferSize];
uint32_t TxIdx = 0, RxIdx = 0;
__IO uint16_t CRC1Value = 0, CRC2Value = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;

void RCC_Configuration(void);
void GPIO_Configuration(void);
TestStatus Buffercmp(uint16_t* pBuffer1, uint16_t* pBuffer2, uint16_t BufferLength);

/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none
**/
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
        this is done through SystemInit() function which is called from startup
        file (startup_n32h4xx.s) before to branch to application main.
        To reconfigure the default setting of SystemInit() function, refer to
        system_n32h47x_48x.c file
      */

    /* System clocks configuration ---------------------------------------------*/
    RCC_Configuration();

    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration();

    SPI_InitStruct(&SPI_InitStructure);
    /* SPI_Master configuration ------------------------------------------------------*/
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_16BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_128;
    SPI_InitStructure.FirstBit      = SPI_FB_MSB;
    SPI_InitStructure.CRCPoly       = 7;
    SPI_Init(SPI_Master, &SPI_InitStructure);

    /* SPI2 configuration ------------------------------------------------------*/
    SPI_InitStructure.SpiMode = SPI_MODE_SLAVE;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Set the FIFO half-full watermark */
    SPI_RxFIFOSizeConfig(SPI_Master, SPI_RX_FIF0NUM_1);
    SPI_TxFIFOSizeConfig(SPI_Master, SPI_TX_FIF0NUM_1);
    /* Enable FIFO */
    SPI_I2S_FIFO_Cmd(SPI_Master, ENABLE);

    /* Set the FIFO half-full watermark */
    SPI_RxFIFOSizeConfig(SPI2, SPI_RX_FIF0NUM_1);
    SPI_TxFIFOSizeConfig(SPI2, SPI_TX_FIF0NUM_1);
    /* Enable FIFO */
    SPI_I2S_FIFO_Cmd(SPI2, ENABLE);

    /* Enable SPI_Master CRC calculation */
    SPI_EnableCalculateCrc(SPI_Master, ENABLE);
    /* Enable SPI2 CRC calculation */
    SPI_EnableCalculateCrc(SPI2, ENABLE);

    /* Enable SPI_Master */
    SPI_Enable(SPI_Master, ENABLE);
    /* Enable SPI2 */
    SPI_Enable(SPI2, ENABLE);

    /* Transfer procedure */
    while (TxIdx < BufferSize - 1)
    {
        /* Wait for SPI_Master Tx buffer empty */
        while (SPI_I2S_GetStatus(SPI_Master, SPI_I2S_TE_FLAG) == RESET)
            ;

        /* Send SPI2 data */
        SPI_I2S_TransmitData(SPI2, SPI2_Buffer_Tx[TxIdx]);
        /* Send SPI_Master data */
        SPI_I2S_TransmitData(SPI_Master, SPI_Master_Buffer_Tx[TxIdx++]);

        /* Wait for SPI2 data reception */
        while (SPI_I2S_GetStatus(SPI2, SPI_I2S_RNE_FLAG) == RESET)
            ;

        /* Read SPI2 received data */
        SPI2_Buffer_Rx[RxIdx] = SPI_RX_FIFO_GET(SPI2);

        /* Wait for SPI_Master data reception */
        while (SPI_I2S_GetStatus(SPI_Master, SPI_I2S_RNE_FLAG) == RESET)
            ;

        /* Read SPI_Master received data */
        SPI_Master_Buffer_Rx[RxIdx++] = SPI_RX_FIFO_GET(SPI_Master);
    }

    /* Wait for SPI_Master Tx buffer empty */
    while (SPI_I2S_GetStatus(SPI_Master, SPI_I2S_TE_FLAG) == RESET)
        ;

    /* Wait for SPI2 Tx buffer empty */
    while (SPI_I2S_GetStatus(SPI2, SPI_I2S_TE_FLAG) == RESET)
        ;

    /* Send last SPI2_Buffer_Tx data */
    SPI_I2S_TransmitData(SPI2, SPI2_Buffer_Tx[TxIdx]);
    /* Enable SPI2 CRC transmission */
    SPI_TransmitCrcNext(SPI2, ENABLE);
    /* Send last SPI_Master_Buffer_Tx data */
    SPI_I2S_TransmitData(SPI_Master, SPI_Master_Buffer_Tx[TxIdx]);
    /* Enable SPI_Master CRC transmission */
    SPI_TransmitCrcNext(SPI_Master, ENABLE);

    /* Wait for SPI_Master last data reception */
    while (SPI_I2S_GetStatus(SPI_Master, SPI_I2S_RNE_FLAG) == RESET)
        ;

    /* Read SPI_Master last received data */
    SPI_Master_Buffer_Rx[RxIdx] = SPI_RX_FIFO_GET(SPI_Master);

    /* Wait for SPI2 last data reception */
    while (SPI_I2S_GetStatus(SPI2, SPI_I2S_RNE_FLAG) == RESET)
        ;

    /* Read SPI2 last received data */
    SPI2_Buffer_Rx[RxIdx] = SPI_RX_FIFO_GET(SPI2);

    /* Wait for SPI_Master data reception: CRC transmitted by SPI2 */
    while (SPI_I2S_GetStatus(SPI_Master, SPI_I2S_RNE_FLAG) == RESET)
        ;

    /* Wait for SPI2 data reception: CRC transmitted by SPI_Master */
    while (SPI_I2S_GetStatus(SPI2, SPI_I2S_RNE_FLAG) == RESET)
        ;

    /* Check the received data with the send ones */
    TransferStatus1 = Buffercmp(SPI2_Buffer_Rx, SPI_Master_Buffer_Tx, BufferSize);
    TransferStatus2 = Buffercmp(SPI_Master_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
    /* TransferStatus1, TransferStatus2 = PASSED, if the data transmitted and received
       are correct */
    /* TransferStatus1, TransferStatus2 = FAILED, if the data transmitted and received
       are different */

    /* Test on the SPI_Master CRC Error flag */
    if ((SPI_I2S_GetStatus(SPI_Master, SPI_CRCERR_FLAG)) == SET)
    {
        TransferStatus2 = FAILED;
    }

    /* Test on the SPI2 CRC Error flag */
    if ((SPI_I2S_GetStatus(SPI2, SPI_CRCERR_FLAG)) == SET)
    {
        TransferStatus1 = FAILED;
    }

    /* Read SPI_Master received CRC value */
    CRC1Value = SPI_RX_FIFO_GET(SPI_Master);
    /* Read SPI2 received CRC value */
    CRC2Value = SPI_RX_FIFO_GET(SPI2);

    while (1)
    {
    }
}

/**
*\*\name    RCC_Configuration.
*\*\fun     Configures the different system clocks.
*\*\param   none
*\*\return  none
**/
void RCC_Configuration(void)
{
    /* Enable peripheral clocks --------------------------------------------------*/
    /* GPIOC, GPIOD, GPIOF clock enable */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC | RCC_AHB_PERIPHEN_GPIOD | RCC_AHB_PERIPHEN_GPIOF, ENABLE);

    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

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

    #if defined (N32H475)

    /* SPI3 and SPI2 Periph clock enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI3 | RCC_APB1_PERIPH_SPI2, ENABLE);

    /* Configure SPI3 pins: SCK��MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.Pin        = GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    /* Confugure MOSI pin as Alternate Function Push Pull */
    GPIO_InitStructure.Pin        = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    /* Confugure MISO pin as Input Floating  */
    GPIO_InitStructure.Pin       = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);

    /* Configure SPI2 pins: SCK��MOSI and MISO*/
    GPIO_InitStructure.Pin       = GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF13;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    /* Confugure MISO pin as Alternate Function Push Pull */
    GPIO_InitStructure.Pin       = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    /* Confugure MOSI pins as Input Floating */
    GPIO_InitStructure.Pin       = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF11;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);

    #else
    /* SPI_Master clock enable */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI5, ENABLE);
    /* SPI2 Periph clock enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI2, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);
    /* Configure SPI5 pins: SCK, MISO and MOSI ---------------------------------*/
    /* Confugure SCK and MOSI pins as Alternate Function Push Pull */
    GPIO_InitStructure.Pin        = GPIO_PIN_7 | GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF11;
    GPIO_InitPeripheral(GPIOF, &GPIO_InitStructure);
    /* Confugure MISO pin as Input Floating  */
    GPIO_InitStructure.Pin       = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF11;
    GPIO_InitPeripheral(GPIOF, &GPIO_InitStructure);

    /* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/
    /* Confugure SCK pins as Input Floating */
    GPIO_InitStructure.Pin       = GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF13;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    /* Confugure MOSI pins as Input Floating */
    GPIO_InitStructure.Pin       = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF11;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
    /* Confugure MISO pin as Alternate Function Push Pull */
    GPIO_InitStructure.Pin       = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
    #endif
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


