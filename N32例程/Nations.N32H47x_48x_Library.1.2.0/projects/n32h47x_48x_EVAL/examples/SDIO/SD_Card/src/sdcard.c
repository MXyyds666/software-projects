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
 *\*\file sdcard.c
 *\*\author Nations
 *\*\version v1.0.0
 *\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
 **/

#include "main.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_sdio.h"
#include "n32h47x_48x_dma.h"
#include "sdcard.h"
#include "misc.h"
#include <stdio.h>

SD_CardInfoType SDCardInfo;
uint32_t CSD_Tab[4];
uint32_t CID_Tab[4];
uint32_t Ext_CSD_Tab[128];
uint32_t SD_Sts_Tab[16]; // Memory card status, part of CTRLSTS

SDIO_InitType SDIO_InitPara;

/**
 *\*\name   SD_DeInit.
 *\*\fun    DeInit the SDIO interface and reset SDIO ports.
 *\*\param  none
 *\*\return none
 */
void SD_DeInit(void)
{
    GPIO_InitType GPIO_InitPara;

    /* Disable SDIO Clock */
    SDIO_EnableClock(DISABLE);

    /* Set Power State to OFF */
    SDIO_SetPower(SDIO_POWER_CTRL_OFF);

    /* DeInitializes the SDIO peripheral */
    SDIO_DeInit();

    /* Disable the SDIO AHB Clock */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_SDIO, DISABLE);

    /* DeInit the GPIO pin */
    GPIO_InitPara.GPIO_Mode        = GPIO_MODE_ANALOG;
    GPIO_InitPara.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitPara.GPIO_Alternate   = GPIO_NO_AF;
    GPIO_InitPara.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitPara.GPIO_Current     = GPIO_DS_8mA;

    /* CMD */
    GPIO_InitPara.Pin              = SD_CMD_PIN;
    GPIO_InitPeripheral(SD_CMD_PORT, &GPIO_InitPara);

    /* CLK */
    GPIO_InitPara.Pin              = SD_CLK_PIN;
    GPIO_InitPeripheral(SD_CLK_PORT, &GPIO_InitPara);

    /* D0 */
    GPIO_InitPara.Pin              = SD_D0_PIN;
    GPIO_InitPeripheral(SD_D0_PORT, &GPIO_InitPara);

    /* D1 */
    GPIO_InitPara.Pin              = SD_D1_PIN;
    GPIO_InitPeripheral(SD_D1_PORT, &GPIO_InitPara);

    /* D2 */
    GPIO_InitPara.Pin              = SD_D2_PIN;
    GPIO_InitPeripheral(SD_D2_PORT, &GPIO_InitPara);

    /* D3 */
    GPIO_InitPara.Pin              = SD_D3_PIN;
    GPIO_InitPeripheral(SD_D3_PORT, &GPIO_InitPara);
}

/**
 *\*\name   SD_PortInit.
 *\*\fun    Init the GPIO pin used for SDIO card.
 *\*\param  none
 *\*\return none
 */
void SD_PortInit(void)
{
    GPIO_InitType GPIO_InitPara;

    /* Enable SDIO port clock */
    RCC_EnableAHB1PeriphClk(  SD_CLK_ENABLE 
                            | SD_CMD_ENABLE
                            | SD_D0_ENABLE
                            | SD_D1_ENABLE
                            | SD_D2_ENABLE
                            | SD_D3_ENABLE,
                            ENABLE);

    /* SDIO pin config */
    GPIO_InitPara.GPIO_Mode        = GPIO_MODE_AF_PP;
    GPIO_InitPara.GPIO_Slew_Rate   = GPIO_SLEW_RATE_FAST;
    GPIO_InitPara.GPIO_Pull        = GPIO_NO_PULL;
    GPIO_InitPara.GPIO_Current     = GPIO_DS_12mA;

    /* CLK */
    GPIO_InitPara.Pin              = SD_CLK_PIN;
    GPIO_InitPara.GPIO_Alternate   = SD_CLK_AF;
    GPIO_InitPeripheral(SD_CLK_PORT, &GPIO_InitPara);

    /* CMD */
    GPIO_InitPara.Pin              = SD_CMD_PIN;
    GPIO_InitPara.GPIO_Alternate   = SD_CMD_AF;
    GPIO_InitPeripheral(SD_CMD_PORT, &GPIO_InitPara);
    
    /* D0 */
    GPIO_InitPara.Pin              = SD_D0_PIN;
    GPIO_InitPara.GPIO_Alternate   = SD_D0_AF;
    GPIO_InitPeripheral(SD_D0_PORT, &GPIO_InitPara);

    /* D1 */
    GPIO_InitPara.Pin              = SD_D1_PIN;
    GPIO_InitPara.GPIO_Alternate   = SD_D1_AF;
    GPIO_InitPeripheral(SD_D1_PORT, &GPIO_InitPara);

    /* D2 */
    GPIO_InitPara.Pin              = SD_D2_PIN;
    GPIO_InitPara.GPIO_Alternate   = SD_D2_AF;
    GPIO_InitPeripheral(SD_D2_PORT, &GPIO_InitPara);

    /* D3 */
    GPIO_InitPara.Pin              = SD_D3_PIN;
    GPIO_InitPara.GPIO_Alternate   = SD_D3_AF;
    GPIO_InitPeripheral(SD_D3_PORT, &GPIO_InitPara);
}

/**
 *\*\name   NVIC_SDIO_Config
 *\*\fun    SDIO interrupt configuration by NVIC.
 *\*\param  none
 *\*\return none
 */
void NVIC_SDIO_Config(FunctionalState Cmd)
{
    NVIC_InitType NVIC_InitPara;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitPara.NVIC_IRQChannel                   = SDIO_IRQn;
    NVIC_InitPara.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitPara.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitPara.NVIC_IRQChannelCmd                = Cmd;
    NVIC_Init(&NVIC_InitPara);
}

/**
 *\*\name   SD_DMA_RxConfig
 *\*\fun    Config the DMA channel used for SDIO receiving and enable.
 *\*\param  DstBuf: Pointer to the buffer for data receiving.
 *\*\param  BufSize: Size of the buffer, count in words.
 *\*\return none
 */
void SD_DMA_RxConfig(uint32_t* DstBuf, uint32_t BufSize)
{
    DMA_InitType DMA_InitPara;

   /* Enable DMA clock */
    RCC_EnableAHBPeriphClk(SDIO_DMA_CLOCK, ENABLE);

    /* DMA Channel DeInit */
    DMA_DeInit(SDIO_DMA_CHANNEL);

   /* DMA Channel Config */
    DMA_InitPara.PeriphAddr     = (uint32_t)(&(SDIO->DATFIFO)); /* Peripheral address, SDIO data fifo */ 
    DMA_InitPara.MemAddr        = (uint32_t)DstBuf;             /* Buffer of data to be stord */ 
    DMA_InitPara.Direction      = DMA_DIR_PERIPH_SRC;           /* Peripheral as source address */ 
    DMA_InitPara.BufSize        = BufSize;                      /* Data buffer size */ 
    DMA_InitPara.PeriphInc      = DMA_PERIPH_INC_DISABLE;       /* peripheral address not increased */ 
    DMA_InitPara.MemoryInc      = DMA_MEM_INC_ENABLE;           /* Data buffer address auto increment */ 
    DMA_InitPara.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_WORD;   /* Peripheral data size is 32-bit */ 
    DMA_InitPara.MemDataSize    = DMA_MEM_DATA_WIDTH_WORD;      /* Buffer data size is 32-bit */ 
    DMA_InitPara.CircularMode   = DMA_MODE_NORMAL;              /* Circular mode disabled */ 
    DMA_InitPara.Priority       = DMA_PRIORITY_HIGH;            /* High channel priority */ 
    DMA_InitPara.Mem2Mem        = DMA_M2M_DISABLE;              /* Memory to memory mode disabled */ 
    DMA_InitPara.BURST_BYPASS   = DMA_BURST_BYPASS_ENABLE;      /* Burst mode bypass */
    DMA_InitPara.BURST_MODE     = DMA_BURST_MODE_DYNAMIC;       /* Burst mode always used */
    DMA_InitPara.BURST_LEN      = DMA_BURST_LEN_8;              /* Burst len is 1 */
    DMA_Init(SDIO_DMA_CHANNEL, &DMA_InitPara);

    /* DMA channel remap */
    DMA_RequestRemap(DMA_REMAP_SDIO_DMA,SDIO_DMA_CHANNEL,ENABLE);

    /* DMA Channel enable */
    DMA_EnableChannel(SDIO_DMA_CHANNEL, ENABLE);
}

/**
 *\*\name   SD_DMA_TxConfig
 *\*\fun    Config the DMA channel used for SDIO transmition and enable.
 *\*\param  SrcBuf: Pointer to the buffer for data send.
 *\*\param  BufSize: Size of the buffer, count in words.
 *\*\return none
 */
