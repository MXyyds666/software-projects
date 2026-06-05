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
 *\*\file main.h
 *\*\author Nations
 *\*\version v1.0.0
 *\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
 **/

#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x.h"

/* Test start address of SD/MMC card define*/
#define TEST_CARD_ADDR          (0U)

/* Multi block read and write enable */
#define MUL_BLOCK_RW

/* Test block number define */
#ifdef MUL_BLOCK_RW
    #define TEST_BLOCK_NUM      (4U)
#else
    #define TEST_BLOCK_NUM      (1U)
#endif
    
#define TEST_BUF_LEN            (SD_BLOCKSIZE*TEST_BLOCK_NUM)
    
/* Read and write function define: DMA or polling */
#define SD_DMA_MODE                                 ((uint32_t)0x00000001)
//#define SD_POLLING_MODE                             ((uint32_t)0x00000002)

/* DMA define */
#define SDIO_DMA_CLOCK              (RCC_AHB_PERIPHEN_DMA1)
#define SDIO_DMA                    (DMA1)
#define SDIO_DMA_CHANNEL            (DMA1_CH1)
#define SDIO_DMA_ENDFLAG            (DMA_FLAG_TC1)

#define LED_PORT                    (GPIOA)
#define LED_PIN                     (GPIO_PIN_3)
#define LED_CLOCK                   (RCC_AHB_PERIPHEN_GPIOA)

#define KEY_PORT                    (GPIOA)
#define KEY_PIN                     (GPIO_PIN_0)
#define KEY_CLOCK                   (RCC_AHB_PERIPHEN_GPIOA)

#define LED_DELAY_TIMES             (1500000)
#define KEY_CHECK_TIMES             (250000)
    
void LED_Init(GPIO_Module* GPIOx,uint16_t Pin,uint32_t clock);
void LED_On(GPIO_Module* GPIOx,uint16_t Pin);
void LED_Off(GPIO_Module* GPIOx,uint16_t Pin);
void LED_Blink(GPIO_Module* GPIOx,uint16_t Pin);
void Key_Init(GPIO_Module* GPIOx,uint16_t Pin, uint32_t clock);
void Delay(uint32_t delay);

void SD_ShowCardInfo(void);
void Memset(void* pBuf, uint8_t data, uint32_t count);
void Fill_Buffer(uint8_t* pBuffer, uint32_t BufferLength, uint8_t Offset);
void DataShow(uint8_t* pBuf, uint32_t DataSize);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

