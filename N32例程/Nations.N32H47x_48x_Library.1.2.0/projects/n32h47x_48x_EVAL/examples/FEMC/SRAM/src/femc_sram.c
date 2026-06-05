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
*\*\file femc_sram.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "n32h47x_48x_femc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "log.h"
#include "femc_sram.h"

/**
*\*\name    FEMC_GPIO_Init.
*\*\fun     FEMC gpio initialized.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\return  none 
**/
void FEMC_GPIO_Init(uint8_t block_index)
{
    GPIO_InitType GPIO_InitStructure;
    
    RCC_EnableAHB1PeriphClk(FEMC_NOE_CLK  | FEMC_NWE_CLK  | FEMC_NE_CLK   |\
                            FEMC_NBL0_CLK | FEMC_NBL1_CLK | FEMC_D0_CLK   |\
                            FEMC_D1_CLK   | FEMC_D2_CLK   | FEMC_D3_CLK   |\
                            FEMC_D4_CLK   | FEMC_D5_CLK   | FEMC_D6_CLK   |\
                            FEMC_D7_CLK   | FEMC_D8_CLK   | FEMC_D9_CLK   |\
                            FEMC_D10_CLK  | FEMC_D11_CLK  | FEMC_D12_CLK  |\
                            FEMC_D13_CLK  | FEMC_D14_CLK  | FEMC_D15_CLK  |\
                            FEMC_A0_CLK   | FEMC_A1_CLK   | FEMC_A2_CLK   |\
                            FEMC_A3_CLK   | FEMC_A4_CLK   | FEMC_A5_CLK   |\
                            FEMC_A6_CLK   | FEMC_A7_CLK   | FEMC_A8_CLK   |\
                            FEMC_A9_CLK   | FEMC_A10_CLK  | FEMC_A11_CLK  |\
                            FEMC_A12_CLK  | FEMC_A13_CLK  | FEMC_A14_CLK  |\
                            FEMC_A15_CLK  | FEMC_A16_CLK  | FEMC_A17_CLK  |\
                            FEMC_A18_CLK , ENABLE);
    
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);
    /*-- GPIO Configuration ------------------------------------------------------*/
    /* Data lines configuration */
    /*  DATA pin configuration  */
    GPIO_InitStructure.Pin            = FEMC_D0_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D0_AF;
    GPIO_InitPeripheral(FEMC_D0_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D1_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D1_AF;
    GPIO_InitPeripheral(FEMC_D1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = FEMC_D2_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D2_AF;
    GPIO_InitPeripheral(FEMC_D2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = FEMC_D3_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D3_AF;
    GPIO_InitPeripheral(FEMC_D3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = FEMC_D4_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D4_AF;
    GPIO_InitPeripheral(FEMC_D4_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D5_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D5_AF;
    GPIO_InitPeripheral(FEMC_D5_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = FEMC_D6_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D6_AF;
    GPIO_InitPeripheral(FEMC_D6_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = FEMC_D7_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D7_AF;
    GPIO_InitPeripheral(FEMC_D7_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D8_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D8_AF;
    GPIO_InitPeripheral(FEMC_D8_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D9_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D9_AF;
    GPIO_InitPeripheral(FEMC_D9_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D10_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D10_AF;
    GPIO_InitPeripheral(FEMC_D10_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D11_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D11_AF;
    GPIO_InitPeripheral(FEMC_D11_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D12_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D12_AF;
    GPIO_InitPeripheral(FEMC_D12_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D13_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D13_AF;
    GPIO_InitPeripheral(FEMC_D13_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D14_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D14_AF;
    GPIO_InitPeripheral(FEMC_D14_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_D15_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_D15_AF;
    GPIO_InitPeripheral(FEMC_D15_PORT, &GPIO_InitStructure);

    /* Address lines configuration */
    GPIO_InitStructure.Pin            = FEMC_A0_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A0_AF;
    GPIO_InitPeripheral(FEMC_A0_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A1_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A1_AF;
    GPIO_InitPeripheral(FEMC_A1_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A2_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A2_AF;
    GPIO_InitPeripheral(FEMC_A2_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A3_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A3_AF;
    GPIO_InitPeripheral(FEMC_A3_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A4_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A4_AF;
    GPIO_InitPeripheral(FEMC_A4_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A5_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A5_AF;
    GPIO_InitPeripheral(FEMC_A5_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A6_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A6_AF;
    GPIO_InitPeripheral(FEMC_A6_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A7_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A7_AF;
    GPIO_InitPeripheral(FEMC_A7_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A8_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A8_AF;
    GPIO_InitPeripheral(FEMC_A8_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A9_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A9_AF;
    GPIO_InitPeripheral(FEMC_A9_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A10_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A10_AF;
    GPIO_InitPeripheral(FEMC_A10_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A11_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A11_AF;
    GPIO_InitPeripheral(FEMC_A11_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A12_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A12_AF;
    GPIO_InitPeripheral(FEMC_A12_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A13_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A13_AF;
    GPIO_InitPeripheral(FEMC_A13_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A14_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A14_AF;
    GPIO_InitPeripheral(FEMC_A14_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A15_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A15_AF;
    GPIO_InitPeripheral(FEMC_A15_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A16_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A16_AF;
    GPIO_InitPeripheral(FEMC_A16_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A17_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A17_AF;
    GPIO_InitPeripheral(FEMC_A17_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A18_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A18_AF;
    GPIO_InitPeripheral(FEMC_A18_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A19_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A19_AF;
    GPIO_InitPeripheral(FEMC_A19_PORT, &GPIO_InitStructure);

    /* NOE pin configuration */
    GPIO_InitStructure.Pin = FEMC_NOE_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_NOE_AF;
    GPIO_InitPeripheral(FEMC_NOE_PORT, &GPIO_InitStructure);
    
    /* NWE pin configuration */
    GPIO_InitStructure.Pin = FEMC_NWE_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_NWE_AF;
    GPIO_InitPeripheral(FEMC_NWE_PORT, &GPIO_InitStructure);

    /* NE pin configuration*/
    GPIO_InitStructure.Pin = FEMC_NE_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_NE_AF;
    GPIO_InitPeripheral(FEMC_NE_PORT, &GPIO_InitStructure);

    /* NBL0 pin configuration*/
    GPIO_InitStructure.Pin = FEMC_NBL0_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_NBL1_AF;
    GPIO_InitPeripheral(FEMC_NBL1_PORT, &GPIO_InitStructure);
    
    /* NBL1 pin configuration*/
    GPIO_InitStructure.Pin = FEMC_NBL1_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_NBL1_AF;
    GPIO_InitPeripheral(FEMC_NBL1_PORT, &GPIO_InitStructure);
}

/**
*\*\name    FEMC_Sram_Init.
*\*\fun     Configures the FEMC to interface with the sram memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\return  none 
**/
void FEMC_Sram_Init(uint8_t block_index)
{
    FEMC_SN_Bank_Block *Block;
    FEMC_NorSramInitType FEMC_NORSRAMInitStructure;
    FEMC_NorSramTimingInitType p;

    FEMC_GPIO_Init(block_index);

    /* Enable the FEMC Clock */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_FEMC, ENABLE);

    /*-- FEMC Configuration ----------------------------------------------------*/
    p.AddrSetTime      = FEMC_NOR_SRAM_ADDR_SETUP_TIME_2HCLK;
    p.AddrHoldTime     = FEMC_NOR_SRAM_ADDR_HOLD_TIME_2HCLK;
    p.DataSetTime      = FEMC_NOR_SRAM_DATA_SETUP_TIME(5);
    p.BusRecoveryCycle = FEMC_NOR_SRAM_BUSRECOVERY_TIME_1HCLK;
    p.ClkDiv           = FEMC_NOR_SRAM_CLK_DIV_2;
    p.DataHoldTime     = FEMC_NOR_SRAM_DATA_HOLD_TIME_2HCLK;
    p.AccMode          = FEMC_NOR_SRAM_ACC_MODE_A;

    if (block_index == 1)
    {
        Block = FEMC_SN_BANK_BLOCK1;
    }
    else if (block_index == 2)
    {
        Block = FEMC_SN_BANK_BLOCK2;
    }
    else if (block_index == 3)
    {
        Block = FEMC_SN_BANK_BLOCK3;
    }
    else if (block_index == 4)
    {
        Block = FEMC_SN_BANK_BLOCK4;
    }
    
    FEMC_InitNorSramStruct(&FEMC_NORSRAMInitStructure);
    FEMC_NORSRAMInitStructure.Block            = Block;
    FEMC_NORSRAMInitStructure.DataAddrMux      = FEMC_NOR_SRAM_MUX_DISABLE;
    FEMC_NORSRAMInitStructure.MemType          = FEMC_MEM_TYPE_SRAM;
    FEMC_NORSRAMInitStructure.MemDataWidth     = FEMC_NOR_SRAM_DATA_WIDTH_16B;
    FEMC_NORSRAMInitStructure.BurstAccMode     = FEMC_NOR_SRAM_BURST_MODE_DISABLE;
    FEMC_NORSRAMInitStructure.WaitSigPolarity  = FEMC_NOR_SRAM_WAIT_SIGNAL_LOW;
    FEMC_NORSRAMInitStructure.WrapMode         = FEMC_NOR_SRAM_WRAP_DISABLE;
    FEMC_NORSRAMInitStructure.WaitSigConfig    = FEMC_NOR_SRAM_NWAIT_BEFORE_STATE;
    FEMC_NORSRAMInitStructure.WriteEnable      = FEMC_NOR_SRAM_WRITE_ENABLE;
    FEMC_NORSRAMInitStructure.WaitSigEnable    = FEMC_NOR_SRAM_NWAIT_DISABLE;
    FEMC_NORSRAMInitStructure.ExtModeEnable    = FEMC_NOR_SRAM_EXTENDED_DISABLE;
    FEMC_NORSRAMInitStructure.WriteBurstEnable = FEMC_NOR_SRAM_BURST_WRITE_DISABLE;
    FEMC_NORSRAMInitStructure.RWTimingStruct   = &p;
    FEMC_NORSRAMInitStructure.WTimingStruct    = &p;

    FEMC_InitNorSram(&FEMC_NORSRAMInitStructure);

    /* Enable FEMC Bank1_NOR Bank */
    FEMC_EnableNorSram(Block, ENABLE);
}

/**
*\*\name    FEMC_Sram_Write_Buffer.
*\*\fun     Writes a Half-word buffer to the FEMC SRAM memory.
*\*\param   pBuffer：pointer to buffer.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   write_addr: SRAM memory internal address from which the data will be written.
*\*\param   number: number of half-words to write.
*\*\return  none 
**/
void FEMC_Sram_Write_Buffer(uint16_t* pBuffer, uint8_t block_index, uint32_t write_addr, uint32_t number)
{
    uint32_t transfer_addr;
    if (block_index == 1)
    {
        transfer_addr = Bank1_Block1_SRAM_ADDR;
    }
    else if (block_index == 2)
    {
        transfer_addr = Bank1_Block2_SRAM_ADDR;
    }
    else if (block_index == 3)
    {
        transfer_addr = Bank1_Block3_SRAM_ADDR;
    }
    else if (block_index == 4)
    {
        transfer_addr = Bank1_Block4_SRAM_ADDR;
    }
    for (; number != 0; number--) /* while there is data to write */
    {
        /* Transfer data to the memory */
        *(uint16_t*)(transfer_addr + write_addr) = *pBuffer++;

        /* Increment the address*/
        write_addr += 2;
    }
}

/**
*\*\name    FEMC_Sram_Read_Buffer.
*\*\fun     Read a Half-word buffer from the FEMC SRAM memory.
*\*\param   pBuffer：pointer to buffer.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   read_addr: SRAM memory internal address to read from.
*\*\param   number: number of half-words to read.
*\*\return  none 
**/
void FEMC_Sram_Read_Buffer(uint16_t* pBuffer, uint8_t block_index, uint32_t read_addr, uint32_t number)
{
    uint32_t transfer_addr;
    if (block_index == 1)
    {
        transfer_addr = Bank1_Block1_SRAM_ADDR;
    }
    else if (block_index == 2)
    {
        transfer_addr = Bank1_Block2_SRAM_ADDR;
    }
    else if (block_index == 3)
    {
        transfer_addr = Bank1_Block3_SRAM_ADDR;
    }
    else if (block_index == 4)
    {
        transfer_addr = Bank1_Block4_SRAM_ADDR;
    }
    for (; number != 0; number--) /* while there is data to read */
    {
        /* Read a half-word from the memory */
        *pBuffer++ = *(__IO uint16_t*)(transfer_addr + read_addr);

        /* Increment the address*/
        read_addr += 2;
    }
}