void SD_DMA_TxConfig(uint32_t* SrcBuf, uint32_t BufSize)
{
    DMA_InitType DMA_InitPara;

    /* Enable DMA clock */
    RCC_EnableAHBPeriphClk(SDIO_DMA_CLOCK, ENABLE);

    /* DMA Channel DeInit */
    DMA_DeInit(SDIO_DMA_CHANNEL);

    /* DMA Channel Config */
    DMA_InitPara.PeriphAddr     = (uint32_t)(&(SDIO->DATFIFO)); /* Peripheral address, SDIO data fifo */ 
    DMA_InitPara.MemAddr        = (uint32_t)SrcBuf;             /* Buffer of data to be send */ 
    DMA_InitPara.Direction      = DMA_DIR_PERIPH_DST;           /* Peripheral as target address */ 
    DMA_InitPara.BufSize        = BufSize;                      /* Data buffer size */ 
    DMA_InitPara.PeriphInc      = DMA_PERIPH_INC_DISABLE;       /* peripheral address not increased */ 
    DMA_InitPara.MemoryInc      = DMA_MEM_INC_ENABLE;           /* Data buffer address auto increment */ 
    DMA_InitPara.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_WORD;   /* Peripheral data size is 32-bit */ 
    DMA_InitPara.MemDataSize    = DMA_MEM_DATA_WIDTH_WORD;      /* Buffer data size is 32-bit */ 
    DMA_InitPara.CircularMode   = DMA_MODE_NORMAL;              /* Circular mode disabled */ 
    DMA_InitPara.Priority       = DMA_PRIORITY_HIGH;            /* High channel priority */ 
    DMA_InitPara.Mem2Mem        = DMA_M2M_DISABLE;              /* Memory to memory mode disabled */ 
    DMA_InitPara.BURST_BYPASS   = DMA_BURST_BYPASS_ENABLE;      /* Burst mode bypass */
    DMA_InitPara.BURST_MODE     = DMA_BURST_MODE_DYNAMIC;       /* Burst mode always used */
    DMA_InitPara.BURST_LEN      = DMA_BURST_LEN_8;              /* Burst len is 1 */
    DMA_Init(SDIO_DMA_CHANNEL, &DMA_InitPara);

    /* DMA channel remap */
    DMA_RequestRemap(DMA_REMAP_SDIO_DMA,SDIO_DMA_CHANNEL,ENABLE);

    /* DMA Channel enable */
    DMA_EnableChannel(SDIO_DMA_CHANNEL, ENABLE);
}

/**
 *\*\name   SD_Init
 *\*\fun    Initialize SD card to make it ready (ready to transfer data).
 *\*\param  ClockBypass: Enable or disable the clock diveder bypass function.
 *\*\param  ClkDiv: Transmission clock division factor.
 *\*\param  BusWidth: Transmission line width.
 *\*\return SD card error code,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_Init(uint32_t ClockBypass, uint32_t ClkDiv, uint32_t BusWidth)
{
    uint8_t lock_pw[16];
    uint32_t resp1;
    uint32_t error;

    /* SDIO peripheral GPIO pin initialization */
    SD_PortInit();

    /* Enable the SDIO AHB Clock */
    RCC->CFG2 |= RCC_CFG2_HCLKPRES;
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_SDIO, ENABLE);

    /*Reset all registers of SDIO*/
    SDIO_DeInit();

    /* Configure the SDIO peripheral */
    /* SDIOCLK = HCLK, SDIO_CLK = HCLK/(2 + ClkDiv) */
    /* SDIO_CLK for initialization should not exceed 400 KHz */
    SDIO_InitPara.ClkDiv            = SD_INIT_CLK_DIV;
    SDIO_InitPara.ClkEdge           = SDIO_CLK_EDGE_RISING;
    SDIO_InitPara.ClkBypass         = ClockBypass;
    SDIO_InitPara.ClkPwrSave        = SDIO_CLK_POWER_SAVE_DISABLE;
    SDIO_InitPara.BusWidth          = SDIO_BUS_WIDTH_1B;         
    SDIO_InitPara.HardwareClkCtrl   = SDIO_HARDWARE_CLKCTRL_DISABLE;
    SDIO_Init(&SDIO_InitPara);

    /* Set Power State to ON */
    SDIO_SetPower(SDIO_POWER_CTRL_ON);

    /* Enable SDIO Clock */
    SDIO_EnableClock(ENABLE);
    
    /* Delay at least 74 SDIO_CLK cycles after clock enable */
    Delay(SystemCoreClock/1000);    /* about 4ms */
    
    /*Power on and carry out card identification process to confirm the operation voltage of the card*/
    error = SD_PowerON();

    /*If the power is on, the recognition is not successful, and the "response timeout" error is returned*/
    if (error != SDMMC_ERROR_NONE)
    {
        /* CMDCTRL Response TimeOut (wait for CMDSEND flag) */
        return (error);
    }

    /* Card identification successful, card initialization*/
    error = SD_InitCard();
    if (error != SDMMC_ERROR_NONE)
    {
        /* CMDCTRL Response TimeOut (wait for CMDSEND flag) */
        return (error);
    }

    /* Configure the SDIO peripheral after power on identification and card initialization,
       enter data transmission mode to improve reading and writing speed */
    SDIO_InitPara.ClkDiv  = ClkDiv; 
    SDIO_InitPara.ClkEdge = SDIO_CLK_EDGE_RISING;
    SDIO_InitPara.ClkBypass = ClockBypass;
    SDIO_InitPara.ClkPwrSave        = SDIO_CLK_POWER_SAVE_DISABLE;
    SDIO_InitPara.BusWidth          = SDIO_BUS_WIDTH_1B;
    SDIO_InitPara.HardwareClkCtrl   = SDIO_HARDWARE_CLKCTRL_DISABLE;
    SDIO_Init(&SDIO_InitPara);

    error = SDIO_CmdSelDesel(SDCardInfo.RelCardAdd); /* Select Card through cmd7 */
    if (error != SDMMC_ERROR_NONE)
    {
        /* CMDCTRL Response TimeOut (wait for CMDSEND flag) */
        return (error);
    }

    error = SDIO_CmdBlockLength(SD_BLOCKSIZE);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    error = SD_GetCardInfo(&SDCardInfo);
    if(error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    while(SD_GetCardState() != SDMMC_CARD_TRANSFER);

    /* Read MMC ext CSD */
    if(     (CARD_MMC_HC == SDCardInfo.CardType)
       ||  (CARD_MMC_LC == SDCardInfo.CardType)    )   /*MMC*/
    {
       error = MMC_GetExtCSD(Ext_CSD_Tab,SDMMC_DATATIMEOUT);
       if (SDMMC_ERROR_NONE != error)
       {
           return (error);
       }
    }

    /* Check whether the card has been locked and unlocked */
     if (SDIO_GetResp(SDIO_RESPONSE_1) & SDMMC_R1_CARD_LOCKED)
     {
        lock_pw[0] = SDMMC_LOCK_DATA_CLR_PWD;
        lock_pw[1] = 14;

        lock_pw[2] = 0x1;
        lock_pw[3] = 0x2;
        lock_pw[4] = 0x3;
        lock_pw[5] = 0x4;
        lock_pw[6] = 0x5;
        lock_pw[7] = 0x6;
        lock_pw[8] = 0x7;
        lock_pw[9] = 0x8;

        lock_pw[10] = 0x1;
        lock_pw[11] = 0x2;
        lock_pw[12] = 0x3;
        lock_pw[13] = 0x4;
        lock_pw[14] = 0x5;
        lock_pw[15] = 0x6;
         
        error = SD_ClearPWD(lock_pw,16U,SDMMC_DATATIMEOUT);
        if (error != SDMMC_ERROR_NONE)
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (error);
        }
        
        while(1)
        {
            error = SD_SendCardStatus(&resp1);
            if (error != SDMMC_ERROR_NONE)
            {
                /* Clear all the static flags */
                SDIO_ClrFlag(SDIO_STATIC_FLAGS);
                return (error);
            }
            
            if((resp1 & SDMMC_R1_LOCK_UNLOCK_FAILED) == SDMMC_R1_LOCK_UNLOCK_FAILED)
            {
                return (SDMMC_ERROR_LOCK_UNLOCK_FAILED);
            }
            
            if(((resp1 >> 9U) & 0x0FU) != SDMMC_CARD_TRANSFER)
            {
                continue;
            }
            
            if((resp1 & SDMMC_R1_CARD_LOCKED) != SDMMC_R1_CARD_LOCKED)
            {
                break;
            }
            
            Delay(0xfff);
        }
    }

    while(1)
    {
        Delay(0xFFFF);
        error = SDIO_CmdBlockLength(SD_BLOCKSIZE);
        if (error == SDMMC_ERROR_NONE)
        {
            break;
        }
    }
    
    error = SD_CofigBusWidth(BusWidth);
    if (error != SDMMC_ERROR_NONE)
    {
        /* CMDCTRL Response TimeOut (wait for CMDSEND flag) */
        return (error);
    }

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_GetCardState
 *\*\fun    Gets the current sd card state.
 *\*\param  none
 *\*\return Current card state
 */
uint32_t SD_GetCardState(void)
{
    uint32_t resp1 = 0;

    if (SD_SendCardStatus(&resp1) != SDMMC_ERROR_NONE)
    {
        return SDMMC_CARD_ERROR;
    }
    else
    {
        return ((resp1 >> 9U) & 0x0FU);
    }
}

/**
 *\*\name   SD_PowerON
 *\*\fun    Gets the current sd card state.
 *\*\param  none
 *\*\return Current card state
 */
