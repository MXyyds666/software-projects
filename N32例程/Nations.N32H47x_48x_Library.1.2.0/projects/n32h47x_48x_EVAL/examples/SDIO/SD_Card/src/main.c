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
 **/

#include "main.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_sdio.h"
#include "sdcard.h"
#include "log.h"

extern SD_CardInfoType SDCardInfo;

uint8_t RxBuf[TEST_BUF_LEN];
uint8_t TxBuf[TEST_BUF_LEN];

/**
 *\*\name   main.
 *\*\fun    Main program.
 *\*\param  none
 *\*\return none
 */
int main(void)
{
    uint32_t error;
    uint32_t i;
    uint32_t rand = 0;
    uint32_t key_cnt = 0;
    uint32_t led_cnt = 0;
    
    log_init();
    LED_Init(LED_PORT,LED_PIN,LED_CLOCK);
    Key_Init(KEY_PORT,KEY_PIN,KEY_CLOCK);
    
#if defined(SD_DMA_MODE)
    log_debug("SD/MMC Card demo start,read and write in DMA mode!\r\n");
#elif defined(SD_POLLING_MODE)
    log_debug("SD/MMC Card demo start,read and write in polling mode!\r\n");
#endif

    while(1)
    {
        if(led_cnt < LED_DELAY_TIMES)
        {
            led_cnt++;
        }
        else
        {
            led_cnt = 0;
            LED_Blink(LED_PORT,LED_PIN);
        }
        /* Key press check */
        if(KEY_PORT->PID & KEY_PIN)
        {
            key_cnt++;
        }
        else
        {
            key_cnt = 0;
        }
        
        /* Wait key press stable */
        if(key_cnt < KEY_CHECK_TIMES)
        {
            continue;
        }
        else
        {
            key_cnt = 0;
        }
        
        /* Wait key release */
        while(KEY_PORT->PID & KEY_PIN);
        log_debug("Key pressed!\r\n");
        rand++;
        
        /* Initialize the SDIO interface and SD card */
        error = SD_Init(SDIO_CLK_BYPASS_DISABLE, SD_TRANSFER_CLK_DIV, SDIO_BUS_WIDTH_4B);
        if(error != SDMMC_ERROR_NONE)
        {
            log_debug("SD/MMC Card initialization failed,error=%d!\r\n",error);
            while(1);
        }
        else
        {
            log_debug("SD/MMC Card initialization success!\r\n");
        }
        
        /* Show card infomation */
        SD_ShowCardInfo();
        
        /* Check card state */
        while(SD_GetCardState() != SDMMC_CARD_TRANSFER);
        
        /* First step: Read card */
        Memset(RxBuf,0x55U,TEST_BUF_LEN);
        error = SD_ReadBlocks(RxBuf,TEST_CARD_ADDR,TEST_BLOCK_NUM,SDMMC_DATATIMEOUT);
        if(error != SDMMC_ERROR_NONE)
        {
            log_debug("SD/MMC Card first read failed,error=%d!\r\n",error);
            while(1);
        }
        
        /* Check card state */
        while(SD_GetCardState() != SDMMC_CARD_TRANSFER);
        
        /* Show current card data */
        log_debug("\r\nCard data at 0x%08X read:\r\n",TEST_CARD_ADDR);
        DataShow(RxBuf, TEST_BUF_LEN);
        
        /* Second step: Erase card */
        error = SD_Erase(TEST_CARD_ADDR,TEST_CARD_ADDR+TEST_BUF_LEN);
        if(error != SDMMC_ERROR_NONE)
        {
            log_debug("SD/MMC Card erase failed,error=%d!\r\n",error);
            while(1);
        }
        else
        {
            log_debug("SD/MMC Card erase success!\r\n");
        }

        /* Check card state */
        while(SD_GetCardState() != SDMMC_CARD_TRANSFER);
        
        /* Third step: Write card */
        Fill_Buffer(TxBuf, TEST_BUF_LEN,(uint8_t)rand);
        error = SD_WriteBlocks(TxBuf,TEST_CARD_ADDR,TEST_BLOCK_NUM,SDMMC_DATATIMEOUT);
        if(error != SDMMC_ERROR_NONE)
        {
            log_debug("SD/MMC Card write failed,error=%d!\r\n",error);
            while(1);
        }
            
        /* Check card state */
        while(SD_GetCardState() != SDMMC_CARD_TRANSFER);
        
        /* Forth step: Check write result by read and compare */
        Memset(RxBuf,0x55U,TEST_BUF_LEN);
        error = SD_ReadBlocks(RxBuf,TEST_CARD_ADDR,TEST_BLOCK_NUM,SDMMC_DATATIMEOUT);
        if(error != SDMMC_ERROR_NONE)
        {
            log_debug("SD/MMC Card read after write failed,error=%d!\r\n",error);
            while(1);
        }
        
        /* Check card state */
        while(SD_GetCardState() != SDMMC_CARD_TRANSFER);
        
        /* Check write result */
        for(i=0;i<TEST_BUF_LEN;i++)
        {
            if(RxBuf[i] != TxBuf[i])
                break;
        }
        
        if(i<TEST_BUF_LEN)
        {
            log_debug("SD/MMC Card write check fail,i=%d,data=0x%02x!\r\n",i,RxBuf[i]);
            while(1);
        }
        else
        {
            log_debug("SD/MMC Card write success!\r\n");
        }
        
        /* Test finish */
        log_debug("SD/MMC Card test OK!\r\n");
    }
}

