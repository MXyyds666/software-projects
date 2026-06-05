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
*\*\file femc_nor.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "femc_nor.h"
#include "n32h47x_48x_femc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "log.h"


#define Bank1_NOR1_ADDR ((uint32_t)0x60000000)
#define Bank1_NOR2_ADDR ((uint32_t)0x64000000)
#define Bank1_NOR3_ADDR ((uint32_t)0x68000000)
#define Bank1_NOR4_ADDR ((uint32_t)0x6C000000)
/* Delay definition */
#define BLOCKERASE_TIMEOUT              ((uint32_t)0x00A00000)
#define CHIPERASE_TIMEOUT               ((uint32_t)0x30000000)
#define PROGRAM_TIMEOUT                 ((uint32_t)0x00001400)

#define ADDR_SHIFT_BLOCK1(A) (Bank1_NOR1_ADDR + (2 * (A)))
#define ADDR_SHIFT_BLOCK2(A) (Bank1_NOR2_ADDR + (2 * (A)))
#define ADDR_SHIFT_BLOCK3(A) (Bank1_NOR3_ADDR + (2 * (A)))
#define ADDR_SHIFT_BLOCK4(A) (Bank1_NOR4_ADDR + (2 * (A)))

#define NOR_WRITE(Address, Data) (*((__IO uint16_t *)(Address)) = (Data))

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
                            FEMC_NWAIT_CLK| FEMC_D0_CLK   |                \
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
                            FEMC_A18_CLK  | FEMC_A19_CLK  | FEMC_A20_CLK  |\
                            FEMC_A21_CLK  | FEMC_A22_CLK  | FEMC_A23_CLK  |\
                            FEMC_A24_CLK  | FEMC_A25_CLK , ENABLE);
    
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
    
    GPIO_InitStructure.Pin            = FEMC_A20_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A20_AF;
    GPIO_InitPeripheral(FEMC_A20_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A21_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A21_AF;
    GPIO_InitPeripheral(FEMC_A21_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A22_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A22_AF;
    GPIO_InitPeripheral(FEMC_A22_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A23_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A23_AF;
    GPIO_InitPeripheral(FEMC_A23_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A24_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A24_AF;
    GPIO_InitPeripheral(FEMC_A24_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = FEMC_A25_PIN;
    GPIO_InitStructure.GPIO_Alternate = FEMC_A25_AF;
    GPIO_InitPeripheral(FEMC_A25_PORT, &GPIO_InitStructure);

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

    /* NWAIT pin configuration*/
    GPIO_InitStructure.Pin        = GPIO_PIN_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF10;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);

}

/**
*\*\name    FEMC_Nor_Init.
*\*\fun     Configures the FEMC to interface with the NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\return  none 
**/
void FEMC_Nor_Init(uint8_t block_index)
{
    FEMC_SN_Bank_Block *Block;
    FEMC_NorSramInitType FEMC_NORSRAMInitStructure;
    FEMC_NorSramTimingInitType p;

    FEMC_GPIO_Init(block_index);

    /* Enable the FEMC Clock */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_FEMC, ENABLE);

    /*-- FEMC Configuration ----------------------------------------------------*/
    p.AddrSetTime      = FEMC_NOR_SRAM_ADDR_SETUP_TIME_15HCLK;
    p.AddrHoldTime     = FEMC_NOR_SRAM_ADDR_HOLD_TIME_2HCLK;
    p.DataSetTime      = FEMC_NOR_SRAM_DATA_SETUP_TIME(12);
    p.BusRecoveryCycle = FEMC_NOR_SRAM_BUSRECOVERY_TIME_1HCLK;
    p.ClkDiv           = FEMC_NOR_SRAM_CLK_DIV_2;
    p.DataHoldTime     = FEMC_NOR_SRAM_DATA_HOLD_TIME_2HCLK;
    p.AccMode          = FEMC_NOR_SRAM_ACC_MODE_B;

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
    FEMC_NORSRAMInitStructure.MemType          = FEMC_MEM_TYPE_NOR;
    FEMC_NORSRAMInitStructure.MemDataWidth     = FEMC_NOR_SRAM_DATA_WIDTH_16B;
    FEMC_NORSRAMInitStructure.BurstAccMode     = FEMC_NOR_SRAM_BURST_MODE_DISABLE;
    FEMC_NORSRAMInitStructure.AsynchroWait     = FEMC_NOR_SRAM_ASYNC_NWAIT_DISABLE;
    FEMC_NORSRAMInitStructure.WaitSigPolarity  = FEMC_NOR_SRAM_WAIT_SIGNAL_LOW;
    FEMC_NORSRAMInitStructure.WrapMode         = FEMC_NOR_SRAM_WRAP_DISABLE;
    FEMC_NORSRAMInitStructure.WaitSigConfig    = FEMC_NOR_SRAM_NWAIT_BEFORE_STATE;
    FEMC_NORSRAMInitStructure.WriteEnable      = FEMC_NOR_SRAM_WRITE_ENABLE;
    FEMC_NORSRAMInitStructure.WaitSigEnable    = FEMC_NOR_SRAM_NWAIT_DISABLE;
    FEMC_NORSRAMInitStructure.ExtModeEnable    = FEMC_NOR_SRAM_EXTENDED_ENABLE;
    FEMC_NORSRAMInitStructure.WriteBurstEnable = FEMC_NOR_SRAM_BURST_WRITE_DISABLE;
    FEMC_NORSRAMInitStructure.RWTimingStruct   = &p;
    FEMC_NORSRAMInitStructure.WTimingStruct    = &p;

    FEMC_InitNorSram(&FEMC_NORSRAMInitStructure);

    /* Enable FEMC Bank1_NOR Bank */
    FEMC_EnableNorSram(Block, ENABLE);
}

/**
*\*\name    FEMC_Nor_ReadID.
*\*\fun     Reads NOR memory's Manufacturer and Device Code.
*\*\param   NOR_ID: pointer to a NOR_IDType structure which will hold
*\*\        the Manufacturer and Device Code.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\return  none 
**/
void FEMC_Nor_ReadID(NOR_IDType* NOR_ID, uint8_t block_index)
{
    if(block_index == 1)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x0090);

        NOR_ID->Manufacturer_Code = *(__IO uint16_t*)ADDR_SHIFT_BLOCK1(0x0000);
        NOR_ID->Device_Code1      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK1(0x0001);
        NOR_ID->Device_Code2      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK1(0x000E);
        NOR_ID->Device_Code3      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK1(0x000F);
    }
    else if(block_index == 2)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x0090);

        NOR_ID->Manufacturer_Code = *(__IO uint16_t*)ADDR_SHIFT_BLOCK2(0x0000);
        NOR_ID->Device_Code1      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK2(0x0001);
        NOR_ID->Device_Code2      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK2(0x000E);
        NOR_ID->Device_Code3      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK2(0x000F);
    }
    else if(block_index == 3)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x0090);

        NOR_ID->Manufacturer_Code = *(__IO uint16_t*)ADDR_SHIFT_BLOCK3(0x0000);
        NOR_ID->Device_Code1      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK3(0x0001);
        NOR_ID->Device_Code2      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK3(0x000E);
        NOR_ID->Device_Code3      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK3(0x000F);
    }
    else if(block_index == 4)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x0090);

        NOR_ID->Manufacturer_Code = *(__IO uint16_t*)ADDR_SHIFT_BLOCK4(0x0000);
        NOR_ID->Device_Code1      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK4(0x0001);
        NOR_ID->Device_Code2      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK4(0x000E);
        NOR_ID->Device_Code3      = *(__IO uint16_t*)ADDR_SHIFT_BLOCK4(0x000F);
    }
    
    FEMC_Nor_Return_To_Read_Mode(block_index);
}

