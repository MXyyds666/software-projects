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

/** n32h47x_48x_StdPeriph_Examples **/

/** FullDuplex_SoftNSS **/

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

#define SPI_SEND_COUNT 256
#define BufferSize 32

#if defined (N32H475)
    #define SPI_Master SPI3
#else
    #define SPI_Master SPI5
#endif

SPI_InitType SPI_InitStructure;
uint8_t SPI_Master_Buffer_Tx[BufferSize] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
                                            0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                                            0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
                                           };
uint8_t SPI2_Buffer_Tx[BufferSize] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B,
                                      0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
                                      0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70
                                     };
uint8_t SPI_Master_Buffer_Rx[BufferSize], SPI2_Buffer_Rx[BufferSize];
__IO uint8_t TxIdx = 0, RxIdx = 0, k = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile TestStatus TransferStatus3 = FAILED, TransferStatus4 = FAILED;

void RCC_Configuration(void);
void GPIO_Configuration(uint16_t SPI_Master_Mode, uint16_t SPI2_Mode);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

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
         file (startup_n32h47x.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_n32h47x.c file
       */

    /* System clocks configuration ---------------------------------------------*/
    RCC_Configuration();

    /* 1st phase: SPI_Master Master and SPI2 Slave */
    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration(SPI_MODE_MASTER, SPI_MODE_SLAVE);

    SPI_InitStruct(&SPI_InitStructure);
    /* SPI_Master Config -------------------------------------------------------------*/
    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_8;
    SPI_InitStructure.FirstBit      = SPI_FB_LSB;
    SPI_InitStructure.CRCPoly       = 7;
    SPI_Init(SPI_Master, &SPI_InitStructure);

    /* SPI2 Config -------------------------------------------------------------*/
    SPI_InitStructure.SpiMode = SPI_MODE_SLAVE;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI_Master */
    SPI_Enable(SPI_Master, ENABLE);
    /* Enable SPI2 */
    SPI_Enable(SPI2, ENABLE);

    /* Transfer procedure */
    while (TxIdx < BufferSize)
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
        SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);

        /* Wait for SPI_Master data reception */
        while (SPI_I2S_GetStatus(SPI_Master, SPI_I2S_RNE_FLAG) == RESET)
            ;

        /* Read SPI_Master received data */
        SPI_Master_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI_Master);
    }

    /* Check the correctness of written dada */
    TransferStatus1 = Buffercmp(SPI2_Buffer_Rx, SPI_Master_Buffer_Tx, BufferSize);
    TransferStatus2 = Buffercmp(SPI_Master_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
    /* TransferStatus1, TransferStatus2 = PASSED, if the transmitted and received data
       are equal */
    /* TransferStatus1, TransferStatus2 = FAILED, if the transmitted and received data
       are different */

    /* 2nd phase: SPI_Master Slave and SPI2 Master */
    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration(SPI_MODE_SLAVE, SPI_MODE_MASTER);

    SPI_I2S_DeInit(SPI_Master);
    SPI_I2S_DeInit(SPI2);

    SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_SLAVE;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_LOW;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_FIRST_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_8;
    SPI_InitStructure.FirstBit      = SPI_FB_LSB;
    SPI_InitStructure.CRCPoly       = 7;
    SPI_Init(SPI_Master, &SPI_InitStructure);

    /* SPI2 Re-configuration ---------------------------------------------------*/
    SPI_InitStructure.SpiMode = SPI_MODE_MASTER;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI2 */
    SPI_Enable(SPI2, ENABLE);
    /* Enable SPI_Master */
    SPI_Enable(SPI_Master, ENABLE);

    /* Reset TxIdx, RxIdx indexes and receive tables values */
    TxIdx = 0;
    RxIdx = 0;

    for (k = 0; k < BufferSize; k++)
        SPI2_Buffer_Rx[k] = 0;

    for (k = 0; k < BufferSize; k++)
        SPI_Master_Buffer_Rx[k] = 0;

    /* Transfer procedure */
    while (TxIdx < BufferSize)
    {
        /* Wait for SPI2 Tx buffer empty */
        while (SPI_I2S_GetStatus(SPI2, SPI_I2S_TE_FLAG) == RESET)
            ;

        /* Send SPI_Master data */
        SPI_I2S_TransmitData(SPI_Master, SPI_Master_Buffer_Tx[TxIdx]);
        /* Send SPI2 data */
        SPI_I2S_TransmitData(SPI2, SPI2_Buffer_Tx[TxIdx++]);

        /* Wait for SPI_Master data reception */
        while (SPI_I2S_GetStatus(SPI_Master, SPI_I2S_RNE_FLAG) == RESET)
            ;

        /* Read SPI_Master received data */
        SPI_Master_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI_Master);

        /* Wait for SPI2 data reception */
        while (SPI_I2S_GetStatus(SPI2, SPI_I2S_RNE_FLAG) == RESET)
            ;

        /* Read SPI2 received data */
        SPI2_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
    }

    /* Check the correctness of written dada */
    TransferStatus3 = Buffercmp(SPI2_Buffer_Rx, SPI_Master_Buffer_Tx, BufferSize);
    TransferStatus4 = Buffercmp(SPI_Master_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
    /* TransferStatus3, TransferStatus4 = PASSED, if the transmitted and received data
       are equal */
    /* TransferStatus3, TransferStatus4 = FAILED, if the transmitted and received data
       are different */

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
    /* AFIO clock enable */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
}