/**
 *\*\name   LED_Init.
 *\*\fun    Initialize a GPIO as LED port.
 *\*\param  GPIOx
 *\*\         - GPIOA
 *\*\         - GPIOB
 *\*\         - GPIOC
 *\*\         - GPIOD
 *\*\         - GPIOE
 *\*\         - GPIOF
 *\*\         - GPIOG
 *\*\         - GPIOH
 *\*\param  Pin
 *\*\         - GPIO_PIN_0
 *\*\         - GPIO_PIN_1
 *\*\         - GPIO_PIN_2
 *\*\         - GPIO_PIN_3
 *\*\         - GPIO_PIN_4
 *\*\         - GPIO_PIN_5
 *\*\         - GPIO_PIN_6
 *\*\         - GPIO_PIN_7
 *\*\         - GPIO_PIN_8
 *\*\         - GPIO_PIN_9
 *\*\         - GPIO_PIN_10
 *\*\         - GPIO_PIN_11
 *\*\         - GPIO_PIN_12
 *\*\         - GPIO_PIN_13
 *\*\         - GPIO_PIN_14
 *\*\         - GPIO_PIN_15
 *\*\         - GPIO_PIN_ALL
 *\*\param  clock
 *\*\         - RCC_AHB_PERIPHEN_GPIOA
 *\*\         - RCC_AHB_PERIPHEN_GPIOB
 *\*\         - RCC_AHB_PERIPHEN_GPIOC
 *\*\         - RCC_AHB_PERIPHEN_GPIOD
 *\*\         - RCC_AHB_PERIPHEN_GPIOE
 *\*\         - RCC_AHB_PERIPHEN_GPIOF
 *\*\         - RCC_AHB_PERIPHEN_GPIOG
 *\*\         - RCC_AHB_PERIPHEN_GPIOH
 *\*\return none
 */
void LED_Init(GPIO_Module* GPIOx,uint16_t Pin, uint32_t clock)
{
    GPIO_InitType InitStruct;
    
    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(clock,ENABLE);
    
    /* Init GPIO as output push-pull mode */
    InitStruct.Pin            = Pin;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
    InitStruct.GPIO_Alternate = GPIO_AF0;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    
    GPIO_InitPeripheral(GPIOx, &InitStruct);
}

/**
 *\*\name   LED_On.
 *\*\fun    Turn on LED by set GPIO pin.
 *\*\param  none
 *\*\return none
 */
void LED_On(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_SetBits(GPIOx,Pin);
}

/**
 *\*\name   LED_Off.
 *\*\fun    Turn off LED by reset GPIO pin.
 *\*\param  none
 *\*\return none
 */
void LED_Off(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_ResetBits(GPIOx,Pin);
}