/**
*\*\name    FEMC_Nor_Erase_Block.
*\*\fun     Erases the specified Nor memory block.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   block_addr: address of the block to erase.
*\*\return  NOR_SUCCESS 
**/
NOR_Status FEMC_Nor_Erase_Block(uint8_t block_index, uint32_t block_addr)
{
    uint32_t test_base_addr;
    if (block_index == 1)
    {
        test_base_addr = Bank1_NOR1_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055);
    }
    else if (block_index == 2)
    {
        test_base_addr = Bank1_NOR2_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055);
    }
    else if (block_index == 3)
    {
        test_base_addr = Bank1_NOR3_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055);
    }
    else if (block_index == 4)
    {
        test_base_addr = Bank1_NOR4_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055);
    }

    NOR_WRITE((test_base_addr + block_addr), 0x30);

    return FEMC_Nor_Get_Status(block_index, BLOCKERASE_TIMEOUT);
}

/**
*\*\name    FEMC_Nor_Erase_Chip.
*\*\fun     Erases the entire chip.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\return  NOR_Status:The returned value can be: 
*\*\            - NOR_SUCCESS, NOR_ERROR or NOR_TIMEOUT
**/
NOR_Status FEMC_Nor_Erase_Chip(uint8_t block_index)
{
    if (block_index == 1)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x0010);
    }
    else if (block_index == 2)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x0010);
    }
    else if (block_index == 3)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x0010);
    }
    else if (block_index == 4)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x0080);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x0010);
    }
    
    return FEMC_Nor_Get_Status(block_index, CHIPERASE_TIMEOUT);
}

