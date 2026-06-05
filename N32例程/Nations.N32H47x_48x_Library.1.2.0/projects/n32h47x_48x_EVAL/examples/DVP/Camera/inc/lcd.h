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
*\*\file lcd.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "misc.h"
#include "n32h47x_48x.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_femc.h"
#include "fonts.h"
    
typedef uint16_t color_t;

typedef struct
{
    uint16_t width;
    uint16_t height;
    void* img_data;
    uint8_t format;
} image_t;

enum
{
    FORMAT_GREY,
    FORMAT_RGB565,
    FORMAT_RGB24,
};



#define RGB565(r, g, b) ((color_t)((((r)&0xF8) << 8) | (((g)&0xFC) << 3) | (((b)&0xF8) >> 3)))
#define COLOR_WHITE     RGB565(255, 255, 255)
#define COLOR_BLACK     RGB565(0, 0, 0)
#define COLOR_GREY      RGB565(128, 128, 128)
#define COLOR_BLUE      RGB565(0, 0, 255)
#define COLOR_RED       RGB565(255, 0, 0)
#define COLOR_GREEN     RGB565(0, 255, 0)
#define COLOR_CYAN      RGB565(0, 255, 255)
#define COLOR_YELLOW    RGB565(255, 255, 0)
#define COLOR_PURPLE    RGB565(255, 0, 255)



#define DEBUG_DELAY()

#define CMD_SetCoordinateX 0x2A
#define CMD_SetCoordinateY 0x2B
#define CMD_SetPixel       0x2C

#define LCD_DispWindow_X_Star 0
#define LCD_DispWindow_Y_Star 0


#define LCD_SCAN_MODE   0

#define WHITE   0xFFFF
#define BLACK   0x0000
#define GREY    0xF7DE
#define BLUE    0x001F
#define BLUE2   0x051F
#define RED     0xF800
#define MAGENTA 0xF81F
#define GREEN   0x07E0
#define CYAN    0x7FFF
#define YELLOW  0xFFE0
#define BRED    0xF81F
#define GRED    0xFFE0
#define GBLUE   0x07FF

#define      IO_STATE_HIGH                (0x1U)   
#define      IO_STATE_LOW                 (0x0U)   

/* Select block */
#define      FSMC_NORSRAM_BLOCK             (FEMC_SN_BANK_BLOCK4)

/* A10 low when this address is accessed */
#define      FSMC_ADDR_LCD_CMD              (0x6C000000U)

/* A10 high when this address is accessed */    
#define      FSMC_ADDR_LCD_DATA             (0x6C080000U)

/* CS pin */
#define      LCD_FEMC_CS_CLK                RCC_AHB_PERIPHEN_GPIOG   
#define      LCD_FEMC_CS_PORT               GPIOG
#define      LCD_FEMC_CS_PIN                GPIO_PIN_12
#define      LCD_FEMC_CS_AF                 GPIO_AF10

/* RS pin */
#define      LCD_FEMC_RS_CLK                RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_RS_PORT               GPIOD
#define      LCD_FEMC_RS_PIN                GPIO_PIN_13
#define      LCD_FEMC_RS_AF                 GPIO_AF10

/* WR pin */
#define      LCD_FEMC_WR_CLK                RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_WR_PORT               GPIOD
#define      LCD_FEMC_WR_PIN                GPIO_PIN_5
#define      LCD_FEMC_WR_AF                 GPIO_AF10

/* RD pin */
#define      LCD_FEMC_RD_CLK                RCC_AHB_PERIPHEN_GPIOD  
#define      LCD_FEMC_RD_PORT               GPIOD
#define      LCD_FEMC_RD_PIN                GPIO_PIN_4
#define      LCD_FEMC_RD_AF                 GPIO_AF10

/* RST pin */
#define      LCD_RST_CLK                    RCC_AHB_PERIPHEN_GPIOH  
#define      LCD_RST_PORT                   GPIOH
#define      LCD_RST_PIN                    GPIO_PIN_5
#define      LCD_RST_AF                     GPIO_NO_AF

/* BL pin */
#define      LCD_BL_CLK                     RCC_AHB_PERIPHEN_GPIOF 
#define      LCD_BL_PORT                    GPIOF
#define      LCD_BL_PIN                     GPIO_PIN_7
#define      LCD_BL_AF                      GPIO_NO_AF

/* DATA pin */
#define      LCD_FEMC_D0_CLK                RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_D0_PORT               GPIOD
#define      LCD_FEMC_D0_PIN                GPIO_PIN_14
#define      LCD_FEMC_D0_AF                 GPIO_AF10

#define      LCD_FEMC_D1_CLK                RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_D1_PORT               GPIOD
#define      LCD_FEMC_D1_PIN                GPIO_PIN_15
#define      LCD_FEMC_D1_AF                 GPIO_AF10

#define      LCD_FEMC_D2_CLK                RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_D2_PORT               GPIOD
#define      LCD_FEMC_D2_PIN                GPIO_PIN_0
#define      LCD_FEMC_D2_AF                 GPIO_AF10

