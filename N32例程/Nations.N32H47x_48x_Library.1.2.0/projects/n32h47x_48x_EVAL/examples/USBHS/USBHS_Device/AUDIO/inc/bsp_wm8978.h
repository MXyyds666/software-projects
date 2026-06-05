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
*\*\file      bsp_wm8978.h
*\*\author    Nations
*\*\version   v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved. 
**/

#ifndef __BSP_WM8978_H__
#define __BSP_WM8978_H__

#include "n32h47x_48x.h"

typedef enum
{
    IN_PATH_OFF     = 0x00, /* No input */
    MIC_LEFT_ON     = 0x01, /* LIN,LIP PIN, MIC左声道（接板载咪头）  */
    MIC_RIGHT_ON    = 0x02, /* RIN,RIP脚，MIC右声道（接板载咪头）  */
    LINE_ON         = 0x04, /* L2,R2 立体声输入(接板载耳机插座) */
    AUX_ON          = 0x08, /* AUXL,AUXR 立体声输入（开发板没用到） */
    DAC_ON          = 0x10, /* I2S数据DAC (CPU产生音频信号) */
    ADC_ON          = 0x20, /* 输入的音频馈入WM8978内部ADC （I2S录音) */
}IN_PATH_E;


typedef enum
{
    OUT_PATH_OFF    = 0x00, /* No output */
    EAR_LEFT_ON     = 0x01, /* LOUT1 耳机左声道 */
    EAR_RIGHT_ON    = 0x02, /* ROUT1 耳机右声道 */
    SPK_ON          = 0x04, /* LOUT2和ROUT2反相输出单声道（开发板没用到）*/
    OUT3_4_ON       = 0x08, /* OUT3 和 OUT4 输出单声道音频（开发板没用到）*/
}OUT_PATH_E;

#define VOLUME_MAX      63
#define VOLUME_STEP     1

#define GAIN_MAX        63
#define GAIN_STEP       1

#define WM8978_I2C_Speed                    400000
#define WM8978_SLAVE_ADDRESS                0x34

/*I2C接口*/
#define WM8978_I2C                          I2C1
#define WM8978_I2C_CLK                      RCC_APB1_PERIPH_I2C1

#define WM8978_I2C_SCL_PIN                  GPIO_PIN_6
#define WM8978_I2C_SCL_GPIO_PORT            GPIOB
#define WM8978_I2C_SCL_GPIO_CLK             RCC_AHB_PERIPHEN_GPIOB
#define WM8978_I2C_SCL_SOURCE               GPIO_PIN_SOURCE6
#define WM8978_I2C_SCL_AF                   GPIO_AF2

#define WM8978_I2C_SDA_PIN                  GPIO_PIN_7
#define WM8978_I2C_SDA_GPIO_PORT            GPIOB
#define WM8978_I2C_SDA_GPIO_CLK             RCC_AHB_PERIPHEN_GPIOB
#define WM8978_I2C_SDA_SOURCE               GPIO_PIN_SOURCE7
#define WM8978_I2C_SDA_AF                   GPIO_AF2

/* I2C timeout */
#define WM8978_I2C_FLAG_TIMEOUT             ((uint32_t)0x4000)
#define WM8978_I2C_LONG_TIMEOUT             ((uint32_t)(10 * WM8978_I2C_FLAG_TIMEOUT))

uint8_t WM8978_Init(void);
uint8_t WM8978_Reset(void);
uint8_t WM8978_Speaker_Init(uint32_t volume, uint32_t audio_freq);
void WM8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen);
void WM8978_OutMute(uint8_t _ucMute);
void WM8978_PowerDown(void);
void WM8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);
void WM8978_SetMicGain(uint8_t _ucGain);
void WM8978_SetLineGain(uint8_t _ucGain);
void WM8978_SetOUT2Volume(uint8_t _ucVolume);
void WM8978_SetOUT1Volume(uint8_t _ucVolume);
uint8_t WM8978_ReadOUT1Volume(void);
uint8_t WM8978_ReadOUT2Volume(void);
void WM8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);

/**
    * WM8978_LRC            PC6  /I2S2_WS
    * WM8978_BCLK           PC7  /I2S2_CK
    * WM8978_ADCDAT         PC2  /I2S2ext_SD
    * WM8978_DACDAT         PC3  /I2S2_SD
    * WM8978_MCLK           PA8  /I2S2_MCK
**/
#define AUDIO_I2S                       SPI2
#define AUDIO_I2S_EXT                   I2S2_EXT
#define AUDIO_I2S_CLK                   RCC_APB1_PERIPH_SPI2