/**
*\*\name    FEMC_Nor_Write_Halfword.
*\*\fun     Writes a half-word to the NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   write_addr: NOR memory internal address to write to.
*\*\param   data: Data to write.
*\*\return  NOR_Status:The returned value can be: 
*\*\            - NOR_SUCCESS, NOR_ERROR or NOR_TIMEOUT
**/
NOR_Status FEMC_Nor_Write_Halfword(uint8_t block_index, uint32_t write_addr, uint16_t data)
{
    uint32_t test_base_addr;
    if (block_index == 1)
    {
        test_base_addr = Bank1_NOR1_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00A0);
    }
    else if (block_index == 2)
    {
        test_base_addr = Bank1_NOR2_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00A0);
    }
    else if (block_index == 3)
    {
        test_base_addr = Bank1_NOR3_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00A0);
    }
    else if (block_index == 4)
    {
        test_base_addr = Bank1_NOR4_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00A0);
    }
    
    NOR_WRITE((test_base_addr + write_addr), data);
    return FEMC_Nor_Get_Status(block_index, PROGRAM_TIMEOUT);; 
}

/**
*\*\name    FEMC_Nor_Write_Byte.
*\*\fun     Writes a byte to the NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   write_addr: NOR memory internal address to write to.
*\*\param   data: Data to write.
*\*\return  NOR_Status:The returned value can be: 
*\*\            - NOR_SUCCESS, NOR_ERROR or NOR_TIMEOUT
**/
NOR_Status FEMC_Nor_Write_Byte(uint8_t block_index, uint32_t write_addr, uint8_t data)
{
    uint32_t test_base_addr;
    if (block_index == 1)
    {
        test_base_addr = Bank1_NOR1_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA); /* write unlock cycle 1 */
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055); /* write unlock cycle 2 */
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00A0); /* write program setup command */
    }
    else if (block_index == 2)
    {
        test_base_addr = Bank1_NOR2_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA); /* write unlock cycle 1 */
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055); /* write unlock cycle 2 */
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00A0); /* write program setup command */
    }
    else if (block_index == 3)
    {
        test_base_addr = Bank1_NOR3_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA); /* write unlock cycle 1 */
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055); /* write unlock cycle 2 */
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00A0); /* write program setup command */
    }
    else if (block_index == 4)
    {
        test_base_addr = Bank1_NOR4_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA); /* write unlock cycle 1 */
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055); /* write unlock cycle 2 */
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00A0); /* write program setup command */
    }
    
    (*(__IO uint8_t*)(test_base_addr + write_addr) = (data)); /* write data to be programmed */

    return (FEMC_Nor_Get_Status(block_index, PROGRAM_TIMEOUT));
}

/**
*\*\name    FEMC_Nor_Write_Buffer_Byte.
*\*\fun     Writes a byte buffer to the FEMC NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   pBuffer: pointer to writer buffer.
*\*\param   write_addr: NOR memory internal address to write to.
*\*\param   num_halfword_to_write: number of byte to write.
*\*\return  NOR_Status:The returned value can be: 
*\*\            - NOR_SUCCESS, NOR_ERROR or NOR_TIMEOUT
**/
NOR_Status FEMC_Nor_Write_Buffer_Byte(uint8_t block_index, uint8_t* pBuffer, uint32_t write_addr, uint32_t num_byte_to_write)
{
    NOR_Status status = NOR_ONGOING;
    do
    {
        /* Transfer data to the memory */
        status     = FEMC_Nor_Write_Byte(block_index, write_addr, *pBuffer++);
        write_addr = write_addr + 1;
        num_byte_to_write--;
    } while ((status == NOR_SUCCESS) && (num_byte_to_write != 0));

    return (status);
}