#define      LCD_FEMC_D3_CLK                RCC_AHB_PERIPHEN_GPIOD  
#define      LCD_FEMC_D3_PORT               GPIOD
#define      LCD_FEMC_D3_PIN                GPIO_PIN_1
#define      LCD_FEMC_D3_AF                 GPIO_AF10

#define      LCD_FEMC_D4_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D4_PORT               GPIOE
#define      LCD_FEMC_D4_PIN                GPIO_PIN_7
#define      LCD_FEMC_D4_AF                 GPIO_AF10

#define      LCD_FEMC_D5_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D5_PORT               GPIOE
#define      LCD_FEMC_D5_PIN                GPIO_PIN_8
#define      LCD_FEMC_D5_AF                 GPIO_AF10

#define      LCD_FEMC_D6_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D6_PORT               GPIOE
#define      LCD_FEMC_D6_PIN                GPIO_PIN_9
#define      LCD_FEMC_D6_AF                 GPIO_AF10

#define      LCD_FEMC_D7_CLK                RCC_AHB_PERIPHEN_GPIOE  
#define      LCD_FEMC_D7_PORT               GPIOE
#define      LCD_FEMC_D7_PIN                GPIO_PIN_10
#define      LCD_FEMC_D7_AF                 GPIO_AF10

#define      LCD_FEMC_D8_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D8_PORT               GPIOE
#define      LCD_FEMC_D8_PIN                GPIO_PIN_11
#define      LCD_FEMC_D8_AF                 GPIO_AF10

#define      LCD_FEMC_D9_CLK                RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D9_PORT               GPIOE
#define      LCD_FEMC_D9_PIN                GPIO_PIN_12
#define      LCD_FEMC_D9_AF                 GPIO_AF10

#define      LCD_FEMC_D10_CLK               RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D10_PORT              GPIOE
#define      LCD_FEMC_D10_PIN               GPIO_PIN_13
#define      LCD_FEMC_D10_AF                GPIO_AF10

#define      LCD_FEMC_D11_CLK               RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D11_PORT              GPIOE
#define      LCD_FEMC_D11_PIN               GPIO_PIN_14
#define      LCD_FEMC_D11_AF                GPIO_AF10

#define      LCD_FEMC_D12_CLK               RCC_AHB_PERIPHEN_GPIOE   
#define      LCD_FEMC_D12_PORT              GPIOE
#define      LCD_FEMC_D12_PIN               GPIO_PIN_15
#define      LCD_FEMC_D12_AF                GPIO_AF10

#define      LCD_FEMC_D13_CLK               RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_D13_PORT              GPIOD
#define      LCD_FEMC_D13_PIN               GPIO_PIN_8
#define      LCD_FEMC_D13_AF                GPIO_AF10

#define      LCD_FEMC_D14_CLK               RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_D14_PORT              GPIOD
#define      LCD_FEMC_D14_PIN               GPIO_PIN_9
#define      LCD_FEMC_D14_AF                GPIO_AF10

#define      LCD_FEMC_D15_CLK               RCC_AHB_PERIPHEN_GPIOD   
#define      LCD_FEMC_D15_PORT              GPIOD
#define      LCD_FEMC_D15_PIN               GPIO_PIN_10
#define      LCD_FEMC_D15_AF                GPIO_AF10


void LCD_Configuration(void);
void LCD_RCC_Configuration(void);
void LCD_GPIO_Configuration(void);
void LCD_FEMC_Configuration(void);
void LCD_BL_Control(uint16_t state);
void LCD_Reset(void);
void ILI9341_REG_Config(void);
void ILI9806G_REG_Config(void);
void LCD_Write_CMD(uint16_t cmd);
void LCD_Write_Data(uint16_t data);
uint16_t LCD_Read_Data(void);
uint16_t LCD_Get_ID(void);
void LCD_Delay (__IO uint32_t nCount);
void LCD_GramScan(uint8_t ucOption);
void LCD_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
static void LCD_SetCursor(uint16_t usX, uint16_t usY);
static __INLINE void LCD_FillColor(uint32_t ulAmout_Point, uint16_t usColor);
void LCD_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void LCD_SetPointPixel(uint16_t usX, uint16_t usY);
static uint16_t LCD_Read_PixelData(void);
uint16_t LCD_GetPointPixel(uint16_t usX, uint16_t usY);
void LCD_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2);
void LCD_DrawRectangle(uint16_t usX_Start,
                           uint16_t usY_Start,
                           uint16_t usWidth,
                           uint16_t usHeight,
                           uint8_t ucFilled);
void LCD_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled);
void LCD_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar);
void LCD_DispString_EN(uint16_t usX, uint16_t usY, char* pStr);
void LCD_DispString_EN_YDir(uint16_t usX, uint16_t usY, char* pStr);
void LCD_SetFont(sFONT* fonts);
sFONT* LCD_GetFont(void);
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor);
void LCD_GetColors(uint16_t* TextColor, uint16_t* BackColor);
void LCD_SetTextColor(uint16_t Color);
void LCD_SetBackColor(uint16_t Color);
void LCD_ClearLine(uint16_t Line);
void lcd_draw_rgb565(const image_t* image);
void lcd_draw_image(uint16_t x, uint16_t y, const image_t* image);
void lcd_ov5640(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_H__ */

