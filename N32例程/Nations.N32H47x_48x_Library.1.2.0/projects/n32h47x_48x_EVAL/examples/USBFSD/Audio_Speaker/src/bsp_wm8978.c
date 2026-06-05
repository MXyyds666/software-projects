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
*\*\file      bsp_WM8978.c
*\*\author    Nations
*\*\version   v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved. 
**/
#include "bsp_WM8978.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_spi.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_i2c.h"
#include "misc.h"
#include "log.h"

extern uint32_t shrtpll_clock;

static void I2C_GPIO_Config(void);
static void I2C_Mode_Config(void);
static  uint8_t WM8978_I2C_TIMEOUT_UserCallback(void);
//static uint8_t WM8978_I2C_WriteRegister(uint8_t RegisterAddr, uint16_t RegisterValue);
static uint16_t WM8978_ReadReg(uint8_t RegAddr);
static uint8_t WM8978_WriteReg(uint8_t RegAddr, uint16_t Value);

static __IO uint32_t  WM8978_I2CTimeout = WM8978_I2C_LONG_TIMEOUT;

/*  I2S DMA call back pointer */
void (*I2S_DMA_TX_Callback)(void);
void (*I2S_DMA_RX_Callback)(void);

/**  Cache area for WM8978 register values (total 58 registers, 0~57), occupying 116 bytes of memory
*\*\ Since the I2C operation of WM8978 does not support read operations, all register values are saved locally
*\*\ When writing to the WM8978 register, synchronize and update the local register value. When reading a register, directly return the locally saved register value.
*\*\ Note: The register value of WM8978 is 9 bits, so uint16_t should be used for storage.
**/
static uint16_t WM8978_RegCash[] = 
{
    0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
    0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
    0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
    0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
    0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
    0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
    0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
    0x001, 0x001
};