uint32_t SD_PowerON(void)
{
    uint32_t error;
    uint32_t resp;
    uint32_t count;
    uint32_t validvoltage;

    /*Next, send a series of commands to start the card identification process*/
    /* CMD0: GO_IDLE_STATE. No CMDCTRL response required */
    error = SDIO_CmdGoIdleState();
    if (error != SDMMC_ERROR_NONE)
    {
        /* CMDCTRL Response TimeOut (wait for CMDSEND flag) */
        return (error);
    }
    
    /* Required power up waiting time before starting the SD initialization  sequence */
    Delay(SystemCoreClock/1000);    /* about 4ms */
    
    /* CMD8: SEND_IF_COND */
    /* Send CMD8 to verify SD card interface operating condition */
    /* Argument: - [31:12]: Reserved (shall be set to '0')
                 - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
                 - [7:0]: Check Pattern (recommended 0xAA) */
    /* CMDCTRL Response: R7 */
    error = SDIO_CmdOperCond();
    if (error == SDMMC_ERROR_NONE) // If there is a response, card follows SD protocol version 2.0
    {
        SDCardInfo.CardVersion = CARD_V2_X;
    }
    else // No response, indicates 1.X or MMC card
    {
        SDCardInfo.CardVersion = CARD_V1_X;
    }

    validvoltage = 0;
    count = 0;

    /* Send cmd55 to detect SD card, MMC card, or unsupported card */
    /* If errorstatus is Command TimeOut, it is a MMC card */
    /* If errorstatus is SDMMC_ERROR_NONE it is a SD card: SD card 2.0 (voltage range mismatch)
       or SD card 1.x */
    error = SDIO_CmdAppCommand(0U);
    if (error == SDMMC_ERROR_NONE) // Responded to cmd55, SD card, may be 1.X, may be 2.0
    {
        /*Start to send the voltage range supported by SDIO circularly for a certain number of times*/
        /* Send ACMD41 SD_APP_OP_COND with Argument */
        while ((!validvoltage) && (count < SDMMC_MAX_VOLT_TRIAL))
        {
            /* Because acmd41 is ACMD command. So cmd55 must send to card first */
            /* SEND CMD55 APP_CMD with RCA as 0 */
            error = SDIO_CmdAppCommand(0U);
            if (error != SDMMC_ERROR_NONE)
            {
                return (error); // No response to cmd55, return
            }

            // acmd41,The command parameters consist of the supported voltage range and HCs bits. The HCS position is
            // used to distinguish whether the card is SDSC or SDHC
            error = SDIO_CmdAppOperCommand(SDMMC_VOLTAGE_WINDOW_SD | SDMMC_HIGH_CAPACITY);
            if (error != SDMMC_ERROR_NONE)
            {
                return (error); // Acmd41 not received correctly, error, return
            }

            /*If the card demand voltage is within the supply voltage range of SDIO, it will automatically power on and
             * mark PWR UP bit*/
            resp = SDIO_GetResp(SDIO_RESPONSE_1);
            validvoltage = (resp & SDMMC_CARD_POWER_UP); // Read the PWR up bit of the card's OCR register to see if it is working at normal voltage
            count++;       // Calculate number of cycles
        }

        if (count >= SDMMC_MAX_VOLT_TRIAL) // Cycle detection is over a certain number of times and the power is not on
        {
            return (SDMMC_ERROR_INVALID_VOLTRANGE);  // SDIO does not support card supply voltage
        }

        /*check HCS bit in card return information*/
        if ((resp & SDMMC_HIGH_CAPACITY) == SDMMC_HIGH_CAPACITY) // Judge the CCS bit in OCR.
        {
            SDCardInfo.CardType = CARD_SDHC_SDXC; // Change card type from initial SDSC type to SDHC type
        }
        else
        {
            SDCardInfo.CardType = CARD_SDSC; // Change card type from initial SDSC type to SDHC type
        }
    }
    else    /* else MMC Card */
    {
        while ((!validvoltage) && (count < SDMMC_MAX_VOLT_TRIAL))
        {
            /* CMD1 */
            error = SDIO_CmdOpCondition(SDMMC_VOLTAGE_WINDOW_MMC);
            if (error != SDMMC_ERROR_NONE)
            {
                return (error); // CMD1 not received correctly, error, return
            }

            /*If the card demand voltage is within the supply voltage range of SDIO, it will automatically power on and
             * mark PWR UP bit*/
            resp = SDIO_GetResp(SDIO_RESPONSE_1); // Read card register, card status
            validvoltage = (resp & SDMMC_CARD_POWER_UP); // Read the PWR up bit of the card's OCR register to see if it is working at normal voltage
            count++;       // Calculate number of cycles
        }

        if (count >= SDMMC_MAX_VOLT_TRIAL) // Cycle detection is over a certain number of times and the power is not on
        {
            return (SDMMC_ERROR_INVALID_VOLTRANGE);  // SDIO does not support card supply voltage
        }

        /* When power routine is finished and command returns valid voltage */
        if(((resp & 0xFF000000U) >> 24U) == 0xC0U)
        {
            SDCardInfo.CardType = CARD_MMC_HC;
        }
        else
        {
            SDCardInfo.CardType = CARD_MMC_LC;
        }
    }

    return (error);
}


/**
 *\*\name   SD_InitCard
 *\*\fun    Initialize all cards or single card to ready state
 *\*\param  none
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_InitCard(void)
{
    uint32_t error;
    uint16_t rca;

    if (SDIO_GetPower() == SDIO_POWER_CTRL_OFF)
    {
        return (SDMMC_ERROR_REQUEST_NOT_APPLICABLE);
    }

    if (CARD_SDIO != SDCardInfo.CardType) // judgment card Type
    {
        /* CMD2 */
        error = SDIO_CmdAllSendCID();
        if (SDMMC_ERROR_NONE != error)
        {
            return (error);
        }

        CID_Tab[0] = SDIO_GetResp(SDIO_RESPONSE_1);
        CID_Tab[1] = SDIO_GetResp(SDIO_RESPONSE_2);
        CID_Tab[2] = SDIO_GetResp(SDIO_RESPONSE_3);
        CID_Tab[3] = SDIO_GetResp(SDIO_RESPONSE_4);
    }
    
    if(     (CARD_MMC_HC == SDCardInfo.CardType)
        ||  (CARD_MMC_LC == SDCardInfo.CardType)    )   /*MMC*/
    {
        rca = 0x5AU;
        /* Send CMD3 SET_REL_ADDR with argument 5a */
        /* MMC Card accept the RCA. */
        error = SDIO_CmdSetRelAddMmc(rca);
    }
    else if (CARD_SDIO != SDCardInfo.CardType)   /*Start the SD card initialization process*/
    {
        /* Send CMD3 SET_REL_ADDR with argument 0 */
        /* SD Card publishes its RCA. */
        rca = 0x01U;
        error = SDIO_CmdSetRelAdd(&rca);
    }
    else
    {
        error = SDMMC_ERROR_UNSUPPORTED_HW;
    }

    if(     (SDMMC_ERROR_NONE != error)
        &&  (SDMMC_ERROR_ILLEGAL_CMD != error) )
    {
        return (error);
    }

    SDCardInfo.RelCardAdd = rca;

    /* Send CMD9 SEND_CSD with rca */
    error = SDIO_CmdSendCSD(rca);
    if (SDMMC_ERROR_NONE != error)
    {
        return (error);
    }

    CSD_Tab[0] = SDIO_GetResp(SDIO_RESPONSE_1);
    CSD_Tab[1] = SDIO_GetResp(SDIO_RESPONSE_2);
    CSD_Tab[2] = SDIO_GetResp(SDIO_RESPONSE_3);
    CSD_Tab[3] = SDIO_GetResp(SDIO_RESPONSE_4);

    SDCardInfo.Class = (SDIO_GetResp(SDIO_RESPONSE_2) >> 20U);

    // test CMD10
    error = SDIO_CmdSendCID(rca);
    if (SDMMC_ERROR_NONE != error)
    {
        return (error);
    }

    if(     (CID_Tab[0] != SDIO_GetResp(SDIO_RESPONSE_1))
        ||  (CID_Tab[1] != SDIO_GetResp(SDIO_RESPONSE_2))
        ||  (CID_Tab[2] != SDIO_GetResp(SDIO_RESPONSE_3))
        ||  (CID_Tab[3] != SDIO_GetResp(SDIO_RESPONSE_4))   )
    {
        return (SDMMC_ERROR_UNSUPPORTED_HW);
    }
    
    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_GetCardCID
 *\*\fun    Returns the information of the card which are stored on the CID register.
 *\*\param  pCID: Pointer to a SD_CID_InfoType structure that contains CID parameters.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_GetCardCID(SD_CID_InfoType *pCID)
{
    pCID->ManufacturerID  = (uint8_t) ((CID_Tab[0] & 0xFF000000U) >> 24U);
    pCID->OEM_AppliID     = (uint16_t)((CID_Tab[0] & 0x00FFFF00U) >> 8U);
    pCID->ProdName1       = (((CID_Tab[0] & 0x000000FFU) << 24U)  \
                        |  ((CID_Tab[1] & 0xFFFFFF00U) >> 8U)   );
    pCID->ProdName2       = (uint8_t)( CID_Tab[1] & 0x000000FFU);
    pCID->ProdRev         = (uint8_t)((CID_Tab[2] & 0xFF000000U) >> 24U);
    pCID->ProdSN          = (((CID_Tab[2] & 0x00FFFFFFU) << 8U)   \
                        |  ((CID_Tab[3] & 0xFF000000U) >> 24U)  );
    pCID->Reserved1       = (uint8_t)((CID_Tab[3] & 0x00F00000U) >> 20U);
    pCID->ManufactDate    = (uint16_t)((CID_Tab[3] & 0x000FFF00U) >> 8U);
    pCID->CID_CRC         = (uint8_t)((CID_Tab[3] & 0x000000FEU) >> 1U);
    pCID->Reserved2       = 1U;

    return SDMMC_ERROR_NONE;
}