#define AUDIO_I2S_CK_PIN                GPIO_PIN_7 /* PC.07 */
#define AUDIO_I2S_CK_GPIO_PORT          GPIOC
#define AUDIO_I2S_CK_GPIO_CLK           RCC_AHB_PERIPHEN_GPIOC
#define AUDIO_I2S_CK_GPIO_AF            GPIO_AF13

#define AUDIO_I2S_WS_PIN                GPIO_PIN_6 /* PC.06 */
#define AUDIO_I2S_WS_GPIO_PORT          GPIOC
#define AUDIO_I2S_WS_GPIO_CLK           RCC_AHB_PERIPHEN_GPIOC
#define AUDIO_I2S_WS_GPIO_AF            GPIO_AF13

#define AUDIO_I2S_SD_PIN                GPIO_PIN_3 /* PC.03 */
#define AUDIO_I2S_SD_GPIO_PORT          GPIOC
#define AUDIO_I2S_SD_GPIO_CLK           RCC_AHB_PERIPHEN_GPIOC
#define AUDIO_I2S_SD_GPIO_AF            GPIO_AF1

#define AUDIO_I2S_AUX_SD_PIN            GPIO_PIN_2 /* PC.02 */
#define AUDIO_I2S_AUX_SD_GPIO_PORT      GPIOC
#define AUDIO_I2S_AUX_SD_GPIO_CLK       RCC_AHB_PERIPHEN_GPIOC
#define AUDIO_I2S_AUX_SD_GPIO_AF        GPIO_AF2

#define AUDIO_I2S_MCK_PIN               GPIO_PIN_8 /* PA.08 */
#define AUDIO_I2S_MCK_GPIO_PORT         GPIOA
#define AUDIO_I2S_MCK_GPIO_CLK          RCC_AHB_PERIPHEN_GPIOA
#define AUDIO_I2S_MCK_GPIO_AF           GPIO_AF1

#define I2S_MASTER                      SPI2
#define I2S_EXT                         I2S2_EXT

#define I2S_MASTER_DMA                  DMA1
#define I2S_MASTER_DMA_CLK              RCC_AHB_PERIPHEN_DMA1
#define I2S_MASTER_Tx_DMA_Channel       DMA1_CH2
#define I2S_MASTER_Tx_DMA_FLAG          DMA_FLAG_TC2
#define I2S_MASTER_Tx_DMA_Remap         DMA_REMAP_SPI2_I2S2_TX
#define I2S_MASTER_Tx_DMA_IRQN          DMA1_Channel2_IRQn
#define I2S_MASTER_DMA_IRQ_HANDLER      DMA1_Channel2_IRQHandler
#define I2S_MASTER_DMA_IT_FLAG_TC       DMA_INT_TXC2
#define I2S_MASTER_DMA_IT_FLAG_HT       DMA_INT_HTX2

#define I2S_EXT_DMA                     DMA1
#define I2S_EXT_DMA_CLK                 RCC_AHB_PERIPHEN_DMA1
#define I2S_EXT_Rx_DMA_Channel          DMA1_CH3
#define I2S_EXT_Rx_DMA_FLAG             DMA_FLAG_TC3
#define I2S_EXT_Rx_DMA_Remap            DMA_REMAP_SPI2_I2S2_RX
#define I2S_EXT_Rx_DMA_IRQN             DMA1_Channel3_IRQn
#define I2S_EXTDMA_IRQ_HANDLER          DMA1_Channel3_IRQHandler
#define I2S_EXTDMA_IT_FLAG_TC           DMA_INT_TXC3
#define I2S_EXTDMA_IT_FLAG_HT           DMA_INT_HTX3

#define I2S_RX_DMA_BUF_SIZE             2048
#define I2S_TX_DMA_BUF_SIZE             1

extern void (*I2S_DMA_TX_Callback)(void);
extern void (*I2S_DMA_RX_Callback)(void);

void I2S_GPIO_Config(void);
void I2S_Stop(void);
void I2S_Mode_Config(uint16_t Standard, uint16_t WordLen, uint32_t AudioFreq);
void I2S_TX_DMA_Init(uint8_t* buf, uint32_t size);
void I2S_TX_DMA_Add_SIZE_Set(uint8_t* buf, uint32_t size);
void I2S_Play_Start(void);
void I2S_Play_Stop(void);

void I2Sext_Mode_Config(const uint16_t _usStandard, const uint16_t _usWordLen,const uint32_t _usAudioFreq);
void I2Sext_RX_DMA_Init(uint8_t* buf, uint32_t size);
void I2Sext_Recorde_Start(void);
void I2Sext_Recorde_Stop(void);
#endif /* __BSP_WM8978_H__ */