/**
*\*\name    I2C_GPIO_Config.
*\*\fun     Configuration I2C gpio.
*\*\param   none
*\*\return  none
**/
static void I2C_GPIO_Config(void)
{
    GPIO_InitType  GPIO_InitStructure;    
    /* WM8978_I2C Periph clock enable */
    RCC_EnableAPB1PeriphClk(WM8978_I2C_CLK, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    /* WM8978_I2C_SCL_GPIO_CLK and WM8978_I2C_SDA_GPIO_CLK Periph clock enable */
    RCC_EnableAHB1PeriphClk(WM8978_I2C_SCL_GPIO_CLK | WM8978_I2C_SDA_GPIO_CLK, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&GPIO_InitStructure);
    /* GPIO configuration */
    /* Configure WM8978_I2C pins: SCL */   
    GPIO_InitStructure.Pin        = WM8978_I2C_SCL_PIN ;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = WM8978_I2C_SCL_AF;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_OD;
    GPIO_InitPeripheral(WM8978_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    /* Configure WM8978_I2C pins: SDA */
    GPIO_InitStructure.Pin = WM8978_I2C_SDA_PIN;
    GPIO_InitPeripheral(WM8978_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}
/**
*\*\name    I2C_Mode_Config.
*\*\fun     Configuration I2C mode.
*\*\param   none
*\*\return  none
**/
static void I2C_Mode_Config(void)
{
    I2C_InitType  I2C_InitStructure; 

    /* I2C  */
    I2C_InitStructure.BusMode       = I2C_BUSMODE_I2C;
    I2C_InitStructure.FmDutyCycle   = I2C_FMDUTYCYCLE_2;    
    I2C_InitStructure.OwnAddr1      = 0x0A; 
    I2C_InitStructure.AckEnable     = I2C_ACKEN;
    I2C_InitStructure.AddrMode      = I2C_ADDR_MODE_7BIT;
    I2C_InitStructure.ClkSpeed      = WM8978_I2C_Speed;
    I2C_Init(WM8978_I2C, &I2C_InitStructure);
    I2C_Enable(WM8978_I2C, ENABLE);
}

/**
*\*\name    WM8978_I2C_TIMEOUT_UserCallback.
*\*\fun     Basic management of the I2C timeout situation.
*\*\param   none
*\*\return  none
**/
static  uint8_t WM8978_I2C_TIMEOUT_UserCallback(void)
{
    /* Block communication and all processes */
    log_info("I2C Timeout error!");
    return 0;
}

/**
*\*\name    WM8978_I2C_WriteRegister.
*\*\fun     Writes a Byte to a given register into the audio codec through the control interface (I2C).
*\*\param   RegisterAddr: The address (location) of the register to be written.
*\*\param   RegisterValue: the Byte value to be written into destination register.
*\*\return  1 if correct communication and 0 if wrong communication
**/
static uint8_t WM8978_I2C_WriteRegister(uint8_t RegisterAddr, uint16_t RegisterValue)
{
    /* Start the config sequence */
    I2C_GenerateStart(WM8978_I2C, ENABLE);

    /* Test on EV5 and clear it */
    WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(WM8978_I2C, I2C_EVT_MASTER_MODE_FLAG))
    {
        if((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();
    }

    /* Transmit the slave address and enable writing operation */
    I2C_SendAddr7bit(WM8978_I2C, WM8978_SLAVE_ADDRESS, I2C_DIRECTION_SEND);

    /* Test on EV6 and clear it */
    WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(WM8978_I2C, I2C_EVT_MASTER_TXMODE_FLAG))
    {
        if((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();
    }

    /* Transmit the first address for write operation */
    I2C_SendData(I2C1, ((RegisterAddr << 1) & 0xFE) | ((RegisterValue >> 8) & 0x1));
    WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDING)) // EV8
    {
        if ((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();

    }
    
    /* Prepare the register value to be sent */
    I2C_SendData(I2C1, RegisterValue&0xff);
    WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDING)) // EV8
    {
        if ((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();
    }

    WM8978_I2CTimeout = WM8978_I2C_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2C1, I2C_EVT_MASTER_DATA_SENDED)) // EV8-2
    {
        if ((WM8978_I2CTimeout--) == 0) return WM8978_I2C_TIMEOUT_UserCallback();
    }
    
    /* End the configuration sequence */
    I2C_GenerateStop(WM8978_I2C, ENABLE);

    /*!< Wait till all data have been physically transferred on the bus */
    WM8978_I2CTimeout = WM8978_I2C_LONG_TIMEOUT;
    while(!I2C_GetFlag(WM8978_I2C, I2C_FLAG_BUSY))
    {
    if((WM8978_I2CTimeout--) == 0) WM8978_I2C_TIMEOUT_UserCallback();
    }

    /* Return the verifying value: 0 (Passed) or 1 (Failed) */
    return 1;  
}

/**
*\*\name    WM8978_ReadReg.
*\*\fun     Read a WM8978 register.
*\*\param   RegAddr: The address (location) of the WM8978 register.
*\*\return  regitster value
**/
static uint16_t WM8978_ReadReg(uint8_t RegAddr)
{
    return WM8978_RegCash[RegAddr];
}

/**
*\*\name    WM8978_WriteReg.
*\*\fun     Write WM8978 register.
*\*\param   RegAddr: The address (location) of the WM8978 register.
*\*\param   Value: The value write to register .
*\*\return  1 if correct communication and 0 if wrong communication
**/
static uint8_t WM8978_WriteReg(uint8_t RegAddr, uint16_t Value)
{
    uint8_t res;
    res = WM8978_I2C_WriteRegister(RegAddr,Value);
    WM8978_RegCash[RegAddr] = Value;
    return res;
}

/**
*\*\name    WM8978_Init.
*\*\fun     Initiliaze WM8978.
*\*\param   none.
*\*\return  1: success, 0 failed
**/
uint8_t WM8978_Init(void)
{
    uint8_t res;
    I2C_GPIO_Config();
    I2C_Mode_Config();
    res = WM8978_Reset();
    return res;
}

/**
*\*\name    WM8978_Speaker_Init.
*\*\fun     Configures WM8978 speaker.
*\*\param   volume: volume 0~100
*\*\param   audio_freq: audio frequecy
*\*\return  0: success, 1: failed
**/
uint8_t WM8978_Speaker_Init(uint32_t volume, uint32_t audio_freq)
{
    uint8_t res;
    
    res = WM8978_Init();
    if(res == 0)
    {
        return 1;
    }
    I2S_GPIO_Config();
    I2S_Mode_Config(I2S_STD_PHILLIPS, I2S_DATA_FMT_16BITS, audio_freq);
    I2S_TX_DMA_Init(NULL, NULL);
    
    /* Configure WM8978, DAC as input��Output as headphone */
    WM8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
    /* Adjust the volume, the left and right volume is the same */
    WM8978_SetOUT1Volume(volume);
    
    /* Configuring the audio interface as phillips��16bit */
    WM8978_CfgAudioIF(I2S_STD_PHILLIPS, 16);

    I2S_Play_Stop();
    return 0;
}

/**
*\*\name    WM8978_SetOUT1Volume.
*\*\fun     Configures OUT1 volume.
*\*\param   volume: volume
*\*\return  none
**/
void WM8978_SetOUT1Volume(uint8_t Volume)
{
    uint16_t regL;
    uint16_t regR;

    if (Volume > VOLUME_MAX)
    {
        Volume = VOLUME_MAX;
    }

    regL = Volume;
    regR = Volume;

    /*  R52 LOUT1 Volume control
        R53 ROUT1 Volume control
    */
    /* Update left channel volume */
    WM8978_WriteReg(52, regL | 0x00);

    /* Update right channel volume */
    WM8978_WriteReg(53, regR | 0x100);
}


/**
*\*\name    WM8978_SetOUT2Volume.
*\*\fun     Configures OUT2 volume.
*\*\param   volume: volume
*\*\return  none
**/
void WM8978_SetOUT2Volume(uint8_t Volume)
{
    uint16_t regL;
    uint16_t regR;

    if (Volume > VOLUME_MAX)
    {
        Volume = VOLUME_MAX;
    }

    regL = Volume;
    regR = Volume;

    /*
        R54 LOUT2 (SPK) Volume control
        R55 ROUT2 (SPK) Volume control
    */
    /* Update left channel volume */
    WM8978_WriteReg(54, regL | 0x00);

    /* Update right channel volume */
    WM8978_WriteReg(55, regR | 0x100);
}

/**
*\*\name    WM8978_ReadOUT1Volume.
*\*\fun     Read OUT1 volume.
*\*\param   none
*\*\return  volume
**/
uint8_t WM8978_ReadOUT1Volume(void)
{
    return (uint8_t)(WM8978_ReadReg(52) & 0x3F );
}

/**
*\*\name    WM8978_ReadOUT2Volume.
*\*\fun     Read OUT2 volume.
*\*\param   none
*\*\return  volume
**/
uint8_t WM8978_ReadOUT2Volume(void)
{
    return (uint8_t)(WM8978_ReadReg(54) & 0x3F );
}

/**
*\*\name    WM8978_OutMute.
*\*\fun     Set WM8978 mute.
*\*\param   1: Enable mute, 0:disable mute
*\*\return  none
**/
void WM8978_OutMute(uint8_t Mute)
{
    uint16_t RegValue;

    if (Mute == 1)
    {
        RegValue = WM8978_ReadReg(52); /* Left Mixer Control */
        RegValue |= (1u << 6);
        WM8978_WriteReg(52, RegValue);

        RegValue = WM8978_ReadReg(53); /* Left Mixer Control */
        RegValue |= (1u << 6);
        WM8978_WriteReg(53, RegValue);

        RegValue = WM8978_ReadReg(54); /* Right Mixer Control */
        RegValue |= (1u << 6);
        WM8978_WriteReg(54, RegValue);

        RegValue = WM8978_ReadReg(55); /* Right Mixer Control */
        RegValue |= (1u << 6);
        WM8978_WriteReg(55, RegValue);
    }
    else
    {
        RegValue = WM8978_ReadReg(52);
        RegValue &= ~(1u << 6);
        WM8978_WriteReg(52, RegValue);

        RegValue = WM8978_ReadReg(53); /* Left Mixer Control */
        RegValue &= ~(1u << 6);
        WM8978_WriteReg(53, RegValue);

        RegValue = WM8978_ReadReg(54);
        RegValue &= ~(1u << 6);
        WM8978_WriteReg(54, RegValue);

        RegValue = WM8978_ReadReg(55); /* Left Mixer Control */
        RegValue &= ~(1u << 6);
        WM8978_WriteReg(55, RegValue);
    }
}

/**
*\*\name    WM8978_SetMicGain.
*\*\fun     Set WM8978 gain.
*\*\param   Gain: 0-63
*\*\return  none
**/
void WM8978_SetMicGain(uint8_t Gain)
{
    if (Gain > GAIN_MAX)
    {
        Gain = GAIN_MAX;
    }

    /* PGA volume control R45��R46 
        Bit8    INPPGAUPDATE
        Bit7    INPPGAZCL       Cross zero and modify
        Bit6    INPPGAMUTEL     PGA mute
        Bit5:0  gain��010000 is 0dB
    */
    WM8978_WriteReg(45, Gain);
    WM8978_WriteReg(46, Gain | (1 << 8));
}

/**
*\*\name    WM8978_SetLineGain.
*\*\fun     Set WM8978 Line input channel gain.
*\*\param   Gain: 0-7. 7 max, 0 min
*\*\return  none
**/
void WM8978_SetLineGain(uint8_t Gain)
{
    uint16_t RegValue;

    if (Gain > 7)
    {
        Gain = 7;
    }
    RegValue = WM8978_ReadReg(47);
    RegValue &= 0x8F;/* Clear Bit6:4 0   1000 1111*/
    RegValue |= (Gain << 4);
    WM8978_WriteReg(47, RegValue);	/* Write left channel input gain control register*/

    RegValue = WM8978_ReadReg(48);
    RegValue &= 0x8F;/* Clear Bit6:4 0   1000 1111*/
    RegValue |= (Gain << 4);
    WM8978_WriteReg(48, RegValue);	/* Write right channel input gain control register */
}

/**
*\*\name    WM8978_PowerDown.
*\*\fun     Close WM8978, enter low power mode.
*\*\param   none
*\*\return  none
**/
void WM8978_PowerDown(void)
{
    WM8978_Reset();
}

/**
*\*\name    WM8978_CfgAudioIF.
*\*\fun     Configure WM8978 audio interface(I2S).
*\*\param   Standard:
*\*\            - I2S_STD_PHILLIPS
*\*\            - I2S_STD_MSB_ALIGN
*\*\            - I2S_STD_LSB_ALIGN
*\*\param   WordLen:
*\*\            - 16
*\*\            - 24
*\*\            - 32
*\*\return  none
**/
void WM8978_CfgAudioIF(uint16_t Standard, uint8_t WordLen)
{
    uint16_t temp;

    temp = 0;
    if (Standard == I2S_STD_PHILLIPS)
    {
        temp |= (2 << 3);
    }
    else if (Standard == I2S_STD_MSB_ALIGN)
    {
        temp |= (1 << 3);
    }
    else if (Standard == I2S_STD_LSB_ALIGN)
    {
        temp |= (0 << 3);
    }
    else    /* PCM */
    {
        temp |= (3 << 3);;
    }

    if (WordLen == 24)
    {
        temp |= (2 << 5);
    }
    else if (WordLen == 32)
    {
        temp |= (3 << 5);
    }
    else
    {
        temp |= (0 << 5);   /* 16bit */
    }
    WM8978_WriteReg(4, temp);

    /*
        R6��Clock generation control register
        MS = 0,  WM8978, MCLK supplied by MCU
    */
    WM8978_WriteReg(6, 0x000);
}

/**
*\*\name    WM8978_CfgAudioPath.
*\*\fun     Configure WM8978 audio interface(I2S).
*\*\param   InPath: Audio input channel configuration
*\*\param   OutPath: Audio Output channel configuration
*\*\return  none
**/
void WM8978_CfgAudioPath(uint16_t InPath, uint16_t OutPath)
{
    uint16_t temp;

    if ((InPath == IN_PATH_OFF) && (OutPath == OUT_PATH_OFF))
    {
        WM8978_PowerDown();
        return;
    }

    /*
        R1   register Power manage 1
        Bit8    BUFDCOPEN,  Output stage 1.5xAVDD/2 driver enable
        Bit7    OUT4MIXEN, OUT4 mixer enable
        Bit6    OUT3MIXEN, OUT3 mixer enable
        Bit5    PLLEN   .not used
        Bit4    MICBEN  ,Microphone Bias Enable
        Bit3    BIASEN  ,Analogue amplifier bias control , the analog amplifier must be set to 1 to work
        Bit2    BUFIOEN , Unused input/output tie off buffer enable
        Bit1:0  VMIDSEL ,  the analog amplifier must be set to non-zero to work
    */
    temp = (1 << 3) | (3 << 0);
    if (OutPath & OUT3_4_ON)
    {
        temp |= ((1 << 7) | (1 << 6));
    }
    if ((InPath & MIC_LEFT_ON) || (InPath & MIC_RIGHT_ON))
    {
        temp |= (1 << 4);
    }
    WM8978_WriteReg(1, temp);

    /*
        R2 register Power manage 2
        Bit8    ROUT1EN,  the analog amplifier must be set to 1 to workROUT1 output enable. The right channel output of the headset is enabled
        Bit7    LOUT1EN,  the analog amplifier must be set to 1 to workLOUT1 output enable. The left channel output of the headset is enabled
        Bit6    SLEEP,    the analog amplifier must be set to 1 to work0 = Normal device operation   1 = Residual current reduced in device standby mode
        Bit5    BOOSTENR, the analog amplifier must be set to 1 to work,Right channel Input BOOST enable 
        Bit4    BOOSTENL, the analog amplifier must be set to 1 to work,Left channel Input BOOST enable
        Bit3    INPGAENR, the analog amplifier must be set to 1 to work,Right channel input PGA enable 
        Bit2    INPGAENL, the analog amplifier must be set to 1 to work,Left channel input PGA enable
        Bit1    ADCENR,   the analog amplifier must be set to 1 to workEnable ADC right channel
        Bit0    ADCENL,   the analog amplifier must be set to 1 to workEnable ADC left channel
    */
    temp = 0;
    if (OutPath & EAR_LEFT_ON)
    {
        temp |= (1 << 7);
    }
    if (OutPath & EAR_RIGHT_ON)
    {
        temp |= (1 << 8);
    }
    if (InPath & MIC_LEFT_ON)
    {
        temp |= ((1 << 4) | (1 << 2));
    }
    if (InPath & MIC_RIGHT_ON)
    {
        temp |= ((1 << 5) | (1 << 3));
    }
    if (InPath & LINE_ON)
    {
        temp |= ((1 << 4) | (1 << 5));
    }
    if (InPath & MIC_RIGHT_ON)
    {
        temp |= ((1 << 5) | (1 << 3));
    }
    if (InPath & ADC_ON)
    {
        temp |= ((1 << 1) | (1 << 0));
    }
    WM8978_WriteReg(2, temp);

    /*
        R3 register Power manage 3
        Bit8    OUT4EN,     OUT4 enable
        Bit7    OUT3EN,     OUT3 enable
        Bit6    LOUT2EN,    LOUT2 output enable
        Bit5    ROUT2EN,    ROUT2 output enable
        Bit4    0,  
        Bit3    RMIXEN,     Right mixer enable
        Bit2    LMIXEN,     Left mixer enable
        Bit1    DACENR,     Right channel DAC enable
        Bit0    DACENL,     Left channel DAC enable
    */
    temp = 0;
    if (OutPath & OUT3_4_ON)
    {
        temp |= ((1 << 8) | (1 << 7));
    }
    if (OutPath & SPK_ON)
    {
        temp |= ((1 << 6) | (1 << 5));
    }
    if (OutPath != OUT_PATH_OFF)
    {
        temp |= ((1 << 3) | (1 << 2));
    }
    if (InPath & DAC_ON)
    {
        temp |= ((1 << 1) | (1 << 0));
    }
    WM8978_WriteReg(3, temp);

    /*
        R44 register Input ctrl

        Bit8    MBVSEL,         Microphone Bias Voltage Control   0 = 0.9 * AVDD   1 = 0.6 * AVDD
        Bit7    0   
        Bit6    R2_2INPPGA,     Connect R2 pin to right channel input PGA positive terminal
        Bit5    RIN2INPPGA,     Connect RIN pin to right channel input PGA negative terminal
        Bit4    RIP2INPPGA,     Connect RIP pin to right channel input PGA amplifier positive terminal
        Bit3    0           
        Bit2    L2_2INPPGA,     Connect L2 pin to left channel input PGA positive terminal
        Bit1    LIN2INPPGA,     Connect LIN pin to left channel input PGA negative terminal
        Bit0    LIP2INPPGA,     Connect LIP pin to left channel input PGA amplifier positive terminal
    */
    temp = 0 << 8;
    if (InPath & LINE_ON)
    {
        temp |= ((1 << 6) | (1 << 2));
    }
    if (InPath & MIC_RIGHT_ON)
    {
        temp |= ((1 << 5) | (1 << 4));
    }
    if (InPath & MIC_LEFT_ON)
    {
        temp |= ((1 << 1) | (1 << 0));
    }
    WM8978_WriteReg(44, temp);


    /*
        R14     ADC Control
        Bit8    HPFEN,  High Pass Filter Enable
        Bit7    HPFAPP, Select audio mode or application mode��0: audio mode
        Bit6:4  HPFCUT, Application mode cut-off frequency  000-111
        Bit3    ADCOSR, ADC oversample rate select: 0=64x (lower power) 1=128x (best performance)
        Bit2      0
        Bit1    ADC right channel polarity adjust:  0=normal  1=inverted
        Bit0    ADC left channel polarity adjust:  0=normal 1=inverted
    */
    if (InPath & ADC_ON)
    {
        temp = (1 << 3) | (0 << 8) | (4 << 0);
    }
    else
    {
        temp = 0;
    }
    WM8978_WriteReg(14, temp);

    /* 
        R27��R28��R29��R30 Used to control the wave limiting filter
        R7 Bit7 NFEN = 0: disable��1: enable
    */
    if (InPath & ADC_ON)
    {
        temp = (0 << 7);
        WM8978_WriteReg(27, temp);
        temp = 0;
        WM8978_WriteReg(28, temp);
        WM8978_WriteReg(29, temp);
        WM8978_WriteReg(30, temp);
    }

    {
        temp = 0;      /* Disable automatic gain control */
        WM8978_WriteReg(32, temp);
        WM8978_WriteReg(33, temp);
        WM8978_WriteReg(34, temp);
    }

    /*  R35  ALC Noise Gate Control
        Bit3    NGATEN, Noise gate function enable
        Bit2:0  Noise gate threshold:
    */
    temp = (3 << 1) | (7 << 0);     /* Disable automatic gain control */
    WM8978_WriteReg(35, temp);

    /*  WM8978��R47(left channel), R48(right channel), MIC gain control register
        R47 (R48 as same to R47)
        B8      PGABOOSTL   = 1,   0: Indicates that the MIC signal has no gain through��1: Indicates that the MIC signal has +20dB gain
        B7      = 0�� reserved
        B6:4    L2_2BOOSTVOL = x,  0: disable ��1-7:-12dB ~ +6dB 
        B3      = 0,  reserved
        B2:0    AUXL2BOOSTVOL = x, 0: disable ��1-7:-12dB ~ +6dB 
    */
    temp = 0;
    if ((InPath & MIC_LEFT_ON) || (InPath & MIC_RIGHT_ON))
    {
        temp |= (1 << 8);   /* MIC has +20dB gain*/
    }
    if (InPath & AUX_ON)
    {
        temp |= (3 << 0);   /* Aux gain select 3, user can adjust */
    }
    if (InPath & LINE_ON)
    {
        temp |= (3 << 4);   /* Line gain select 3, user can adjust */
    }
    WM8978_WriteReg(47, temp);
    WM8978_WriteReg(48, temp);

    /* 
        R15 Control left channel ADC volume
        R16 Control right channel ADC volume
        Bit8    ADCVU  = 1 can updated , used the ADC volume of the left and right channels is updated synchronously
        Bit7:0  gain select: 0000 0000 = mute
                             0000 0001 = -127dB
                             0000 0010 = -12.5dB  (0.5dB step)
                             1111 1111 = 0dB
    */
    temp = 0xFF;
    WM8978_WriteReg(15, temp);
    temp = 0x1FF;
    WM8978_WriteReg(16, temp);	/* updated left and right channels synchronously */


    /*  R43 register AUXR �C ROUT2 BEEP Mixer Function
        B8:6 = 0

        B5   MUTERPGA2INV,  Mute input to INVROUT2 mixer
        B4   INVROUT2,  Invert ROUT2 output , used to speaker
        B3:1 BEEPVOL = 7;   AUXR input to ROUT2 inverter gain
        B0   BEEPEN = 1;    Enable AUXR beep input

    */
    temp = 0;
    if (OutPath & SPK_ON)
    {
        temp |= (1 << 4);   /* ROUT2 Reverse phase, used to speaker */
    }
    if (InPath & AUX_ON)
    {
        temp |= ((7 << 1) | (1 << 0));
    }
    WM8978_WriteReg(43, temp);

    /* R49  Output ctrl
        B8:7    0
        B6      DACL2RMIX,  Left DAC output to right output mixer
        B5      DACR2LMIX,  Right DAC output to left output
        B4      OUT4BOOST,  0 = OUT4 output gain = -1; DC = AVDD / 2��1 = OUT4 output gain = +1.5��DC = 1.5 x AVDD / 2
        B3      OUT3BOOST,  0 = OUT3 output gain = -1; DC = AVDD / 2��1 = OUT3 output gain = +1.5��DC = 1.5 x AVDD / 2
        B2      SPKBOOST,   0 = Speaker gain = -1;  DC = AVDD / 2 ; 1 = Speaker gain = +1.5; DC = 1.5 x AVDD / 2
        B1      TSDEN,   Thermal Shutdown Enable
        B0      VROI,   Disabled Outputs to VREF Resistance
    */
    temp = 0;
    if (InPath & DAC_ON)
    {
        temp |= ((1 << 6) | (1 << 5));
    }
    if (OutPath & SPK_ON)
    {
        temp |=  ((1 << 2) | (1 << 1));	/* SPK 1.5x gain, Thermal Shutdown Enable */
    }
    if (OutPath & OUT3_4_ON)
    {
        temp |=  ((1 << 4) | (1 << 3));	/* BOOT3  BOOT4  1.5x gain */
    }
    WM8978_WriteReg(49, temp);

    /*  REG 50    (50 is left channel��51 is right channel)
        B8:6    AUXLMIXVOL = 111    AUX is used for FM radio signal input
        B5      AUXL2LMIX = 1       Left Auxilliary input to left channel
        B4:2    BYPLMIXVOL          volume
        B1      BYPL2LMIX = 0;      Left bypass path (from the left channel input boost output) to left output mixer
        B0      DACL2LMIX = 1;      Left DAC output to left output mixer
    */
    temp = 0;
    if (InPath & AUX_ON)
    {
        temp |= ((7 << 6) | (1 << 5));
    }
    if ((InPath & LINE_ON) || (InPath & MIC_LEFT_ON) || (InPath & MIC_RIGHT_ON))
    {
        temp |= ((7 << 2) | (1 << 1));
    }
    if (InPath & DAC_ON)
    {
        temp |= (1 << 0);
    }
    WM8978_WriteReg(50, temp);
    WM8978_WriteReg(51, temp);

    /*  R56 register  OUT3 mixer ctrl
        B8:7    0
        B6      OUT3MUTE,   0 = Output stage outputs OUT3 mixer;  1 = Output stage muted �C drives out VMID.
        B5:4    0
        B3      BYPL2OUT3,  OUT4 mixer output to OUT3
        B4      0
        B2      LMIX2OUT3,  Left ADC input to OUT3
        B1      LDAC2OUT3,  Left DAC mixer to OUT3
        B0      LDAC2OUT3,  Left DAC output to OUT3
    */
    temp = 0;
    if (OutPath & OUT3_4_ON)
    {
        temp |= (1 << 3);
    }
    WM8978_WriteReg(56, temp);

    /* R57 register     OUT4 (MONO) mixer ctrl
        B8:7    0
        B6      OUT4MUTE,   0 = Output stage outputs OUT4 mixer  1 = Output stage muted �C drives outVMID.
        B5      HALFSIG,    0 = OUT4 normal output	1 = OUT4 attenuated by 6dB
        B4      LMIX2OUT4,  Left DAC mixer to OUT4
        B3      LDAC2UT4,   Left DAC to OUT4
        B2      BYPR2OUT4,  Right ADC input to OUT4
        B1      RMIX2OUT4,  Right DAC mixer to OUT4
        B0      RDAC2OUT4,  Right DAC output to OUT4
    */
    temp = 0;
    if (OutPath & OUT3_4_ON)
    {
        temp |= ((1 << 4) |  (1 << 1));
    }
    WM8978_WriteReg(57, temp);


    /* R11, 12 register DAC Digital Volume
       R11     Left DAC Digital Volume
       R12     Right DAC Digital Volume
    */
    if (InPath & DAC_ON)
    {
        WM8978_WriteReg(11, 255);
        WM8978_WriteReg(12, 255 | 0x100);
    }
    else
    {
        WM8978_WriteReg(11, 0);
        WM8978_WriteReg(12, 0 | 0x100);
    }

    /*  R10 register DAC Control
        B8  0
        B7  0
        B6  SOFTMUTE,   Softmute enable:
        B5  0
        B4  0
        B3  DACOSR128,  DAC oversampling rate: 0=64x (lowest power) 1=128x (best performance)
        B2  AMUTE,      Automute enable
        B1  DACPOLR,    Right DAC output polarity
        B0  DACPOLL,    Left DAC output polarity:
    */
    if (InPath & DAC_ON)
    {
        WM8978_WriteReg(10, 0);
    }
}

/**
*\*\name    WM8978_NotchFilter.
*\*\fun     Set notch filter.
*\*\param   NFA0[13:0] and NFA1[13:0]
*\*\return  none.
**/
void WM8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1)
{
    uint16_t temp;

    /*  page 26
        A programmable notch filter is provided. This filter has a variable centre frequency and bandwidth,
        programmable via two coefficients, a0 and a1. a0 and a1 are represented by the register bits
        NFA0[13:0] and NFA1[13:0]. Because these coefficient values require four register writes to setup
        there is an NFU (Notch Filter Update) flag which should be set only when all four registers are setup.
    */
    temp = (1 << 7) | (_NFA0 & 0x3F);
    WM8978_WriteReg(27, temp);

    temp = ((_NFA0 >> 7) & 0x3F);
    WM8978_WriteReg(28, temp);

    temp = (_NFA1 & 0x3F);
    WM8978_WriteReg(29, temp);

    temp = (1 << 8) | ((_NFA1 >> 7) & 0x3F);
    WM8978_WriteReg(30, temp);
}

/**
*\*\name    WM8978_CtrlGPIO1.
*\*\fun     Control WM8978 GPIO ouput 0 or 1
*\*\param   Value: GPIO1 output value, 0 or 1
*\*\return  none.
**/
void WM8978_CtrlGPIO1(uint8_t Value)
{
    uint16_t RegValue;

    if (Value == 0) /* output 0 */
    {
        RegValue = 6; /* B2:0 = 110 */
    }
    else
    {
        RegValue = 7; /* B2:0 = 111 */
    }
    WM8978_WriteReg(8, RegValue);
}

/**
*\*\name    WM8978_Reset.
*\*\fun     Reset WM8978 register
*\*\param   Value: GPIO1 output value, 0 or 1
*\*\return  1: success, 0: failed.
**/
uint8_t WM8978_Reset(void)
{
    /* WM8978 register default value */
    const uint16_t reg_default[] = {
    0x000, 0x000, 0x000, 0x000, 0x050, 0x000, 0x140, 0x000,
    0x000, 0x000, 0x000, 0x0FF, 0x0FF, 0x000, 0x100, 0x0FF,
    0x0FF, 0x000, 0x12C, 0x02C, 0x02C, 0x02C, 0x02C, 0x000,
    0x032, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
    0x038, 0x00B, 0x032, 0x000, 0x008, 0x00C, 0x093, 0x0E9,
    0x000, 0x000, 0x000, 0x000, 0x003, 0x010, 0x010, 0x100,
    0x100, 0x002, 0x001, 0x001, 0x039, 0x039, 0x039, 0x039,
    0x001, 0x001
    };
    uint8_t res;
    uint8_t i;

    res = WM8978_WriteReg(0x00, 0);

    for (i = 0; i < sizeof(reg_default) / 2; i++)
    {
        WM8978_RegCash[i] = reg_default[i];
    }
    return res;
}

/**
*\*\name    I2S_GPIO_Config.
*\*\fun     Configure WM8978 I2S IO
*\*\param   none
*\*\return  none
**/
void I2S_GPIO_Config(void)
{
    GPIO_InitType GPIO_InitStructure;

    /* gpio clocks enable */
    RCC_EnableAHB1PeriphClk(AUDIO_I2S_CK_GPIO_CLK | AUDIO_I2S_WS_GPIO_CLK | AUDIO_I2S_SD_GPIO_CLK  | AUDIO_I2S_AUX_SD_GPIO_CLK | AUDIO_I2S_MCK_GPIO_CLK, ENABLE);
    /* Enable peripheral clocks */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);

    /* Configure I2S2 pins: CK, WS, SD */
    GPIO_InitStructure.Pin            = AUDIO_I2S_CK_PIN;
    GPIO_InitStructure.GPIO_Alternate = AUDIO_I2S_CK_GPIO_AF;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitPeripheral(AUDIO_I2S_CK_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = AUDIO_I2S_WS_PIN;
    GPIO_InitStructure.GPIO_Alternate = AUDIO_I2S_WS_GPIO_AF;
    GPIO_InitPeripheral(AUDIO_I2S_WS_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.Pin            = AUDIO_I2S_SD_PIN;
    GPIO_InitStructure.GPIO_Alternate = AUDIO_I2S_SD_GPIO_AF;
    GPIO_InitPeripheral(AUDIO_I2S_SD_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin            = AUDIO_I2S_MCK_PIN;
    GPIO_InitStructure.GPIO_Alternate = AUDIO_I2S_MCK_GPIO_AF;
    GPIO_InitPeripheral(AUDIO_I2S_MCK_GPIO_PORT, &GPIO_InitStructure);

    /* Configure I2S_EXT pins: AUX_SD */
    GPIO_InitStructure.Pin            = AUDIO_I2S_AUX_SD_PIN;
    GPIO_InitStructure.GPIO_Alternate = AUDIO_I2S_AUX_SD_GPIO_AF;
    GPIO_InitPeripheral(AUDIO_I2S_AUX_SD_GPIO_PORT, &GPIO_InitStructure);
}

/**
*\*\name    I2S_Stop.
*\*\fun     I2S stop transmite
*\*\param   none
*\*\return  none
**/
void I2S_Stop(void)
{
    DMA_EnableChannel(I2S_MASTER_Tx_DMA_Channel, DISABLE);
    DMA_EnableChannel(I2S_EXT_Rx_DMA_Channel, DISABLE);
}

/**
*\*\name    I2S_Mode_Config.
*\*\fun     Configure I2S mode
*\*\param   Standard:
*\*\            - I2S_STD_PHILLIPS
*\*\            - I2S_STD_MSB_ALIGN
*\*\            - I2S_STD_LSB_ALIGN
*\*\param   WordLen:
*\*\            - I2S_DATA_FMT_16BITS          
*\*\            - I2S_DATA_FMT_16BITS_EXTENDED 
*\*\            - I2S_DATA_FMT_24BITS          
*\*\            - I2S_DATA_FMT_32BITS          
*\*\param  AudioFreq: 8K~192K
*\*\return  none
**/
void I2S_Mode_Config(uint16_t Standard, uint16_t WordLen, uint32_t AudioFreq)
{
    uint32_t timeout_value;
    uint32_t counter_value = 0;
    RCC_ClocksType sysclk_value;
    FlagStatus status_value;
    I2S_InitType I2S_InitStructure;

    RCC_GetClocksFreqValue(&sysclk_value);
    timeout_value = (HSE_STARTUP_TIMEOUT/((uint32_t)240000000/sysclk_value.SysclkFreq));
	
    /* Enable phase reset to filter waveform burrs */
    RCC_EnableSHRTPLLPHAReset(ENABLE);
    if(RCC_ConfigSHRTPll(RCC_SHRTPLL_SRC_HSE, HSE_VALUE, shrtpll_clock, ENABLE) == SUCCESS)
    {
        /* Wait till SHRTPLL is ready */
        do
        {
            status_value = RCC_GetFlagStatus(RCC_FLAG_SHRTPLLRDF);
            counter_value++;
        } while ((counter_value != timeout_value) && (status_value == RESET));
        
        if (RCC_GetFlagStatus(RCC_FLAG_SHRTPLLRDF) != RESET)
        {
            /* Disable phase reset */
            RCC_EnableSHRTPLLPHAReset(DISABLE);
            RCC_ConfigI2S2Clk(RCC_I2S2_CLKSRC_SHRTPLL);
        }
        else
        {
            RCC_ConfigI2S2Clk(RCC_I2S2_CLKSRC_SYSCLK);
            shrtpll_clock = sysclk_value.SysclkFreq;
        }
    }
    else
    {
        RCC_ConfigI2S2Clk(RCC_I2S2_CLKSRC_SYSCLK);
        shrtpll_clock = sysclk_value.SysclkFreq;
    }
    
    /*  I2S clocks enable */
    RCC_EnableAPB1PeriphClk(AUDIO_I2S_CLK, ENABLE);

    SPI_I2S_DeInit(AUDIO_I2S);
    /* I2S peripheral configuration */
    I2S_InitStruct(&I2S_InitStructure);
    I2S_InitStructure.Standard        = Standard;
    I2S_InitStructure.DataFormat      = WordLen;
    I2S_InitStructure.MCLKEnable      = I2S_MCLK_ENABLE;
    I2S_InitStructure.AudioFrequency  = AudioFreq;
    I2S_InitStructure.CLKPOL          = I2S_CLKPOL_LOW;
    I2S_InitStructure.ClkSrcFrequency = shrtpll_clock;

    /* I2S configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_MASTER_TX;
    I2S_Init(AUDIO_I2S, &I2S_InitStructure);

    /* Enable the I2S */
    I2S_Enable(AUDIO_I2S, ENABLE);
}

/**
*\*\name    I2S_TX_DMA_Init.
*\*\fun     Initiliaze I2S tx DMA
*\*\param   buf: memory address
*\*\param   size: data number(HalfWord)
*\*\return  none
**/
void I2S_TX_DMA_Init(uint8_t* buf, uint32_t size)
{  
    DMA_InitType DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    
    RCC_EnableAHBPeriphClk(I2S_MASTER_DMA_CLK, ENABLE);
    
    /* I2S_MASTER_Tx_DMA_Channel configuration */
    DMA_DeInit(I2S_MASTER_Tx_DMA_Channel);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&I2S_MASTER->DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)buf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize        = size;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(I2S_MASTER_Tx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(I2S_MASTER_Tx_DMA_Remap,I2S_MASTER_Tx_DMA_Channel,ENABLE);
    
    /* Enable DMAx Channel x Transfer Complete interrupt */
    DMA_ConfigInt(I2S_MASTER_Tx_DMA_Channel, DMA_INT_TXC, ENABLE);
    
    /* Enable I2S_MASTER Tx request */
    SPI_I2S_EnableDma(I2S_MASTER, SPI_I2S_DMA_TX, ENABLE);
        
    /* Enable I2S TX IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel                   = I2S_MASTER_Tx_DMA_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    I2S_TX_DMA_Add_SIZE_Set.
*\*\fun     Configures I2S DMA.
*\*\param   buf: memory address
*\*\param   size: buffer size
*\*\return  none
**/
void I2S_TX_DMA_Add_SIZE_Set(uint8_t* buf, uint32_t size)
{
    I2S_Play_Stop();
    DMA_ClearFlag(I2S_MASTER_Tx_DMA_FLAG, I2S_MASTER_DMA);
    while((I2S_MASTER_Tx_DMA_Channel->CHCFG & DMA_CHANNEL_ENABLE))
    {
    }
    I2S_MASTER_Tx_DMA_Channel->MADDR = (uint32_t)buf;
    I2S_MASTER_Tx_DMA_Channel->TXNUM = size;
    I2S_Play_Start();
}

/**
*\*\name    I2S_MASTER_DMA_IRQ_HANDLER.
*\*\fun     This function is DMA interrupt request handles. 
*\*\param   none 
*\*\return  none 
**/
void I2S_MASTER_DMA_IRQ_HANDLER(void)
{
    /* Test on DMA channel Transfer Complete interrupt */
    if (DMA_GetIntStatus(I2S_MASTER_DMA_IT_FLAG_TC, I2S_MASTER_DMA))
    {
        /* Clear DMA channel Half Transfer, Transfer Complete and Global interrupt pending bits */
        DMA_ClrIntPendingBit(I2S_MASTER_DMA_IT_FLAG_TC, I2S_MASTER_DMA);
//        I2S_DMA_TX_Callback();
    }
}

/**
*\*\name    I2S_Play_Start.
*\*\fun     I2S start send data
*\*\param   none
*\*\return  none
**/
void I2S_Play_Start(void)
{
    DMA_EnableChannel(I2S_MASTER_Tx_DMA_Channel,ENABLE);
}

/**
*\*\name    I2S_Play_Stop.
*\*\fun     I2S stop send data
*\*\param   none
*\*\return  none
**/
void I2S_Play_Stop(void)
{
    DMA_EnableChannel(I2S_MASTER_Tx_DMA_Channel,DISABLE);
}

/**
*\*\name    I2Sext_Mode_Config.
*\*\fun     Configure I2Sext mode
*\*\param   Standard:
*\*\            - I2S_STD_PHILLIPS
*\*\            - I2S_STD_MSB_ALIGN
*\*\            - I2S_STD_LSB_ALIGN
*\*\param   WordLen:
*\*\            - 16
*\*\            - 24
*\*\            - 32
*\*\param  AudioFreq: 8K~192K
*\*\return  none
**/
void I2Sext_Mode_Config(uint16_t Standard, uint16_t WordLen, uint32_t AudioFreq)
{
    I2S_InitType I2S_InitStructure;

    /* I2S peripheral configuration */
    I2S_InitStruct(&I2S_InitStructure);
    I2S_InitStructure.Standard       = Standard;
    I2S_InitStructure.DataFormat     = WordLen;
    I2S_InitStructure.MCLKEnable     = I2S_MCLK_ENABLE;
    I2S_InitStructure.AudioFrequency = AudioFreq;
    I2S_InitStructure.CLKPOL         = I2S_CLKPOL_LOW;

    /* I2S_EXT configuration */
    I2S_InitStructure.I2sMode = I2S_MODE_SlAVE_RX;
    I2S_EXT_Init(I2S_EXT, &I2S_InitStructure); 

    /* Enable the I2S_EXT */
    I2S_EXT_Enable(I2S_EXT, ENABLE);
}

/**
*\*\name    I2S_RX_DMA_Init.
*\*\fun     Initiliaze I2S rx DMA
*\*\param   buf: memory address
*\*\param   size: data number(HalfWord)
*\*\return  none
**/
void I2Sext_RX_DMA_Init(uint8_t* buf, uint32_t size)
{  
    DMA_InitType DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    
    RCC_EnableAHBPeriphClk(I2S_EXT_DMA_CLK, ENABLE);

    /* I2S_SLAVE_Rx_DMA_Channel configuration ---------------------------------------------*/
    DMA_DeInit(I2S_EXT_Rx_DMA_Channel);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.PeriphAddr     = (uint32_t)&I2S_EXT->I2S_DAT;
    DMA_InitStructure.MemAddr        = (uint32_t)buf;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;
    DMA_InitStructure.BufSize        = size;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD;;
    DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;
    DMA_InitStructure.CircularMode   = DMA_MODE_CIRCULAR;
    DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(I2S_EXT_Rx_DMA_Channel, &DMA_InitStructure);
    DMA_RequestRemap(I2S_EXT_Rx_DMA_Remap,I2S_EXT_Rx_DMA_Channel,ENABLE);

    /* Enable DMAx Channel x Transfer Complete interrupt */
    DMA_ConfigInt(I2S_EXT_Rx_DMA_Channel, DMA_INT_HTX | DMA_INT_TXC, ENABLE);
    
    /* Enable I2S_SLAVE Rx request */
    I2S_EXT_EnableDma(I2S2_EXT, I2S_EXT_DMA_RX, ENABLE);

    /* Enable I2S RX IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel                   = I2S_EXT_Rx_DMA_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
*\*\name    I2S_EXTDMA_IRQ_HANDLER.
*\*\fun     This function is DMA interrupt request handles. 
*\*\param   none 
*\*\return  none 
**/
void I2S_EXTDMA_IRQ_HANDLER(void)
{
    /* Test on DMA channel Transfer Complete interrupt */
    if (DMA_GetIntStatus(I2S_EXTDMA_IT_FLAG_TC, I2S_MASTER_DMA))
    {
        /* Clear DMA channel Half Transfer, Transfer Complete and Global interrupt pending bits */
        DMA_ClrIntPendingBit(I2S_EXTDMA_IT_FLAG_TC, I2S_MASTER_DMA);
        I2S_DMA_RX_Callback();
    }
    if(DMA_GetIntStatus(I2S_EXTDMA_IT_FLAG_HT, I2S_MASTER_DMA))
    {
        /* Clear DMA channel Half Transfer, Transfer Complete and Global interrupt pending bits */
        DMA_ClrIntPendingBit(I2S_EXTDMA_IT_FLAG_HT, I2S_MASTER_DMA);
        I2S_DMA_RX_Callback();
    }
}

/**
*\*\name    I2Sext_Recorde_Start.
*\*\fun     I2S start receive data
*\*\param   none
*\*\return  none
**/
void I2Sext_Recorde_Start(void)
{
    DMA_EnableChannel(I2S_EXT_Rx_DMA_Channel, ENABLE);
}

/**
*\*\name    I2Sext_Recorde_Stop.
*\*\fun     I2S stop receive data
*\*\param   none
*\*\return  none
**/
void I2Sext_Recorde_Stop(void)
{
    DMA_EnableChannel(I2S_EXT_Rx_DMA_Channel,DISABLE);
}

