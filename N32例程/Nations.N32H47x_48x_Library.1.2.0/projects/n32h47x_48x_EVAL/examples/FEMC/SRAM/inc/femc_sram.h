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
*\*\file femc_sram.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __FEMC_SRAM_H__
#define __FEMC_SRAM_H__

#include "n32h47x_48x.h"

#define Bank1_Block1_SRAM_ADDR ((uint32_t)0x60000000) // 0x60000000~63FFFFFF
#define Bank1_Block2_SRAM_ADDR ((uint32_t)0x64000000) // 0x64000000~67FFFFFF
#define Bank1_Block3_SRAM_ADDR ((uint32_t)0x68000000) // 0x68000000~6BFFFFFF
#define Bank1_Block4_SRAM_ADDR ((uint32_t)0x6C000000) // 0x6C000000~6FFFFFFF
#define BUFFER_SIZE      0x100
#define WRITE_READ_ADDR  0x00

/* DATA pin */
#define FEMC_D0_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_D0_PORT               GPIOD
#define FEMC_D0_PIN                GPIO_PIN_14
#define FEMC_D0_AF                 GPIO_AF10

#define FEMC_D1_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_D1_PORT               GPIOD
#define FEMC_D1_PIN                GPIO_PIN_15
#define FEMC_D1_AF                 GPIO_AF10

#define FEMC_D2_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_D2_PORT               GPIOD
#define FEMC_D2_PIN                GPIO_PIN_0
#define FEMC_D2_AF                 GPIO_AF10

#define FEMC_D3_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_D3_PORT               GPIOD
#define FEMC_D3_PIN                GPIO_PIN_1
#define FEMC_D3_AF                 GPIO_AF10

#define FEMC_D4_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define FEMC_D4_PORT               GPIOE
#define FEMC_D4_PIN                GPIO_PIN_7
#define FEMC_D4_AF                 GPIO_AF10

#define FEMC_D5_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define FEMC_D5_PORT               GPIOE
#define FEMC_D5_PIN                GPIO_PIN_8
#define FEMC_D5_AF                 GPIO_AF10

#define FEMC_D6_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define FEMC_D6_PORT               GPIOE
#define FEMC_D6_PIN                GPIO_PIN_9
#define FEMC_D6_AF                 GPIO_AF10

#define FEMC_D7_CLK                RCC_AHB_PERIPHEN_GPIOE
#define FEMC_D7_PORT               GPIOE
#define FEMC_D7_PIN                GPIO_PIN_10
#define FEMC_D7_AF                 GPIO_AF10

#define FEMC_D8_CLK                RCC_AHB_PERIPHEN_GPIOE
#define FEMC_D8_PORT               GPIOE
#define FEMC_D8_PIN                GPIO_PIN_11
#define FEMC_D8_AF                 GPIO_AF10

#define FEMC_D9_CLK                RCC_AHB_PERIPHEN_GPIOE
#define FEMC_D9_PORT               GPIOE
#define FEMC_D9_PIN                GPIO_PIN_12
#define FEMC_D9_AF                 GPIO_AF10

#define FEMC_D10_CLK                RCC_AHB_PERIPHEN_GPIOE
#define FEMC_D10_PORT               GPIOE
#define FEMC_D10_PIN                GPIO_PIN_13
#define FEMC_D10_AF                 GPIO_AF10

#define FEMC_D11_CLK                RCC_AHB_PERIPHEN_GPIOE
#define FEMC_D11_PORT               GPIOE
#define FEMC_D11_PIN                GPIO_PIN_14
#define FEMC_D11_AF                 GPIO_AF10

#define FEMC_D12_CLK                RCC_AHB_PERIPHEN_GPIOE
#define FEMC_D12_PORT               GPIOE
#define FEMC_D12_PIN                GPIO_PIN_15
#define FEMC_D12_AF                 GPIO_AF10

#define FEMC_D13_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_D13_PORT               GPIOD
#define FEMC_D13_PIN                GPIO_PIN_8
#define FEMC_D13_AF                 GPIO_AF10

#define FEMC_D14_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_D14_PORT               GPIOD
#define FEMC_D14_PIN                GPIO_PIN_9
#define FEMC_D14_AF                 GPIO_AF10

#define FEMC_D15_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_D15_PORT               GPIOD
#define FEMC_D15_PIN                GPIO_PIN_10
#define FEMC_D15_AF                 GPIO_AF10

/* ADD pin */
#define FEMC_A0_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A0_PORT               GPIOF
#define FEMC_A0_PIN                GPIO_PIN_0
#define FEMC_A0_AF                 GPIO_AF10

#define FEMC_A1_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A1_PORT               GPIOF
#define FEMC_A1_PIN                GPIO_PIN_1
#define FEMC_A1_AF                 GPIO_AF10

#define FEMC_A2_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A2_PORT               GPIOF
#define FEMC_A2_PIN                GPIO_PIN_2
#define FEMC_A2_AF                 GPIO_AF10

#define FEMC_A3_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A3_PORT               GPIOF
#define FEMC_A3_PIN                GPIO_PIN_3
#define FEMC_A3_AF                 GPIO_AF10

#define FEMC_A4_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A4_PORT               GPIOF
#define FEMC_A4_PIN                GPIO_PIN_4
#define FEMC_A4_AF                 GPIO_AF10