/**
 *\*\name   SD_GetCardCSD
 *\*\fun    Returns the information of the card which are stored on the CSD register.
 *\*\param  pCSD: Pointer to a SD_CSD_InfoType structure that contains CSD parameters.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_GetCardCSD(SD_CSD_InfoType *pCSD)
{
    pCSD->CSDStruct       = (uint8_t) ((CSD_Tab[0] & 0xC0000000U) >> 30U);
    pCSD->SysSpecVersion  = (uint8_t) ((CSD_Tab[0] & 0x3C000000U) >> 26U);
    pCSD->Reserved1       = (uint8_t) ((CSD_Tab[0] & 0x03000000U) >> 24U);
    pCSD->TAAC            = (uint8_t) ((CSD_Tab[0] & 0x00FF0000U) >> 16U);
    pCSD->NSAC            = (uint8_t) ((CSD_Tab[0] & 0x0000FF00U) >> 8U);
    pCSD->MaxBusClkFrec   = (uint8_t) (CSD_Tab[0]  & 0x000000FFU);
    pCSD->CardComdClasses = (uint16_t)((CSD_Tab[1] & 0xFFF00000U) >> 20U);
    pCSD->RdBlockLen      = (uint8_t) ((CSD_Tab[1] & 0x000F0000U) >> 16U);
    pCSD->PartBlockRead   = (uint8_t) ((CSD_Tab[1] & 0x00008000U) >> 15U);
    pCSD->WrBlockMisalign = (uint8_t) ((CSD_Tab[1] & 0x00004000U) >> 14U);
    pCSD->RdBlockMisalign = (uint8_t) ((CSD_Tab[1] & 0x00002000U) >> 13U);
    pCSD->DSRImpl         = (uint8_t) ((CSD_Tab[1] & 0x00001000U) >> 12U);
    pCSD->Reserved2       = 0U; /* Reserved */

    if(   (SDCardInfo.CardType == CARD_SDSC)       \
        ||(SDCardInfo.CardType == CARD_MMC_HC)     \
        ||(SDCardInfo.CardType == CARD_MMC_LC)     )
    {
        pCSD->DeviceSize         = (((CSD_Tab[1] & 0x000003FFU) << 2U)   \
                                |   ((CSD_Tab[2] & 0xC0000000U) >> 30U) );
        pCSD->MaxRdCurrentVDDMin = (uint8_t)((CSD_Tab[2] & 0x38000000U) >> 27U);
        pCSD->MaxRdCurrentVDDMax = (uint8_t)((CSD_Tab[2] & 0x07000000U) >> 24U);
        pCSD->MaxWrCurrentVDDMin = (uint8_t)((CSD_Tab[2] & 0x00E00000U) >> 21U);
        pCSD->MaxWrCurrentVDDMax = (uint8_t)((CSD_Tab[2] & 0x001C0000U) >> 18U);
        pCSD->DeviceSizeMul      = (uint8_t)((CSD_Tab[2] & 0x00038000U) >> 15U);
    }
    else if(SDCardInfo.CardType == CARD_SDHC_SDXC)
    {
        /* Byte 7 */
        pCSD->DeviceSize = (((CSD_Tab[1] & 0x0000003FU) << 16U)     \
                        |   ((CSD_Tab[2] & 0xFFFF0000U) >> 16U));
    }
    else
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_UNSUPPORTED_HW);
    }

    pCSD->EraseGrSize         = (uint8_t)((CSD_Tab[2] & 0x00004000U) >> 14U);
    pCSD->EraseGrMul          = (uint8_t)((CSD_Tab[2] & 0x00003F80U) >> 7U);
    pCSD->WrProtectGrSize     = (uint8_t)(CSD_Tab[2] & 0x0000007FU);
    pCSD->WrProtectGrEnable   = (uint8_t)((CSD_Tab[3] & 0x80000000U) >> 31U);
    pCSD->ManDeflECC          = (uint8_t)((CSD_Tab[3] & 0x60000000U) >> 29U);
    pCSD->WrSpeedFact         = (uint8_t)((CSD_Tab[3] & 0x1C000000U) >> 26U);
    pCSD->MaxWrBlockLen       = (uint8_t)((CSD_Tab[3] & 0x03C00000U) >> 22U);
    pCSD->WriteBlockPaPartial = (uint8_t)((CSD_Tab[3] & 0x00200000U) >> 21U);
    pCSD->Reserved3           = 0U;
    pCSD->ContentProtectAppli = (uint8_t)((CSD_Tab[3] & 0x00010000U) >> 16U);
    pCSD->FileFormatGroup     = (uint8_t)((CSD_Tab[3] & 0x00008000U) >> 15U);
    pCSD->CopyFlag            = (uint8_t)((CSD_Tab[3] & 0x00004000U) >> 14U);
    pCSD->PermWrProtect       = (uint8_t)((CSD_Tab[3] & 0x00002000U) >> 13U);
    pCSD->TempWrProtect       = (uint8_t)((CSD_Tab[3] & 0x00001000U) >> 12U);
    pCSD->FileFormat          = (uint8_t)((CSD_Tab[3] & 0x00000C00U) >> 10U);
    pCSD->ECC                 = (uint8_t)((CSD_Tab[3] & 0x00000300U) >> 8U);
    pCSD->CSD_CRC             = (uint8_t)((CSD_Tab[3] & 0x000000FEU) >> 1U);
    pCSD->Reserved4           = 1U;

    return SDMMC_ERROR_NONE;
}

