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
*\*\file bsp_dvp.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "bsp_dvp.h"

uint8_t g_DVP_int_flag = 0;

void DVPDemo_Init(void)
{
    DVP_NVIC_Configuration();  
    OV5640_InitPort();
    OV5640_Init();
    OV5640_RGB565_Config();
    DVP_InitInterface();
    DVP_ConfigInt(DVP_INT_M1TC,ENABLE);
    DVP_EnableBuffer1(ENABLE);
    DVP_EnablePort(ENABLE);
}


void DVP_InitInterface(void)
{
    DVP_InitType DVP_InitStructure;
    DVP_DMAInitType DVP_DMAInitStructure;
    
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DVP, ENABLE);    
    
    DVP_StructInit(&DVP_InitStructure);
    
    DVP_InitStructure.CaptureMode      = DVP_CAPTURE_MODE_CONTINUE;
    DVP_InitStructure.CropMode         = DVP_NORMAL_MODE;
    DVP_InitStructure.VBFilter         = 3;
    DVP_InitStructure.PixelClkPolarity = DVP_PIXEL_POLARITY_FALLING;
    DVP_InitStructure.HsyncPolarity    = DVP_HSYNC_POLARITY_HIGH;
    DVP_InitStructure.VsyncPolarity    = DVP_VSYNC_POLARITY_LOW;
    DVP_InitStructure.DataInvert       = DVP_DATA_NOTINVERT;
    DVP_InitStructure.DataReverse      = DVP_DATA_NOTREVERSE;
    DVP_InitStructure.DataMode         = DVP_DATA_MODE1;
    DVP_InitStructure.PixelByte        = DVP_PIXEL_BYTE1;
   
    DVP_Init(&DVP_InitStructure);
    
    DVP_DMAStructInit(&DVP_DMAInitStructure);
    
    DVP_DMAInitStructure.FrameBufferNum      = 1;
    DVP_DMAInitStructure.FrameBufferAddress1 = LCD_IMAGE_ADDRESS;
    DVP_DMAInitStructure.DisplayStatus       = DVP_DISPLAY_ADDRESS;
    DVP_DMAInitStructure.BurstSize           = DVP_BURST_SIZE_BYTE_4;
    DVP_DMAInitStructure.FifoThreshold       = DVP_FIFO_THRSEHOLD_BYTE_4;
    DVP_DMAInitStructure.FrameBufferSize     = DVP_IMAGE_SIZE;
    DVP_ConfigDma(&DVP_DMAInitStructure);
    
    
}

void DVP_NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel                   = DVP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}



