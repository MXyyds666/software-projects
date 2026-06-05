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
#include "main.h"


const uint32_t SRC_Const_Buffer[] = {
    0x72aa0305, 0x839916bf, 0x913782c5, 0x7e3bd953, 0x4174430c, 0x40d59981, 0xbccff537, 0xd22bca1a,
    0xda039c07, 0xff38219a, 0x17f16c33, 0x0056b9cf, 0x4e75ab0a, 0x0751b137, 0xbb051ec9, 0xcac01e06,
    0x5de647ba, 0xf2bd2108, 0x60a32be0, 0xbf80a68f, 0x299b4a55, 0xd37b1965, 0x84a3ca5b, 0x0f08f335,
    0x21c4084b, 0x738688ca, 0xe7971a03, 0xadcd6bfc, 0xd4bb55ae, 0x0ea4ed88, 0x131711a3, 0x4e92f42f,
};

uint32_t DST_Buffer[BUFFER_SIZE];
__IO uint32_t g_DMA_flag = 0;
volatile Status TransferStatus = FAILED; 

void NVIC_Configuration(void);
void GPIO_Configuration(void);
void DMA_Configuration(void);
Status Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
static void Delay(__IO uint32_t nCount);

int main(void)
{
    /* NVIC configuration */
    NVIC_Configuration();

    /* GPIO configuration */
    GPIO_Configuration();
    
    /*clear dir buffer*/
    memset(DST_Buffer, 0, sizeof(DST_Buffer));
    
    /* DMA configuration */
    DMA_Configuration();

    /* Wait the end of transmission */
    while (g_DMA_flag == 0)
    {
    }

    /* Check if the transmitted and received data are equal */
    TransferStatus = Buffercmp(SRC_Const_Buffer, DST_Buffer, BUFFER_SIZE);

    GPIO_SetBits(LED_GPIO, LED_PIN);
    
    while (1)
    {
        if(TransferStatus == PASSED){
            Delay(0x2FFFFF);
            GPIO_ResetBits(LED_GPIO, LED_PIN);
            Delay(0x2FFFFF);
            GPIO_SetBits(LED_GPIO, LED_PIN);
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
    /* Enable GPIOA clock */
    RCC_EnableAHB1PeriphClk(LED_GPIO_CLK, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = LED_PIN;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(LED_GPIO, &GPIO_InitStructure);
}

/**
*\*\name    NVIC_Configuration.
*\*\fun     Configure the nested vectored interrupt controller.
*\*\param   none 
*\*\return  none 
**/
void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    /* Enable DMA1 channel6 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
    RCC_EnableAHBPeriphClk(DMA_CLK, ENABLE);

    /* DMA1 channel x configuration */
    DMA_DeInit(DMA_CHANNEL_USED);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)SRC_Const_Buffer;
    DMA_InitStructure.MemAddr        = (uint32_t)DST_Buffer;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;   //peripheral is RAM
    DMA_InitStructure.BufSize        = BUFFER_SIZE;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_ENABLE;
    DMA_InitStructure.MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_WORD;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_WORD;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_ENABLE;
    /*  DMA BURST */
    DMA_InitStructure.BURST_BYPASS = DMA_BURST_BYPASS_ENABLE;
    /* BURST mode is almost */
    DMA_InitStructure.BURST_MODE = DMA_BURST_MODE_ALMOST;
    /* BURST length */
    DMA_InitStructure.BURST_LEN = DMA_BURST_LEN_8;
    
    DMA_Init(DMA_CHANNEL_USED, &DMA_InitStructure);
    
    DMA_RequestRemap(DMA_REMAP_RES, DMA_CHANNEL_USED, ENABLE);

    /* Enable DMAx Channel x Transfer Complete interrupt */
    DMA_ConfigInt(DMA_CHANNEL_USED, DMA_INT_TXC, ENABLE);

    /* Enable DMAx Channel x transfer */
    DMA_EnableChannel(DMA_CHANNEL_USED, ENABLE);
}

/**
*\*\name    Buffercmp.
*\*\fun     This function handles is DMA interrupt request.
*\*\param   none 
*\*\return  none 
**/
Status Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
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

/**
*\*\name    DMA_IRQ_HANDLER.
*\*\fun     This function is DMA interrupt request handles. 
*\*\param   none 
*\*\return  none 
**/
void DMA_IRQ_HANDLER(void)
{
    /* Test on DMA channel Transfer Complete interrupt */
    if (DMA_GetIntStatus(DMA_IT_FLAG_TC, DMA_USED))
    {
        /* Clear DMA channel Half Transfer, Transfer Complete and Global interrupt pending bits */
        DMA_ClrIntPendingBit(DMA_IT_FLAG_GL, DMA_USED);
        g_DMA_flag = 1;
    }
}