/**
 *\*\name   SD_GetCardInfo
 *\*\fun    Get SD card information
 *\*\param  cardinfo The pointer to the SD_CardInfoType structure contains the specific information of the SD card
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_GetCardInfo(SD_CardInfoType* pCardInfo)
{
    uint32_t error;
    uint32_t block_nbr;

    /* Get CID infomation */
    error = SD_GetCardCID(&(pCardInfo->CID));
    if(SDMMC_ERROR_NONE != error)
    {
        return error;
    }
    
    /* Get CSD infomation */
    error = SD_GetCardCSD(&(pCardInfo->CSD));
    if(SDMMC_ERROR_NONE != error)
    {
        return error;
    }

    /* Read EXT_CSD for MMC card */
    if(   (SDCardInfo.CardType == CARD_MMC_HC)     \
        ||(SDCardInfo.CardType == CARD_MMC_LC)     )
    {
        error = MMC_ReadExtCSD(&block_nbr,212,0x0FFFFFFFU);
        if(error != SDMMC_ERROR_NONE)
        {
            return (error);
        }
    }

    /* Calculate the card size */
    if(SDCardInfo.CardType == CARD_MMC_HC)
    {
        pCardInfo->BlockNbr     = block_nbr;
        pCardInfo->LogBlockNbr  = pCardInfo->BlockNbr;
        pCardInfo->BlockSize    = 512U;
        pCardInfo->LogBlockSize = pCardInfo->BlockSize;
    }
    else if(SDCardInfo.CardType == CARD_SDHC_SDXC)
    {
        pCardInfo->BlockNbr     = ((pCardInfo->CSD.DeviceSize + 1U) * 1024U);
        pCardInfo->LogBlockNbr  = pCardInfo->BlockNbr;
        pCardInfo->BlockSize    = 512U;
        pCardInfo->LogBlockSize = pCardInfo->BlockSize;
    }
    else    /*CARD_SDSC & CARD_MMC_LC*/
    {
        pCardInfo->BlockNbr     = (pCardInfo->CSD.DeviceSize + 1U) ;
        pCardInfo->BlockNbr    *= (1UL << ((pCardInfo->CSD.DeviceSizeMul & 0x07U) + 2U));
        pCardInfo->BlockSize    = (1UL << (pCardInfo->CSD.RdBlockLen & 0x0FU));
        pCardInfo->LogBlockNbr  = (pCardInfo->BlockNbr) * ((pCardInfo->BlockSize) / 512U);
        pCardInfo->LogBlockSize = 512U; 
    }
        
    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_ClearPWD
 *\*\fun    Clear the card password if it is locked.
 *\*\param  buf The pointer to the data buffer contains the password
 *\*\param  BlockSize The size of the data buffer
 *\*\param  Timeout: Specify timeout value, can not be 0.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_ClearPWD(uint8_t *buf, uint32_t BlockSize, uint32_t Timeout)
{
    uint32_t error;
    uint32_t cnt;
    uint32_t timeout;
    uint32_t *pSrc;
    SDIO_DataInitType InitData;
    
    if (BlockSize == 1)
    {
        BlockSize = 4;
    }
    
    if(Timeout == 0)
    {
        return (SDMMC_ERROR_INVALID_PARAMETER);
    }
    
    while(1)
    {
        error = SDIO_CmdBlockLength(BlockSize);
        if (error == SDMMC_ERROR_NONE)
        {
            break;
        }
    }
    
    /* Reset data control register */
    SDIO->DATCTRL = 0x0U;

    /* Config data control register*/
    InitData.DatTimeout        = SDMMC_DATATIMEOUT;
    InitData.DatLen            = BlockSize;
    
    for(cnt=0; cnt<31; cnt++)
    {
        if((1 << cnt) == BlockSize)
        {
            InitData.DatBlkSize = (uint32_t)(cnt << 8);
            break;
        }
    }
    InitData.TransferDirection = SDIO_TRANSDIR_TOCARD;
    InitData.TransferMode      = SDIO_TRANSMODE_BLOCK;
    InitData.DPSMConfig        = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(&InitData);
    
    /* Send CMD42 */
    error = SDIO_CmdLockUnlock(0U);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }
    
    /* Send data */
    timeout = Timeout;
    cnt = 0;
    pSrc = (uint32_t *)buf;
    while(SDIO_GetFlag(SDIO_FLAG_TXURERR    \
                    |  SDIO_FLAG_DCRCERR    \
                    |  SDIO_FLAG_DATTIMEOUT \
                    |  SDIO_FLAG_DATEND     \
                    |  SDIO_FLAG_SBERR) == RESET)
    {
        if(SDIO_GetFlag(SDIO_FLAG_TFIFOF) == RESET)
        {
            if(cnt < (BlockSize/4))
            {
                SDIO_WriteData(pSrc[cnt]);
                cnt++;
            }
        }

        if(0U == timeout )
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }
    
    /* Reset data control register */
    SDIO->DATCTRL = 0x0U;

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_GetCardStatus
 *\*\fun    Get SD card status
 *\*\param  pCardSts The pointer to the SD_CardStatusType structure contains the status of the SD card
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_GetCardStatus(SD_CardStatusType *pCardSts)
{
    uint32_t error;

    error = SD_SendSDStatus((uint32_t*)SD_Sts_Tab);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    pCardSts->DataBusWidth          = (uint8_t)((SD_Sts_Tab[0] & 0xC0U) >> 6U);
    pCardSts->SecuredMode           = (uint8_t)((SD_Sts_Tab[0] & 0x20U) >> 5U);
    pCardSts->CardType              = (uint16_t)( ((SD_Sts_Tab[0] & 0x00FF0000U) >> 8U)     \
                                                | ((SD_Sts_Tab[0] & 0xFF000000U) >> 24U)    );
    pCardSts->ProtectedAreaSize     = (((SD_Sts_Tab[1] & 0x000000FFU) << 24U)    \
                                    |  ((SD_Sts_Tab[1] & 0x0000FF00U) <<  8U)    \
                                    |  ((SD_Sts_Tab[1] & 0x00FF0000U) >>  8U)    \
                                    |  ((SD_Sts_Tab[1] & 0xFF000000U) >> 24U)    );
    pCardSts->SpeedClass            = (uint8_t)(SD_Sts_Tab[2]  & 0xFFU);
    pCardSts->PerformanceMove       = (uint8_t)((SD_Sts_Tab[2] & 0xFF00U) >> 8U);
    pCardSts->AllocationUnitSize    = (uint8_t)((SD_Sts_Tab[2] & 0xF00000U) >> 20U);
    pCardSts->EraseSize             = (uint16_t)( ((SD_Sts_Tab[2] & 0xFF000000U) >> 16U) \
                                                | (SD_Sts_Tab[3]   & 0x000000FFU)    );
    pCardSts->EraseTimout          = (uint8_t)((SD_Sts_Tab[3] & 0xFC00U) >> 10U);
    pCardSts->EraseOffset           = (uint8_t)((SD_Sts_Tab[3] & 0x0300U) >> 8U);

    /* Set block size for card if it is not equal to current block size for card. */
    error = SDIO_CmdBlockLength(SD_BLOCKSIZE);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_CofigBusWidth
 *\*\fun    Configure the data width of the card (depending on whether the card supports it)
 *\*\param  WideMode Specifies the SD card wide bus mode.
 *\*\           - SDIO_BUS_WIDTH_1B 1-bit data transfer
 *\*\           - SDIO_BUS_WIDTH_4B 4-bit data transfer
 *\*\           - SDIO_BUS_WIDTH_8B 8-bit data transfer (Only for MMC)
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_CofigBusWidth(uint32_t WideMode)
{
    uint32_t error;

    /* MMC Card */
    if(   (CARD_MMC_HC == SDCardInfo.CardType)  \
        ||(CARD_MMC_LC == SDCardInfo.CardType)  )
    {
        error = MMC_SetWideBus(WideMode); // Use acmd6 to set the bus width and the transmission mode of the card
        if (SDMMC_ERROR_NONE != error)
        {
            return (error);
        }

        /* Configure the SDIO peripheral */
        SDIO_InitPara.BusWidth = WideMode;
        SDIO_Init(&SDIO_InitPara);
    }
    else if(CARD_SDIO != SDCardInfo.CardType)
    {
        if (SDIO_BUS_WIDTH_8B == WideMode) // 2.0 SD does not support 8bits
        {
            return (SDMMC_ERROR_UNSUPPORTED_FEATURE);
        }
        else if (SDIO_BUS_WIDTH_4B == WideMode) // 4 data line mode
        {
            error = SD_EnableWideBus(ENABLE); // Use acmd6 to set the bus width and the transmission mode of the card
            if (SDMMC_ERROR_NONE != error)
            {
                return error;
            }

            SDIO_InitPara.BusWidth = SDIO_BUS_WIDTH_4B;
            SDIO_Init(&SDIO_InitPara);
        }
        else // Single data line mode
        {
            error = SD_EnableWideBus(DISABLE); // Use acmd6 to set the bus width and the transmission mode of the card
            if (SDMMC_ERROR_NONE != error)
            {
                return error;
            }

            SDIO_InitPara.BusWidth = SDIO_BUS_WIDTH_1B;
            SDIO_Init(&SDIO_InitPara);
        }
    }

    error = SDIO_CmdBlockLength(SD_BLOCKSIZE);
    if(error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_ReadBlocks
 *\*\fun    Reads block(s) from a specified address in a card.
 *\*\param  ReadBuf pointer to the buffer that will contain the received data.
 *\*\param  BlockAddr Block Address from where data is to be read,not Byte address.
 *\*\param  NumberOfBlocks number of blocks to be read.
 *\*\param  Timeout: Specify timeout value, can not be 0.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 *\*\note This function should be followed by a check on the card state through SD_GetCardState.
 */
uint32_t SD_ReadBlocks(uint8_t* ReadBuf, uint32_t BlockAddr, uint32_t NumberOfBlocks, uint32_t Timeout)
{
    uint32_t error;
    uint32_t ReadAddr;
    uint32_t timeout;
#if defined(SD_POLLING_MODE)
    uint32_t count;
    uint32_t *pBuf;
#endif
    SDIO_DataInitType InitData;
    
    /* Check nunber of blocks to be read */
    if(NumberOfBlocks < 1)
    {
        return (SDMMC_ERROR_INVALID_PARAMETER);
    }
    
    /* Check the range of data */
    if((BlockAddr + NumberOfBlocks) > SDCardInfo.LogBlockNbr)
    {
        return (SDMMC_ERROR_ADDR_OUT_OF_RANGE);
    }
    
    /* Get the read addreass */
    if(   (SDCardInfo.CardType == CARD_SDHC_SDXC)     \
        ||(SDCardInfo.CardType == CARD_MMC_HC)   ) // ReadAddr of the high capacity card is block(512bytes) number, not byte address
    {
        ReadAddr = BlockAddr;
    }
    else
    {
        ReadAddr = BlockAddr * 512U;   // ReadAddr of the standard capacity card is byte address
    }

    /* Clear all static flags */
    while(SDIO_GetFlag(SDIO_STATIC_FLAGS) != RESET)
    {
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    }
    
    /* Reset data control register */
    SDIO->DATCTRL = 0x0;

    /* Set block size to 512Bytes */
    error = SDIO_CmdBlockLength(SD_BLOCKSIZE);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }
    
    /* Check card state */
    while(SD_GetCardState() != SDMMC_CARD_TRANSFER);
    
    /* Clear all static flags */
    while(SDIO_GetFlag(SDIO_STATIC_FLAGS) != RESET)
    {
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    }
    
    /* Config data register */
    InitData.DatTimeout        = SDMMC_DATATIMEOUT;
    InitData.DatLen            = NumberOfBlocks * SD_BLOCKSIZE;
    InitData.DatBlkSize        = SDIO_DATBLK_SIZE_512B;
    InitData.TransferDirection = SDIO_TRANSDIR_TOSDIO;
    InitData.TransferMode      = SDIO_TRANSMODE_BLOCK;
    InitData.DPSMConfig        = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(&InitData);

    /* Send command for reading */
    if(NumberOfBlocks > 1U)
    {
        error = SDIO_CmdReadMultiBlock(ReadAddr);
    }
    else
    {
        error = SDIO_CmdReadSingleBlock(ReadAddr);
    }
    
    if(error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }
#if defined(SD_DMA_MODE)
    SD_DMA_RxConfig((uint32_t*)ReadBuf, ((NumberOfBlocks * SD_BLOCKSIZE)/4));
    SDIO_DMACmd(ENABLE);

    timeout = Timeout;
    while(1)
    {
        if( (DMA_GetFlagStatus(SDIO_DMA_ENDFLAG,SDIO_DMA) == SET)
           &&(SDIO_GetFlag(SDIO_FLAG_DATEND) == SET)    )
        {
            break;
        }

        if(0U == timeout )
        {
            break;
        }
        else
        {
            timeout--;
        }
    }

    SDIO_DMACmd(DISABLE);

    if(0U == timeout )
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_TIMEOUT);
    }
    
#elif defined(SD_POLLING_MODE)
    timeout = Timeout;
    pBuf = (uint32_t *)ReadBuf;
    
    while(SDIO_GetFlag(SDIO_FLAG_RXORERR    \
                    |  SDIO_FLAG_DCRCERR    \
                    |  SDIO_FLAG_DATTIMEOUT \
                    |  SDIO_FLAG_DATEND     \
                    |  SDIO_FLAG_SBERR) == RESET)
    {
        if(SDIO_GetFlag(SDIO_FLAG_RFIFOHF) != RESET)
        {
            for(count = 0; count < 8; count++)
            {
                *(pBuf + count) = SDIO_ReadData();
            }

            pBuf += 8;
        }
        
        if(0U == timeout )
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }
#endif

    /* Send stop transmission command in case of multiblock read */
    if((SDIO_GetFlag(SDIO_FLAG_DATEND)!= RESET) && (NumberOfBlocks > 1U))
    {
        if(SDCardInfo.CardType != CARD_SDIO)
        {
            error = SDIO_CmdStopTransfer();
            if(error != SDMMC_ERROR_NONE)
            {
                /* Clear all the static flags */
                SDIO_ClrFlag(SDIO_STATIC_FLAGS);
                return (error);
            }
        }
    }

    if(SDIO_GetFlag(SDIO_FLAG_DATTIMEOUT) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_DATA_TIMEOUT);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_DCRCERR) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_DATA_CRC_FAIL);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_RXORERR) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_RX_OVERRUN);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_SBERR) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_START_BIT_ERR);
    }
    else
    {
        /* Do nothing */
    }