/**
 *\*\name   LED_Blink.
 *\*\fun    Blink LED by toggle GPIO pin.
 *\*\param  none
 *\*\return none
 */
void LED_Blink(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_TogglePin(GPIOx,Pin);
}

/**
 *\*\name   Key_Init.
 *\*\fun    Initialize a GPIO as key port.
 *\*\param  GPIOx
 *\*\         - GPIOA
 *\*\         - GPIOB
 *\*\         - GPIOC
 *\*\         - GPIOD
 *\*\         - GPIOE
 *\*\         - GPIOF
 *\*\         - GPIOG
 *\*\         - GPIOH
 *\*\param  Pin
 *\*\         - GPIO_PIN_0
 *\*\         - GPIO_PIN_1
 *\*\         - GPIO_PIN_2
 *\*\         - GPIO_PIN_3
 *\*\         - GPIO_PIN_4
 *\*\         - GPIO_PIN_5
 *\*\         - GPIO_PIN_6
 *\*\         - GPIO_PIN_7
 *\*\         - GPIO_PIN_8
 *\*\         - GPIO_PIN_9
 *\*\         - GPIO_PIN_10
 *\*\         - GPIO_PIN_11
 *\*\         - GPIO_PIN_12
 *\*\         - GPIO_PIN_13
 *\*\         - GPIO_PIN_14
 *\*\         - GPIO_PIN_15
 *\*\         - GPIO_PIN_ALL
 *\*\param  clock
 *\*\         - RCC_AHB_PERIPHEN_GPIOA
 *\*\         - RCC_AHB_PERIPHEN_GPIOB
 *\*\         - RCC_AHB_PERIPHEN_GPIOC
 *\*\         - RCC_AHB_PERIPHEN_GPIOD
 *\*\         - RCC_AHB_PERIPHEN_GPIOE
 *\*\         - RCC_AHB_PERIPHEN_GPIOF
 *\*\         - RCC_AHB_PERIPHEN_GPIOG
 *\*\         - RCC_AHB_PERIPHEN_GPIOH
 *\*\return none
 */
void Key_Init(GPIO_Module* GPIOx,uint16_t Pin, uint32_t clock)
{
    GPIO_InitType InitStruct;
    
    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(clock,ENABLE);
    
    /* Init GPIO as input push-down mode */
    InitStruct.Pin            = Pin;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_AF0;
    InitStruct.GPIO_Pull      = GPIO_PULL_DOWN;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    
    GPIO_InitPeripheral(GPIOx, &InitStruct);
}

/**
 *\*\name   Delay.
 *\*\fun    Delay a short time.
 *\*\param  none
 *\*\return none
 */
void Delay(uint32_t delay)
{
    uint32_t i;
    
    for(i=0;i<delay;i++);
}

