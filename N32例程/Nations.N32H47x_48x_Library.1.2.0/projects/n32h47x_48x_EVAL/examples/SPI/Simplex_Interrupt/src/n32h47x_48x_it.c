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
*\*\file n32h47x_48x_it.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "n32h47x_48x_it.h"
#include "main.h"

/** N32H475_StdPeriph_Template **/

#define BufferSize 32

extern __IO uint8_t TxIdx, RxIdx;
extern uint8_t SPI_Master_Buffer_Tx[BufferSize], SPI2_Buffer_Rx[BufferSize];

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
/** This function handles NMI exception. **/
void NMI_Handler(void)
{
}

/** This function handles Hard Fault exception. **/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/** This function handles Memory Manage exception. **/
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/** This function handles Bus Fault exception. **/
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/** This function handles Usage Fault exception. **/
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/** This function handles SVCall exception. **/
void SVC_Handler(void)
{
}

/** This function handles Debug Monitor exception. **/
void DebugMon_Handler(void)
{
}

/** This function handles SysTick Handler. **/
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            n32h47x_48x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/** This function handles SPI_Master or SPI2 global interrupt request. **/
#if defined (N32H475)
void SPI3_I2S3_IRQHandler(void)
{
    if (SPI_I2S_GetIntStatus(SPI3, SPI_I2S_INT_TE) != RESET)
    {
        /* Send SPI_Master data */
        SPI_I2S_TransmitData(SPI3, SPI_Master_Buffer_Tx[TxIdx++]);

        /* Disable SPI_Master TXE interrupt */
        if (TxIdx == BufferSize)
        {
            SPI_I2S_EnableInt(SPI3, SPI_I2S_INT_TE, DISABLE);
        }
    }
}
#else
void SPI5_IRQHandler(void)
{
    if (SPI_I2S_GetIntStatus(SPI5, SPI_I2S_INT_TE) != RESET)
    {
        /* Send SPI5 data */
        SPI_I2S_TransmitData(SPI5, SPI_Master_Buffer_Tx[TxIdx++]);

        /* Disable SPI5 TXE interrupt */
        if (TxIdx == BufferSize)
        {
            SPI_I2S_EnableInt(SPI5, SPI_I2S_INT_TE, DISABLE);
        }
    }
}
#endif

/** This function handles SPI2 global interrupt request. **/
void SPI2_I2S2_IRQHandler(void)
{
    /* Check the interrupt source */
    if (SPI_I2S_GetIntStatus(SPI2, SPI_I2S_INT_RNE) == SET)
    {
        /* Store SPI2 received data */
        SPI2_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
    }
}

/******************************************************************************/
/*                 n32h47x_48x Peripherals Interrupt Handlers                     */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_n32h4xx.s).                                                 */
/******************************************************************************/

/** This function handles PPP interrupt request. **/
//void PPP_IRQHandler(void)
//{
//}