#if defined(SD_POLLING_MODE)
    /* Empty FIFO if there is still any data */
    while (SDIO_GetFlag(SDIO_FLAG_RDATVALID) != RESET)
    {
        *pBuf = SDIO_ReadData();
        pBuf++;
      
        if(0U == timeout )
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }
#endif

    /* Reset data control register */
    SDIO->DATCTRL = 0x0U;

    /* Clear all the static flags */
    SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    
    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_WriteBlocks
 *\*\fun    Write block(s) from a specified address in a card.
 *\*\param  SrcBuf pointer to the buffer that will contain the data to transmit.
 *\*\param  BlockAddr Block Address where data will be written,not Byte address.
 *\*\param  NumberOfBlocks number of blocks to be write.
 *\*\param  Timeout: Specify timeout value, can not be 0.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 *\*\note This function should be followed by a check on the card state through SD_GetCardState.
 */
uint32_t SD_WriteBlocks(uint8_t* SrcBuf, uint32_t BlockAddr, uint32_t NumberOfBlocks, uint32_t Timeout)
{
    uint32_t error;
    uint32_t WriteAddr;
    uint32_t timeout;
#if defined(SD_POLLING_MODE)
    uint32_t count;
    uint32_t *pSrc;
#endif
    SDIO_DataInitType InitData;
    
    /* Check nunber of blocks to be write */
    if(NumberOfBlocks < 1)
    {
        return (SDMMC_ERROR_INVALID_PARAMETER);
    }
    
    /* Check the range of data */
    if((BlockAddr + NumberOfBlocks) > SDCardInfo.LogBlockNbr)
    {
        return (SDMMC_ERROR_ADDR_OUT_OF_RANGE);
    }
    
    /* Get the write addreass */
    if(   (SDCardInfo.CardType == CARD_SDHC_SDXC)     \
        ||(SDCardInfo.CardType == CARD_MMC_HC)   ) // WriteAddr of the high capacity card is block(512bytes) number, not byte address
    {
        WriteAddr = BlockAddr;
    }
    else
    {
        WriteAddr = BlockAddr * 512U;   // WriteAddr of the standard capacity card is byte address
    }
    
    /* Clear all static flags */
    while(SDIO_GetFlag(SDIO_STATIC_FLAGS) != RESET)
    {
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    }
    
    /* Reset data control register */
    SDIO->DATCTRL = 0x0;
    
    /* Set block size to 512Bytes */
    error = SDIO_CmdBlockLength(SD_BLOCKSIZE);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }
    
    /* Set nunber of blocks for MMC */
    if( (SDCardInfo.CardType != CARD_MMC_HC)    \
      &&(SDCardInfo.CardType != CARD_MMC_LC)    )
    {
        /* Send CMD55 */
        error = SDIO_CmdAppCommand((uint32_t)SDCardInfo.RelCardAdd << 16U);
        if (error != SDMMC_ERROR_NONE)
        {
            return (error);
        }
        
        error = SDIO_CmdBlockCount(NumberOfBlocks);
        if(error != SDMMC_ERROR_NONE)
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (error);
        }
    }
    
    /* Check card state */
    while(SD_GetCardState() != SDMMC_CARD_TRANSFER);
    
    /* Clear all static flags */
    while(SDIO_GetFlag(SDIO_STATIC_FLAGS) != RESET)
    {
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    }
    
    /* Config data register */
    InitData.DatTimeout        = SDMMC_DATATIMEOUT;
    InitData.DatLen            = NumberOfBlocks * SD_BLOCKSIZE;
    InitData.DatBlkSize        = SDIO_DATBLK_SIZE_512B;
    InitData.TransferDirection = SDIO_TRANSDIR_TOCARD;
    InitData.TransferMode      = SDIO_TRANSMODE_BLOCK;
    InitData.DPSMConfig        = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(&InitData);

    /* Send command for writting */
    if(NumberOfBlocks > 1U)
    {
        error = SDIO_CmdWriteMultiBlock(WriteAddr);
    }
    else
    {
        error = SDIO_CmdWriteSingleBlock(WriteAddr);
    }
    
    if(error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

#if defined(SD_DMA_MODE)

    SD_DMA_TxConfig((uint32_t*)SrcBuf, ((NumberOfBlocks * SD_BLOCKSIZE)/4));
    SDIO_DMACmd(ENABLE);

    timeout = Timeout;
    while(1)
    {
        if( (DMA_GetFlagStatus(SDIO_DMA_ENDFLAG,SDIO_DMA) == SET)
           &&(SDIO_GetFlag(SDIO_FLAG_DATEND) == SET)    )
        {
            break;
        }

        if(0U == timeout )
        {
            break;
        }
        else
        {
            timeout--;
        }
    }
    
    SDIO_DMACmd(DISABLE);

    if(0U == timeout )
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_TIMEOUT);
    }

#elif defined(SD_POLLING_MODE)
    timeout = Timeout;
    pSrc = (uint32_t *)SrcBuf;
    while(SDIO_GetFlag(SDIO_FLAG_TXURERR    \
                    |  SDIO_FLAG_DCRCERR    \
                    |  SDIO_FLAG_DATTIMEOUT \
                    |  SDIO_FLAG_DATEND     \
                    |  SDIO_FLAG_SBERR) == RESET)
    {
        if(SDIO_GetFlag(SDIO_FLAG_TFIFOHE) != RESET)
        {
            for(count = 0; count < 8; count++)
            {
                SDIO_WriteData(pSrc[count]);
            }

            pSrc += 8;
        }

        if(0U == timeout )
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }
#endif

    /* Send stop transmission command in case of multiblock read */
    if((SDIO_GetFlag(SDIO_FLAG_DATEND)!= RESET) && (NumberOfBlocks > 1U))
    {
        if(SDCardInfo.CardType != CARD_SDIO)
        {
            error = SDIO_CmdStopTransfer();
            if(error != SDMMC_ERROR_NONE)
            {
                /* Clear all the static flags */
                SDIO_ClrFlag(SDIO_STATIC_FLAGS);
                return (error);
            }
        }
    }

    if(SDIO_GetFlag(SDIO_FLAG_DATTIMEOUT) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_DATA_TIMEOUT);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_DCRCERR) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_DATA_CRC_FAIL);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_TXURERR) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_TX_UNDERRUN);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_SBERR) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_START_BIT_ERR);
    }
    else
    {
        /* Do nothing */
    }

    /* Reset data control register */
    SDIO->DATCTRL = 0x0U;

    /* Clear all the static flags */
    SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    
    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_Erase
 *\*\fun    Erases the specified memory area of the given SD card.
 *\*\param  StrAddr Start Block address.
 *\*\param  EndAddr End Block address.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 *\*\note This function should be followed by a check on the card state through SD_GetCardState.
 */
