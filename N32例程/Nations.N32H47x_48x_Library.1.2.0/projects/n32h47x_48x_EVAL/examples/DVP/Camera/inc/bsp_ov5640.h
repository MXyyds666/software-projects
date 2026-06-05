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
*\*\file bsp_ov5640.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#ifndef __BSP_OV5640_H__
#define __BSP_OV5640_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_dvp.h"
#include "log.h"

/* DVP pin group 1 */
#define DVP_MODULE_REMAP0_PCLK_PIN      GPIO_PIN_4
#define DVP_MODULE_REMAP0_PCLK_PORT     GPIOE
#define DVP_MODULE_REMAP0_PCLK_AF       GPIO_AF6


#define DVP_MODULE_REMAP0_HSYNC_PIN     GPIO_PIN_2
#define DVP_MODULE_REMAP0_HSYNC_PORT    GPIOE
#define DVP_MODULE_REMAP0_HSYNC_AF      GPIO_AF6


#define DVP_MODULE_REMAP0_VSYNC_PIN     GPIO_PIN_3
#define DVP_MODULE_REMAP0_VSYNC_PORT    GPIOE
#define DVP_MODULE_REMAP0_VSYNC_AF      GPIO_AF6


#define DVP_MODULE_REMAP0_DATA0_PIN     GPIO_PIN_5
#define DVP_MODULE_REMAP0_DATA0_PORT    GPIOE
#define DVP_MODULE_REMAP0_DATA0_AF      GPIO_AF7


#define DVP_MODULE_REMAP0_DATA1_PIN     GPIO_PIN_6
#define DVP_MODULE_REMAP0_DATA1_PORT    GPIOE
#define DVP_MODULE_REMAP0_DATA1_AF      GPIO_AF7


#define DVP_MODULE_REMAP0_DATA2_PIN     GPIO_PIN_10
#define DVP_MODULE_REMAP0_DATA2_PORT    GPIOG
#define DVP_MODULE_REMAP0_DATA2_AF      GPIO_AF9


#define DVP_MODULE_REMAP0_DATA3_PIN     GPIO_PIN_11
#define DVP_MODULE_REMAP0_DATA3_PORT    GPIOG
#define DVP_MODULE_REMAP0_DATA3_AF      GPIO_AF9


#define DVP_MODULE_REMAP0_DATA4_PIN     GPIO_PIN_10
#define DVP_MODULE_REMAP0_DATA4_PORT    GPIOB
#define DVP_MODULE_REMAP0_DATA4_AF      GPIO_AF12


#define DVP_MODULE_REMAP0_DATA5_PIN     GPIO_PIN_11
#define DVP_MODULE_REMAP0_DATA5_PORT    GPIOB
#define DVP_MODULE_REMAP0_DATA5_AF      GPIO_AF12


#define DVP_MODULE_REMAP0_DATA6_PIN     GPIO_PIN_0
#define DVP_MODULE_REMAP0_DATA6_PORT    GPIOB
#define DVP_MODULE_REMAP0_DATA6_AF      GPIO_AF11


#define DVP_MODULE_REMAP0_DATA7_PIN     GPIO_PIN_1
#define DVP_MODULE_REMAP0_DATA7_PORT    GPIOB
#define DVP_MODULE_REMAP0_DATA7_AF      GPIO_AF11

#define CAMERA_RST         GPIO_PIN_4
#define CAMERA_RST_PORT    GPIOB


#define SCCB_SCL        GPIO_PIN_6
#define SCCB_SCL_PORT   GPIOB

#define SCCB_SDA        GPIO_PIN_7
#define SCCB_SDA_PORT   GPIOB

#define OV5640_ADDR         0x78    
#define OV5640_ID           0X5640  

#define OV5640_CHIPIDH          0X300A      
#define OV5640_CHIPIDL          0X300B     


#define DVP_OUTPUT_RGB565    0
#define DVP_OUTPUT_YUV422    1
#define DVP_OUTPUT_FORMAT    DVP_OUTPUT_RGB565

void OV5640_InitPort(void);
uint8_t OV5640_Init(void);
void OV5640_RGB565_Config(void);
void OV5640_Delay1( __IO uint32_t x);
void OV5640_Delay(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_OV5640_H__ */