#define FEMC_A5_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A5_PORT               GPIOF
#define FEMC_A5_PIN                GPIO_PIN_5
#define FEMC_A5_AF                 GPIO_AF10

#define FEMC_A6_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A6_PORT               GPIOF
#define FEMC_A6_PIN                GPIO_PIN_12
#define FEMC_A6_AF                 GPIO_AF10

#define FEMC_A7_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A7_PORT               GPIOF
#define FEMC_A7_PIN                GPIO_PIN_13
#define FEMC_A7_AF                 GPIO_AF10

#define FEMC_A8_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A8_PORT               GPIOF
#define FEMC_A8_PIN                GPIO_PIN_14
#define FEMC_A8_AF                 GPIO_AF10

#define FEMC_A9_CLK                RCC_AHB_PERIPHEN_GPIOF
#define FEMC_A9_PORT               GPIOF
#define FEMC_A9_PIN                GPIO_PIN_15
#define FEMC_A9_AF                 GPIO_AF10

#define FEMC_A10_CLK                RCC_AHB_PERIPHEN_GPIOG
#define FEMC_A10_PORT               GPIOG
#define FEMC_A10_PIN                GPIO_PIN_0
#define FEMC_A10_AF                 GPIO_AF10

#define FEMC_A11_CLK                RCC_AHB_PERIPHEN_GPIOG
#define FEMC_A11_PORT               GPIOG
#define FEMC_A11_PIN                GPIO_PIN_1
#define FEMC_A11_AF                 GPIO_AF10

#define FEMC_A12_CLK                RCC_AHB_PERIPHEN_GPIOG
#define FEMC_A12_PORT               GPIOG
#define FEMC_A12_PIN                GPIO_PIN_2
#define FEMC_A12_AF                 GPIO_AF10

#define FEMC_A13_CLK                RCC_AHB_PERIPHEN_GPIOG
#define FEMC_A13_PORT               GPIOG
#define FEMC_A13_PIN                GPIO_PIN_3
#define FEMC_A13_AF                 GPIO_AF10

#define FEMC_A14_CLK                RCC_AHB_PERIPHEN_GPIOG
#define FEMC_A14_PORT               GPIOG
#define FEMC_A14_PIN                GPIO_PIN_4
#define FEMC_A14_AF                 GPIO_AF10

#define FEMC_A15_CLK                RCC_AHB_PERIPHEN_GPIOG
#define FEMC_A15_PORT               GPIOG
#define FEMC_A15_PIN                GPIO_PIN_5
#define FEMC_A15_AF                 GPIO_AF10

#define FEMC_A16_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_A16_PORT               GPIOD
#define FEMC_A16_PIN                GPIO_PIN_11
#define FEMC_A16_AF                 GPIO_AF10

#define FEMC_A17_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_A17_PORT               GPIOD
#define FEMC_A17_PIN                GPIO_PIN_12
#define FEMC_A17_AF                 GPIO_AF10

#define FEMC_A18_CLK                RCC_AHB_PERIPHEN_GPIOD
#define FEMC_A18_PORT               GPIOD
#define FEMC_A18_PIN                GPIO_PIN_13
#define FEMC_A18_AF                 GPIO_AF10

#define FEMC_A19_CLK                RCC_AHB_PERIPHEN_GPIOE
#define FEMC_A19_PORT               GPIOE
#define FEMC_A19_PIN                GPIO_PIN_3
#define FEMC_A19_AF                 GPIO_AF10

/* NOE pin */
#define FEMC_NOE_CLK               RCC_AHB_PERIPHEN_GPIOD
#define FEMC_NOE_PORT              GPIOD
#define FEMC_NOE_PIN               GPIO_PIN_4
#define FEMC_NOE_AF                GPIO_AF10

/* NWE pin */
#define FEMC_NWE_CLK               RCC_AHB_PERIPHEN_GPIOD
#define FEMC_NWE_PORT              GPIOD
#define FEMC_NWE_PIN               GPIO_PIN_5
#define FEMC_NWE_AF                GPIO_AF10

/* NE pin */
#define FEMC_NE_CLK               RCC_AHB_PERIPHEN_GPIOB
#define FEMC_NE_PORT              GPIOB
#define FEMC_NE_PIN               GPIO_PIN_9                //NE1
#define FEMC_NE_AF                GPIO_AF0

/* NBL0 pin */
#define FEMC_NBL0_CLK             RCC_AHB_PERIPHEN_GPIOE
#define FEMC_NBL0_PORT            GPIOE
#define FEMC_NBL0_PIN             GPIO_PIN_0
#define FEMC_NBL0_AF              GPIO_AF10

/* NBL1 pin */
#define FEMC_NBL1_CLK             RCC_AHB_PERIPHEN_GPIOE
#define FEMC_NBL1_PORT            GPIOE
#define FEMC_NBL1_PIN             GPIO_PIN_1
#define FEMC_NBL1_AF              GPIO_AF10

void FEMC_Sram_Init(uint8_t bank_num);
void FEMC_Sram_Read_Buffer(uint16_t* pBuffer, uint8_t block_index, uint32_t read_addr, uint32_t number);
void FEMC_Sram_Write_Buffer(uint16_t* pBuffer, uint8_t block_index, uint32_t write_addr, uint32_t number);
#endif //__FEMC_SRAM_H__