/**
*\*\name    FEMC_Nor_Write_Buffer_Halfword.
*\*\fun     Writes a half-word buffer to the FEMC NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   pBuffer: pointer to writer buffer.
*\*\param   write_addr: NOR memory internal address to write to.
*\*\param   num_halfword_to_write: number of Half words to write.
*\*\return  NOR_Status:The returned value can be: 
*\*\            - NOR_SUCCESS, NOR_ERROR or NOR_TIMEOUT
**/
NOR_Status FEMC_Nor_Write_Buffer_Halfword(uint8_t block_index, uint16_t* pBuffer, uint32_t write_addr, uint32_t num_halfword_to_write)
{
    NOR_Status status = NOR_ONGOING;

    do
    {
        /* Transfer data to the memory */
        status     = FEMC_Nor_Write_Halfword(block_index, write_addr, *pBuffer++);
        write_addr = write_addr + 2;
        num_halfword_to_write--;
    } while ((status == NOR_SUCCESS) && (num_halfword_to_write != 0));
    return (status);
}

/**
*\*\name    FEMC_Nor_Read_Halfword.
*\*\fun     Reads a half-word from the NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   read_addr: NOR memory internal address to read from.
*\*\return  Half-word read from the NOR memory
**/
uint16_t FEMC_Nor_Read_Halfword(uint8_t block_index, uint32_t read_addr)
{
    uint32_t test_base_addr;
    if (block_index == 0)
    {
        test_base_addr = Bank1_NOR1_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x002AA), 0x0055);
    }
    else
    {
        test_base_addr = Bank1_NOR2_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x002AA), 0x0055);
    }
    NOR_WRITE((test_base_addr + read_addr), 0x00F0);
    return (*(__IO uint16_t*)((test_base_addr + read_addr)));
}

/**
*\*\name    FEMC_Nor_Read_Byte.
*\*\fun     Reads a byte from the NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   read_addr: NOR memory internal address to read from.
*\*\return  Byte read from the NOR memory
**/
uint8_t FEMC_Nor_Read_Byte(uint8_t block_index, uint32_t read_addr)
{
    uint32_t test_base_addr;
    if (block_index == 1)
    {
        test_base_addr = Bank1_NOR1_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x002AA), 0x0055);
    }
    else if (block_index == 1)
    {
        test_base_addr = Bank1_NOR2_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x002AA), 0x0055);
    }
    else if (block_index == 2)
    {
        test_base_addr = Bank1_NOR3_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x002AA), 0x0055);
    }
    else if (block_index == 3)
    {
        test_base_addr = Bank1_NOR4_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x002AA), 0x0055);
    }

    NOR_WRITE((test_base_addr + read_addr), 0x00F0);

    return (*(__IO uint8_t*)((test_base_addr + read_addr)));
}

/**
*\*\name    FEMC_Nor_Read_Buffer_Halfword.
*\*\fun     Reads a block of data in half-word from the FEMC NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   pBuffer: pointer to the buffer that receives the data read from the NOR memory.
*\*\param   read_addr: NOR memory internal address to read from.
*\*\param   num_halfword_to_read: number of Half word to read.
*\*\return  none
**/
void FEMC_Nor_Read_Buffer_Halfword(uint8_t block_index, uint16_t* pBuffer, uint32_t read_addr, uint32_t num_halfword_to_read)
{
    uint32_t test_base_addr;
    if (block_index == 1)
    {
        test_base_addr = Bank1_NOR1_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x02AA), 0x0055);
    }
    else if (block_index == 2)
    {
        test_base_addr = Bank1_NOR2_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x02AA), 0x0055);
    }
    else if (block_index == 3)
    {
        test_base_addr = Bank1_NOR3_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x02AA), 0x0055);
    }
    else if (block_index == 4)
    {
        test_base_addr = Bank1_NOR4_ADDR;
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x0555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x02AA), 0x0055);
    }

    NOR_WRITE(test_base_addr + read_addr, 0x00F0);

    for (; num_halfword_to_read != 0x00; num_halfword_to_read--) /* while there is data to read */
    {
        /* Read a Halfword from the NOR */
        *pBuffer++ = *(__IO uint16_t*)((test_base_addr + read_addr));
        read_addr  = read_addr + 2;
    }
}

/**
*\*\name    FEMC_Nor_Read_Buffer_Byte.
*\*\fun     Reads a block of data in byte from the FEMC NOR memory.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   pBuffer: pointer to the buffer that receives the data read from the NOR memory.
*\*\param   read_addr: NOR memory internal address to read from.
*\*\param   num_halfword_to_read: number of Byte to read.
*\*\return  none
**/
void FEMC_Nor_Read_Buffer_Byte(uint8_t block_index, uint8_t* pBuffer, uint32_t read_addr, uint32_t num_byte_to_read)
{
    uint32_t test_base_addr;
    if (block_index == 0)
    {
        test_base_addr = Bank1_NOR1_ADDR;
    }
    else
    {
        test_base_addr = Bank1_NOR2_ADDR;
    }
    for (; num_byte_to_read != 0x00; num_byte_to_read--) /* while there is data to read */
    {
        /* Read a Halfword from the NOR */
        *pBuffer++ = *(__IO uint8_t*)((test_base_addr + read_addr));
        read_addr  = read_addr + 1;
    }
}