uint32_t SD_Erase(uint32_t StrAddr, uint32_t EndAddr)
{
    uint32_t error;
    
    if(EndAddr < StrAddr)
    {
        return (SDMMC_ERROR_INVALID_PARAMETER);
    }

    if((EndAddr/512) > SDCardInfo.LogBlockNbr)
    {
        return (SDMMC_ERROR_ADDR_OUT_OF_RANGE);
    }

    if((SDCardInfo.Class & SDIO_CCCC_ERASE) == 0U)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_UNSUPPORTED_FEATURE);
    }

    if( (SDIO_GetResp(SDIO_RESPONSE_1) & SDMMC_R1_CARD_LOCKED) == SDMMC_R1_CARD_LOCKED)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_LOCK_UNLOCK_FAILED);
    }

    if(   (SDCardInfo.CardType == CARD_SDHC_SDXC)     \
        ||(SDCardInfo.CardType == CARD_MMC_HC)   ) // The address of the high capacity card is block(512bytes) number, not byte address
    {
        StrAddr /= 512;
        EndAddr /= 512;
    }

    if( (SDCardInfo.CardType == CARD_MMC_HC)    \
      ||(SDCardInfo.CardType == CARD_MMC_LC)    )
    {
        /* Send CMD35 MMC_ERASE_GRP_START with argument as addr  */
        error = SDIO_CmdEraseStartAdd(StrAddr);
    }
    else if(SDCardInfo.CardType != CARD_SDIO)
    {
         /* Send CMD32 SD_ERASE_GRP_START with argument as addr  */
        error = SDIO_CmdSDEraseStartAdd(StrAddr);
    }
    else
    {
        error = SDMMC_ERROR_NONE;
    }

    if(error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    if( (SDCardInfo.CardType == CARD_MMC_HC)    \
      ||(SDCardInfo.CardType == CARD_MMC_LC)    )
    {
        /* Send CMD36 MMC_ERASE_GRP_END with argument as addr  */
        error = SDIO_CmdEraseEndAdd(EndAddr);
    }
    else if(SDCardInfo.CardType != CARD_SDIO)
    {
        /* Send CMD33 SD_ERASE_GRP_END with argument as addr  */
        error = SDIO_CmdSDEraseEndAdd(EndAddr);
    }
    else
    {
        error = SDMMC_ERROR_NONE;
    }

    if(error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    /* Send CMD38 ERASE */
    error = SDIO_CmdErase();
    if(error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_SendCardStatus
 *\*\fun    Returns the current card's status.
 *\*\param  pCardStatus: pointer to the buffer that will contain the SD card
 *\*\      status (Card Status register).
 *\*\return SD card error code,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_SendCardStatus(uint32_t* pCardStatus)
{
    uint32_t error;

    /* Send CMD13 SEND_STATUS */
    error = SDIO_CmdSendStatus((uint32_t)SDCardInfo.RelCardAdd << 16U);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    *pCardStatus = SDIO_GetResp(SDIO_RESPONSE_1);
    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_SendSDStatus
 *\*\fun    Returns the current SD card's status.
 *\*\param  cardinfo pointer to the buffer that will contain the SD card status
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t SD_SendSDStatus(uint32_t* pSts)
{
    uint32_t error;
    uint32_t count;
    uint32_t timeout;
    SDIO_DataInitType InitData;

    if(SDMMC_R1_CARD_LOCKED == (SDIO_GetResp(SDIO_RESPONSE_1) & SDMMC_R1_CARD_LOCKED))
    {
        return (SDMMC_ERROR_LOCK_UNLOCK_FAILED);
    }

    /* Set block size for card if it is not equal to current block size for card. */
    error = SDIO_CmdBlockLength(64U);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    /* Send CMD55 */
    error = SDIO_CmdAppCommand((uint32_t)SDCardInfo.RelCardAdd << 16U);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    /* Configure the SD DPSM (Data Path State Machine) */
    InitData.DatTimeout        = SDMMC_DATATIMEOUT;
    InitData.DatLen            = 64U;
    InitData.DatBlkSize        = SDIO_DATBLK_SIZE_64B;
    InitData.TransferDirection = SDIO_TRANSDIR_TOSDIO;
    InitData.TransferMode      = SDIO_TRANSMODE_BLOCK;
    InitData.DPSMConfig        = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(&InitData);

    /* Send ACMD13 SD_APP_STAUS  with argument as card's RCA.*/
    error = SDIO_CmdSendStatus(0U);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    timeout = SDMMC_DATATIMEOUT;
    while(RESET== SDIO_GetFlag(SDIO_FLAG_RXORERR    \
                            |  SDIO_FLAG_DCRCERR    \
                            |  SDIO_FLAG_DATTIMEOUT \
                            |  SDIO_FLAG_DATBLKEND  \
                            |  SDIO_FLAG_SBERR) )
    {
        if (SDIO_GetFlag(SDIO_FLAG_RFIFOHF) != RESET)
        {
            for (count = 0U; count < 8U; count++)
            {
                *pSts = SDIO_ReadData();
                pSts++;
            }
        }

        if(0U == timeout )
        {
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }

    if (SDIO_GetFlag(SDIO_FLAG_DATTIMEOUT) != RESET)
    {
        SDIO_ClrFlag(SDIO_FLAG_DATTIMEOUT);
        return (SDMMC_ERROR_DATA_TIMEOUT);
    }
    else if (SDIO_GetFlag(SDIO_FLAG_DCRCERR) != RESET)
    {
        SDIO_ClrFlag(SDIO_FLAG_DCRCERR);
        return (SDMMC_ERROR_DATA_CRC_FAIL);
    }
    else if (SDIO_GetFlag(SDIO_FLAG_RXORERR) != RESET)
    {
        SDIO_ClrFlag(SDIO_FLAG_RXORERR);
        return (SDMMC_ERROR_RX_OVERRUN);
    }
    else if (SDIO_GetFlag(SDIO_FLAG_SBERR) != RESET)
    {
        SDIO_ClrFlag(SDIO_FLAG_SBERR);
        return (SDMMC_ERROR_START_BIT_ERR);
    }
    else
    {

    }

    timeout = SDMMC_DATATIMEOUT;
    while(SDIO_GetFlag(SDIO_FLAG_RDATVALID) != RESET)
    {
        *pSts = SDIO_ReadData();
        pSts++;

        if(0U == timeout )
        {
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }

    /* Reset data control register */
    SDIO->DATCTRL = 0x0U;

    /* Clear all the static status flags*/
    SDIO_ClrFlag(SDIO_STATIC_FLAGS);

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   MMC_GetExtCSD
 *\*\fun    Returns information the information of the card which are stored on
 *\*\       the Extended CSD register.
 *\*\param  pExtCSD Pointer to a memory area (512 bytes) that contains all 
 *\*\       Extended CSD register parameters
 *\*\param  Timeout Specify timeout value, can not be 0.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t MMC_GetExtCSD(uint32_t *pExtCSD, uint32_t Timeout)
{
    SDIO_DataInitType InitData;
    uint32_t error;
    uint32_t count;
    uint32_t timeout;

    while(SDIO_GetFlag(SDIO_STATIC_FLAGS) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    }
    
    /* Reset data control register */
    SDIO->DATCTRL = 0;

    /* Configure the MMC DPSM (Data Path State Machine) */
    InitData.DatTimeout        = SDMMC_DATATIMEOUT;
    InitData.DatLen            = 512;
    InitData.DatBlkSize        = SDIO_DATBLK_SIZE_512B;
    InitData.TransferDirection = SDIO_TRANSDIR_TOSDIO;
    InitData.TransferMode      = SDIO_TRANSMODE_BLOCK;
    InitData.DPSMConfig        = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(&InitData);

    /* Send CMD8 SEND_EXT_CSD */
    error = SDIO_CmdSendEXTCSD(0U);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    timeout = Timeout;
    /* Poll on SDMMC flags */
    while(RESET != SDIO_GetFlag(SDIO_FLAG_RXORERR   \
                            |   SDIO_FLAG_DCRCERR   \
                            |   SDIO_FLAG_DATTIMEOUT\
                            |   SDIO_FLAG_DATEND)   )
    {
        if(SDIO_GetFlag(SDIO_FLAG_RFIFOHF) == SET)
        {
            /* Read data from Rx FIFO */
            for(count = 0U; count < 8U; count++)
            {
                pExtCSD[count] = SDIO_ReadData();
            }
        }

        if(0U == timeout)
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }

    /* Get error state */
    if(SDIO_GetFlag(SDIO_FLAG_DATTIMEOUT))
    {
      /* Clear all the static flags */
      SDIO_ClrFlag(SDIO_STATIC_FLAGS);
      return (SDMMC_ERROR_TIMEOUT);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_DCRCERR))
    {
      /* Clear all the static flags */
      SDIO_ClrFlag(SDIO_STATIC_FLAGS);
      return (SDMMC_ERROR_DATA_CRC_FAIL);
    }
    else if(SDIO_GetFlag(SDIO_FLAG_RXORERR))
    {
      /* Clear all the static flags */
      SDIO_ClrFlag(SDIO_STATIC_FLAGS);
      return (SDMMC_ERROR_RX_OVERRUN);
    }
    else
    {
      /* Nothing to do */
    }

    SDIO_ClrFlag(SDIO_STATIC_DATA_FLAGS);
    return SDMMC_ERROR_NONE;
}

/**
 *\*\name   MMC_ReadExtCSD
 *\*\fun    Reads extended CSD register to get the sectors number of the device
 *\*\param  pData Pointer to the read buffer
 *\*\param  Index Index of the field to be read
 *\*\param  Timeout Specify timeout value, can not be 0.
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
uint32_t MMC_ReadExtCSD(uint32_t *pData, uint16_t Index, uint32_t Timeout)
{
    SDIO_DataInitType InitData;
    uint32_t error;
    uint32_t count;
    uint32_t temp;
    uint32_t timeout;
    uint32_t i;

    while(SDIO_GetFlag(SDIO_STATIC_FLAGS) != RESET)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
    }
    
    /* Reset data control register */
    SDIO->DATCTRL = 0;
    
    InitData.DatTimeout        = SDMMC_DATATIMEOUT;
    InitData.DatLen            = 512;
    InitData.DatBlkSize        = SDIO_DATBLK_SIZE_512B;
    InitData.TransferDirection = SDIO_TRANSDIR_TOSDIO;
    InitData.TransferMode      = SDIO_TRANSMODE_BLOCK;
    InitData.DPSMConfig        = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(&InitData);

    /* Send CMD8 SEND_EXT_CSD */
    error = SDIO_CmdSendEXTCSD(0U);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    i = 0;
    timeout = Timeout;
    /* Poll on SDMMC flags */
    while(RESET != SDIO_GetFlag(SDIO_FLAG_RXORERR   \
                            |   SDIO_FLAG_DCRCERR   \
                            |   SDIO_FLAG_DATTIMEOUT\
                            |   SDIO_FLAG_DATEND)   )
    {
        if(SDIO_GetFlag(SDIO_FLAG_RFIFOHF) == SET)
        {
            /* Read data from SDMMC Rx FIFO */
            for(count = 0U; count < 8U; count++)
            {
                temp = SDIO_ReadData();
                /* eg : SEC_COUNT   : Index = 212 => i+count = 53 */
                /*      DEVICE_TYPE : Index = 196 => i+count = 49 */
                if ((i + count) == ((uint32_t)Index/4U))
                {
                    *pData = temp;
                }
            }
            i += 8U;
        }

        if(0U == timeout)
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }

    /* Reset data control register */
    SDIO->DATCTRL = 0x0U;

    /* Clear all the static flags */
    SDIO_ClrFlag(SDIO_STATIC_DATA_FLAGS);
    
    return SDMMC_ERROR_NONE;
}

/**
 *\*\name   MMC_PwrClassUpdate
 *\*\fun    Update the power class of the device.
 *\*\param  WideMode Specifies the SD card wide bus mode.
 *\*\           - SDIO_BUS_WIDTH_1B 1-bit data transfer
 *\*\           - SDIO_BUS_WIDTH_4B 4-bit data transfer
 *\*\           - SDIO_BUS_WIDTH_8B 8-bit data transfer
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
static uint32_t MMC_PwrClassUpdate(uint32_t WideMode)
{
    uint32_t count;
    uint32_t error;
    uint32_t response;
    uint32_t power_class;
    uint32_t supported_pwr_class;

    if((WideMode == SDIO_BUS_WIDTH_8B) || (WideMode == SDIO_BUS_WIDTH_4B))
    {
        power_class = 0U; /* Default value after power-on or software reset */

        /* Read the PowerClass field of the Extended CSD register */
        error = MMC_ReadExtCSD(&power_class, 187, SDMMC_DATATIMEOUT); /* Field POWER_CLASS [187] */
        if(error != SDMMC_ERROR_NONE)
        {
            error = SDMMC_ERROR_GENERAL_UNKNOWN_ERROR;
        }
        else
        {
            power_class = ((power_class >> 24U) & 0x000000FFU);
        }

        /* Get the supported PowerClass field of the Extended CSD register */
        /* Field PWR_CL_26_xxx [201 or 203] */
        supported_pwr_class = ((Ext_CSD_Tab[(MMC_EXT_CSD_PWR_CL_26_INDEX/4)] >> MMC_EXT_CSD_PWR_CL_26_POS) & 0x000000FFU);

        if(error == SDMMC_ERROR_NONE)
        {
            if(WideMode == SDIO_BUS_WIDTH_8B)
            {
                /* Bit [7:4] : power class for 8-bits bus configuration - Bit [3:0] : power class for 4-bits bus configuration */
                supported_pwr_class = (supported_pwr_class >> 4U);
            }

            if ((power_class & 0x0FU) != (supported_pwr_class & 0x0FU))
            {
                /* Need to change current power class */
                error = SDIO_CmdSwitch((0x03BB0000U | ((supported_pwr_class & 0x0FU) << 8U)));
                if(error == SDMMC_ERROR_NONE)
                {
                    /* While card is not ready for data and trial number for sending CMD13 is not exceeded */
                    count = SDMMC_MAX_TRIAL;
                    while(1)
                    {
                        error = SDIO_CmdSendStatus((((uint32_t)SDCardInfo.RelCardAdd) << 16U));
                        if(error != SDMMC_ERROR_NONE)
                        {
                            break;
                        }

                        /* Get command response */
                        response = SDIO_GetResp(SDIO_RESPONSE_1);
                        if((response & 0x100U) != 0U)
                        {
                            break;
                        }

                        count--;
                        if(0 == count)
                        {
                            break;
                        }
                    }

                    /* Check the status after the switch command execution */
                    if ((count != 0U) && (error == SDMMC_ERROR_NONE))
                    {
                        /* Check the bit SWITCH_ERROR of the device status */
                        if ((response & 0x80U) != 0U)
                        {
                            error = SDMMC_ERROR_UNSUPPORTED_FEATURE;
                        }
                    }
                    else if (count == 0U)
                    {
                        error = SDMMC_ERROR_TIMEOUT;
                    }
                    else
                    {
                        /* Nothing to do */
                    }
                }
            }
        }
    }

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   MMC_SetWideBus
 *\*\fun    Configure the data width of the MMC card
 *\*\param  WideMode Specifies the SD card wide bus mode.
 *\*\           - SDIO_BUS_WIDTH_1B 1-bit data transfer
 *\*\           - SDIO_BUS_WIDTH_4B 4-bit data transfer
 *\*\           - SDIO_BUS_WIDTH_8B 8-bit data transfer
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
static uint32_t MMC_SetWideBus(uint32_t WideMode)
{
    uint32_t error;
    uint32_t arg;
    uint32_t resp1;
    uint32_t timeout;

    /* Update the power class of the device. */
    error = MMC_PwrClassUpdate(WideMode);
    if(error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    /* Check parameter */
    if (SDIO_BUS_WIDTH_8B == WideMode)
    {
        arg = 0x03B70200U;
    }
    else if(SDIO_BUS_WIDTH_4B == WideMode)
    {
        arg = 0x03B70100U;
    }
    else if(SDIO_BUS_WIDTH_1B == WideMode)
    {
        arg = 0x03B70000U;
    }
    else
    {
        return (SDMMC_ERROR_INVALID_PARAMETER);
    }

    /* short delay */
    Delay(SystemCoreClock/2000);    /* about 4ms */
    
    /* Send CMD6  with argument  for wide bus mode */
    error = SDIO_CmdSwitch(arg);
    if (error != SDMMC_ERROR_NONE)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (error);
    }

    timeout = SDMMC_MAX_TRIAL;
    /* Check status until ready */
    while(1)
    {
        error = SD_SendCardStatus(&resp1);
        if (error != SDMMC_ERROR_NONE)
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (error);
        }

        if((resp1 & SDMMC_R1_CARD_READY_FOR_DATA) == SDMMC_R1_CARD_READY_FOR_DATA)
        {
            break;
        }
        
        if(0U == timeout )
        {
            /* Clear all the static flags */
            SDIO_ClrFlag(SDIO_STATIC_FLAGS);
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }

    if((resp1 & SDMMC_R1_CARD_UNKNOWN_ERROR) == SDMMC_R1_CARD_UNKNOWN_ERROR)
    {
        /* Clear all the static flags */
        SDIO_ClrFlag(SDIO_STATIC_FLAGS);
        return (SDMMC_ERROR_GENERAL_UNKNOWN_ERROR);
    }
    
    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   SD_EnableWideBus
 *\*\fun    Enable or disable 4 bit mode of SDIO
 *\*\param  Cmd 
 *\*\           - ENABLE
 *\*\           - DISABLE
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
static uint32_t SD_EnableWideBus(FunctionalState Cmd)
{
    uint32_t error;
    uint32_t arg;
    uint32_t scr[2] = {0, 0};

    /* Check whether the card is locked */
    if(SDMMC_R1_CARD_LOCKED == (SDIO_GetResp(SDIO_RESPONSE_1) & SDMMC_R1_CARD_LOCKED))
    {
        return (SDMMC_ERROR_LOCK_UNLOCK_FAILED);
    }

    /* Get SCR Register */
    error = FindSCR(SDCardInfo.RelCardAdd,scr); // Get SCR register contents to SCR array
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    /* If requested card supports wide bus operation */
    if ((scr[1] & SDMMC_WIDE_BUS_SUPPORT) == SDMMC_ALLZERO) // whether the card supports 4-bit mode
    {
        return (SDMMC_ERROR_REQUEST_NOT_APPLICABLE);
    }

    /* Send CMD55 APP_CMD with argument as card's RCA.*/
    error = SDIO_CmdAppCommand((uint32_t)SDCardInfo.RelCardAdd << 16U);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    /* If wide bus operation to be enabled */
    if (Cmd == ENABLE)
    {
        arg = 2U;
    } /* If wide bus operation to be disabled */
    else
    {
        arg = 0U;
    }

    /* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
    /*acmd6: Command to turn on 4bit mode*/
    error = SDIO_CmdBusWidth(arg);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    return (SDMMC_ERROR_NONE);
}

/**
 *\*\name   FindSCR
 *\*\fun    Read the contents of SCR register of SD card
 *\*\param  rca Card relative address
 *\*\param  pScr Pointer to load SCR content
 *\*\return SD card error state,it is SDMMC_ERROR_NONE if successful
 */
static uint32_t FindSCR(uint16_t rca, uint32_t* pScr)
{
    uint32_t index;
    uint32_t error;
    uint32_t timeout;
    uint32_t tempscr[2]  = {0, 0};
    SDIO_DataInitType DataInit;

    /* Set Block Size To 8 Bytes */
    error = SDIO_CmdBlockLength(8U);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    /* Send CMD55 APP_CMD with argument as card's RCA.*/
    error = SDIO_CmdAppCommand((uint32_t)rca << 16U);
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }
    
    DataInit.DatTimeout        = SDMMC_DATATIMEOUT;
    DataInit.DatLen            = 8U;
    DataInit.DatBlkSize        = SDIO_DATBLK_SIZE_8B;
    DataInit.TransferDirection = SDIO_TRANSDIR_TOSDIO;
    DataInit.TransferMode      = SDIO_TRANSMODE_BLOCK;
    DataInit.DPSMConfig        = SDIO_DPSM_ENABLE;
    SDIO_ConfigData(&DataInit);

    /* Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
    error = SDIO_CmdSendSCR();
    if (error != SDMMC_ERROR_NONE)
    {
        return (error);
    }

    timeout = SDMMC_MAX_TRIAL;
    index = 0;
    while(SDIO_GetFlag(SDIO_FLAG_RXORERR|SDIO_FLAG_DCRCERR|SDIO_FLAG_DATTIMEOUT) == RESET)
    {
        if (SDIO_GetFlag(SDIO_FLAG_RDATVALID) != RESET)
        {
            *(tempscr + index) = SDIO_ReadData();
            index++;

            if (index > 1)
                break;
        }
        
        if(SDIO_GetFlag(SDIO_FLAG_RXRUN) == RESET)
        {
            break;
        }
        
        if(0U == timeout )
        {
            return (SDMMC_ERROR_TIMEOUT);
        }
        else
        {
            timeout--;
        }
    }

    if (SDIO_GetFlag(SDIO_FLAG_DATTIMEOUT) != RESET)
    {
        error = SDMMC_ERROR_DATA_TIMEOUT;
    }
    else if (SDIO_GetFlag(SDIO_FLAG_DCRCERR) != RESET)
    {
        error = SDMMC_ERROR_DATA_CRC_FAIL;
    }
    else if (SDIO_GetFlag(SDIO_FLAG_RXORERR) != RESET)
    {
        error = SDMMC_ERROR_RX_OVERRUN;
    }
    else if (SDIO_GetFlag(SDIO_FLAG_SBERR) != RESET)
    {
        error = SDMMC_ERROR_START_BIT_ERR;
    }
    else
    {
        *(pScr + 1) = ((tempscr[0] & SDMMC_0TO7BITS) << 24) | ((tempscr[0] & SDMMC_8TO15BITS) << 8)
                    | ((tempscr[0] & SDMMC_16TO23BITS) >> 8) | ((tempscr[0] & SDMMC_24TO31BITS) >> 24);

        *(pScr) = ((tempscr[1] & SDMMC_0TO7BITS) << 24) | ((tempscr[1] & SDMMC_8TO15BITS) << 8)
                | ((tempscr[1] & SDMMC_16TO23BITS) >> 8) | ((tempscr[1] & SDMMC_24TO31BITS) >> 24);
        
        error = SDMMC_ERROR_NONE;
    }
    
    /* Reset data control register */
    SDIO->DATCTRL = 0x0U;

    /* Clear all the static flags */
    SDIO_ClrFlag(SDIO_STATIC_FLAGS);

    return (error);
}