void SD_ShowCardInfo(void)
{
    uint8_t str[6]={0,};
    
    log_debug("\t CardType:    %d\r\n", SDCardInfo.CardType);
    log_debug("\t CardVersion: %d\r\n", SDCardInfo.CardVersion);
    log_debug("\t Class:       0x%X\r\n", SDCardInfo.Class);
    log_debug("\t RelCardAdd:  0x%X\r\n", SDCardInfo.RelCardAdd);
    log_debug("\t BlockNbr:    %d\r\n", SDCardInfo.BlockNbr);
    log_debug("\t BlockSize:   %d\r\n", SDCardInfo.BlockSize);
    log_debug("\t LogBlockNbr: %d\r\n", SDCardInfo.LogBlockNbr);
    log_debug("\t LogBlockSize:%d\r\n", SDCardInfo.LogBlockSize);
    log_debug("\r\n");
    log_debug("\t ManufacturerID:0x%X\r\n", SDCardInfo.CID.ManufacturerID);
    str[0] = (uint8_t)((SDCardInfo.CID.OEM_AppliID & 0xFF00) >> 8U);
    str[1] = (uint8_t)((SDCardInfo.CID.OEM_AppliID & 0x00FF) >> 0U);
    log_debug("\t OEM_AppliID:   %c%c\r\n", str[0],str[1]);
    str[0] = (uint8_t)((SDCardInfo.CID.ProdName1 & 0xFF000000) >> 24U);
    str[1] = (uint8_t)((SDCardInfo.CID.ProdName1 & 0x00FF0000) >> 16U);
    str[2] = (uint8_t)((SDCardInfo.CID.ProdName1 & 0x0000FF00) >>  8U);
    str[3] = (uint8_t)((SDCardInfo.CID.ProdName1 & 0x000000FF) >>  0U);
    str[4] = (uint8_t)(SDCardInfo.CID.ProdName2);
    log_debug("\t ProdName:      %c%c%c%c%c\r\n", str[0],str[1],str[2],str[3],str[4]);
    str[0] = (uint8_t)((SDCardInfo.CID.ProdRev & 0xF0) >> 4U);
    str[1] = (uint8_t)((SDCardInfo.CID.ProdRev & 0x0F) >> 0U);
    log_debug("\t ProdRev:       %d.%d\r\n", str[0],str[1]);
    log_debug("\t ProdSN:        0x%X\r\n", SDCardInfo.CID.ProdSN);
    str[0] = (uint8_t)((SDCardInfo.CID.ManufactDate & 0xF00) >> 8U);
    str[1] = (uint8_t)((SDCardInfo.CID.ManufactDate & 0x0F0) >> 4U);
    str[2] = (uint8_t)((SDCardInfo.CID.ManufactDate & 0x00F) >> 0U);
    log_debug("\t ManufactDate:  %d%d.%d\r\n", str[0],str[1],str[2]);
    log_debug("\r\n");
    log_debug("\t CSDStruct:         0x%X\r\n", SDCardInfo.CSD.CSDStruct);
    log_debug("\t MaxBusClkFrec:     0x%X\r\n", SDCardInfo.CSD.MaxBusClkFrec);
    log_debug("\t CardComdClasses:   0x%X\r\n", SDCardInfo.CSD.CardComdClasses);
    log_debug("\t RdBlockLen:        %d\r\n", SDCardInfo.CSD.RdBlockLen);
    log_debug("\t WrSpeedFact:       %d\r\n", SDCardInfo.CSD.WrSpeedFact);
    log_debug("\t MaxWrBlockLen:     %d\r\n", SDCardInfo.CSD.MaxWrBlockLen);
    log_debug("\t FileFormatGroup:   %d\r\n", SDCardInfo.CSD.FileFormatGroup);
    log_debug("\t FileFormat:        %d\r\n", SDCardInfo.CSD.FileFormat);
}

/**
 *\*\name   Memset
 *\*\fun    Set a memory field with certain value.
 *\*\param  pBuf Pointer to the memory field.
 *\*\param  data The data value to be write into memory.
 *\*\param  count Memory size count in byte.
 *\*\return none
 */
void Memset(void* pBuf, uint8_t data, uint32_t count)
{
    char* p = (char*)pBuf;

    while (count--)
    {
        *p++ = data;
    }

    return;
}

/**
 *\*\name   Fill_Buffer
 *\*\fun    Fills buffer with user predefined data.
 *\*\param  pBuffer pointer on the Buffer to fill.
 *\*\param  BufferLength size of the buffer to fill.
 *\*\param  Offset first value to fill on the Buffer.
 *\*\return none
 */
void Fill_Buffer(uint8_t* pBuffer, uint32_t BufferLength, uint8_t Offset)
{
    uint32_t index = 0;

    /* Put in global buffer same values */
    for (index = 0; index < BufferLength; index++)
    {
        pBuffer[index] = index + Offset;
    }
}

/**
 *\*\name   DataShow
 *\*\fun    Show the buf data byte by byte.
 *\*\param  pBuf pointer on the Buffer.
 *\*\param  DataSize size of the buffer, count in byte.
 *\*\return none
 */
void DataShow(uint8_t* pBuf, uint32_t DataSize)
{
    uint32_t i;
    
    for(i = 0; i < DataSize; i++)
    {
        log_debug("  0x%02X", *(pBuf + i));

        if(((i+1)%8) == 0)
        {
            log_debug("\r\n\t");
        }
    }
    log_debug("\r\n\r\n");
}