/**
*\*\name    FEMC_Nor_Return_To_Read_Mode.
*\*\fun     FEMC NOR memory return to read mode.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\return  NOR_SUCCESS
**/
NOR_Status FEMC_Nor_Return_To_Read_Mode(uint8_t block_index)
{
    uint32_t test_base_addr;
    if (block_index == 1)
    {
        test_base_addr = Bank1_NOR1_ADDR;
    }
    else if(block_index == 2)
    {
        test_base_addr = Bank1_NOR2_ADDR;
    }
    else if(block_index == 3)
    {
        test_base_addr = Bank1_NOR3_ADDR;
    }
    else if(block_index == 4)
    {
        test_base_addr = Bank1_NOR4_ADDR;
    }
    NOR_WRITE(test_base_addr, 0x00F0);
//    systick_delay_ms(1);
//    
    return (NOR_SUCCESS);
}

/**
*\*\name    FEMC_Nor_Reset.
*\*\fun     Returns the NOR memory to Read mode and resets the errors in the NOR memory Status Register.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\return  NOR_SUCCESS
**/
NOR_Status FEMC_Nor_Reset(uint8_t block_index)
{
    if (block_index == 1)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK1(0x002AA), 0x0055);
        NOR_WRITE(Bank1_NOR1_ADDR, 0x00F0);
    }
    else if (block_index == 2)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK2(0x002AA), 0x0055);
        NOR_WRITE(Bank1_NOR2_ADDR, 0x00F0);
    }
    else if (block_index == 3)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK3(0x002AA), 0x0055);
        NOR_WRITE(Bank1_NOR3_ADDR, 0x00F0);
    }
    else if (block_index == 4)
    {
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x00555), 0x00AA);
        NOR_WRITE(ADDR_SHIFT_BLOCK4(0x002AA), 0x0055);
        NOR_WRITE(Bank1_NOR4_ADDR, 0x00F0);
    }
    return (NOR_SUCCESS);
}

/**
*\*\name    FEMC_Nor_Get_Status.
*\*\fun     Returns the NOR operation status.
*\*\param   block_index:
*\*\            - 1     block 1
*\*\            - 2     block 2
*\*\            - 3     block 3
*\*\            - 4     block 4
*\*\param   time_out: NOR progamming Timeout
*\*\return  NOR_SUCCESS or NOR_TIMEOUT
**/
NOR_Status FEMC_Nor_Get_Status(uint8_t block_index, uint32_t time_out)
{
    uint16_t val1 = 0x00, val2 = 0x00;
    NOR_Status status = NOR_ONGOING;
    uint32_t test_base_addr;

    if (block_index == 1)
    {
        test_base_addr = Bank1_NOR1_ADDR;
    }
    else if (block_index == 2)
    {
        test_base_addr = Bank1_NOR2_ADDR;
    }
    else if (block_index == 3)
    {
        test_base_addr = Bank1_NOR3_ADDR;
    }
    else if (block_index == 4)
    {
        test_base_addr = Bank1_NOR4_ADDR;
    }

    /* Get the NOR memory operation status -------------------------------------*/
    while ((time_out != 0x00) && (status != NOR_SUCCESS))
    {
        time_out--;

        /* Read DQ6 and DQ5 */
        val1 = *(__IO uint16_t*)(test_base_addr);
        val2 = *(__IO uint16_t*)(test_base_addr);

        /* If DQ6 did not toggle between the two reads then return NOR_Success */
        if ((val1 & 0x0040) == (val2 & 0x0040))
        {
            return NOR_SUCCESS;
        }

        if ((val1 & 0x0020) != 0x0020)
        {
            status = NOR_ONGOING;
        }

        val1 = *(__IO uint16_t*)(test_base_addr);
        val2 = *(__IO uint16_t*)(test_base_addr);

        if ((val1 & 0x0040) == (val2 & 0x0040))
        {
            return NOR_SUCCESS;
        }
        else if ((val1 & 0x0020) == 0x0020)
        {
            return NOR_ERROR;
        }
    }

    if (time_out == 0x00)
    {
        status = NOR_TIMEOUT;
    }

    /* Return the operation status */
    return (status);
}