/**
*\*\name    GPIO_Configuration.
*\*\fun     Configures the different GPIO ports.
*\*\param   SPI_Master_Mode Specifies the SPI_Master operating mode.
*\*\				 - SPI_Master_Mode_Master
*\*\				 - SPI_Master_Mode_Slave
*\*\param   SPI2_Mode Specifies the SPI2 operating mode.
*\*\				 - SPI2_Mode_Master
*\*\				 - SPI2_Mode_Slave
*\*\return  none
**/
void GPIO_Configuration(uint16_t SPI_Master_Mode, uint16_t SPI2_Mode)
{
    GPIO_InitType GPIO_InitStructure;

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);

    #if defined (N32H475)

    /* SPI3 and SPI2 Periph clock enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI3 | RCC_APB1_PERIPH_SPI2, ENABLE);

    /* Configure SPI3 pins: SCK, MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.Pin       = GPIO_PIN_3;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF2;

    if (SPI_Master_Mode == SPI_MODE_MASTER)
    {
        /* Configure SCK pins as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }
    else
    {
        /* Configure SCK pins as Input  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }

    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin       = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;

    if (SPI_Master_Mode == SPI_MODE_MASTER)
    {
        /* Configure MOSI pins as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }
    else
    {
        /* Configure MOSI pins as Input  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }

    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;

    if (SPI_Master_Mode == SPI_MODE_MASTER)
    {
        /* Configure MISO pin as Input  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }
    else
    {
        /* Configure MISO pin as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }

    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);

    #else
    /* SPI_Master clock enable */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI5, ENABLE);

    /* SPI2 Periph clock enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_SPI2, ENABLE);
    /* Configure SPI3 pins: SCK, MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.Pin       = GPIO_PIN_7 | GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF11;

    if (SPI_Master_Mode == SPI_MODE_MASTER)
    {
        /* Configure SCK and MOSI pins as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }
    else
    {
        /* Configure SCK and MOSI pins as Input  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }

    GPIO_InitPeripheral(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF11;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;

    if (SPI_Master_Mode == SPI_MODE_MASTER)
    {
        /* Configure MISO pin as Input  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }
    else
    {
        /* Configure MISO pin as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }

    GPIO_InitPeripheral(GPIOF, &GPIO_InitStructure);

    #endif
    /* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.Pin = GPIO_PIN_7 ;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF13;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;

    if (SPI2_Mode == SPI_MODE_SLAVE)
    {
        /* Configure SCK pins as Input  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }
    else
    {
        /* Configure SCK pins as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }

    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF11;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;

    if (SPI2_Mode == SPI_MODE_SLAVE)
    {
        /* Configure MOSI pins as Input  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }
    else
    {
        /* Configure MOSI pins as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }

    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);


    GPIO_InitStructure.Pin = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1;

    if (SPI2_Mode == SPI_MODE_SLAVE)
    {
        /* Configure MISO pin as Alternate Function Push Pull */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    }
    else
    {
        /* Configure MISO pin as Input Floating  */
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
    }

    GPIO_InitPeripheral(GPIOC, &GPIO_InitStructure);
}
/**
*\*\name    Buffercmp.
*\*\fun     Compares two buffers.
*\*\param   pBuffer1
*\*\param   pBuffer2
*\*\param   BufferLength
*\*\return  FAILED or PASSED
**/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
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




