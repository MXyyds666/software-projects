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
*\*\file hw_config.h
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#ifndef __HW_CONFIG_H__
#define __HW_CONFIG_H__

#include "n32h47x_48x.h"
#include "usbfsd_type.h"

//#define USB_LOW_PWR_MGMT_SUPPORT

/* Exported define -----------------------------------------------------------*/
#define SYSCLK_VALUE_48MHz            ((uint32_t)48000000)
#define SYSCLK_VALUE_72MHz            ((uint32_t)72000000)
#define SYSCLK_VALUE_96MHz            ((uint32_t)96000000)
#define SYSCLK_VALUE_108MHz           ((uint32_t)108000000)
#define SYSCLK_VALUE_144MHz           ((uint32_t)144000000)
#define SYSCLK_VALUE_192MHz           ((uint32_t)192000000)
#define SYSCLK_VALUE_240MHz           ((uint32_t)240000000)

#define CURSOR_STEP     20

#define DOWN            2
#define LEFT            3
#define RIGHT           4
#define UP              5

ErrorStatus Set_USBClock(uint32_t sysclk);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nCount);
void Cfg_KeyIO(void);
ErrorStatus USB_Config(uint32_t sysclk);
uint8_t JoyState(void);
void Joystick_Send(uint8_t Keys);
void Cfg_KeyIO(void);
void USBFS_IO_Configure(void);
ErrorStatus USB_Config(uint32_t sysclk);

typedef enum 
{
    BUTTON_WAKEUP = 0,
    BUTTON_RIGHT = 1,
    BUTTON_LEFT = 2,
    BUTTON_UP = 3,
    BUTTON_DOWN = 4,
    BUTTON_SEL = 5
} Button_TypeDef;


typedef enum 
{ 
    JOY_NONE = 0,
    JOY_SEL = 1,
    JOY_DOWN = 2,
    JOY_LEFT = 3,
    JOY_RIGHT = 4,
    JOY_UP = 5
} JOYState_TypeDef;

/* Button Defines Legacy */ 
#define Button_RIGHT         BUTTON_RIGHT
#define Button_LEFT          BUTTON_LEFT
#define Button_UP            BUTTON_UP
#define Button_DOWN          BUTTON_DOWN
#define Button_SEL           BUTTON_SEL
#define Mode_GPIO            BUTTON_MODE_GPIO
#define Mode_EXTI            BUTTON_MODE_EXTI
#define Button_Mode_TypeDef  ButtonMode_TypeDef
#define JOY_CENTER           JOY_SEL
#define JOY_State_TypeDef    JOYState_TypeDef
#define Button_PORT          GPIOA

#define BUTTONn                          6

/**
 * @brief Wakeup push-button
 */
#define WAKEUP_BUTTON_PIN                GPIO_PIN_0
#define WAKEUP_BUTTON_GPIO_PORT          GPIOA
#define WAKEUP_BUTTON_GPIO_CLK           RCC_AHB_PERIPHEN_GPIOA

/**
 * @brief Joystick Right push-button
 */
#define RIGHT_BUTTON_PIN                 GPIO_PIN_15
#define RIGHT_BUTTON_GPIO_PORT           GPIOA
#define RIGHT_BUTTON_GPIO_CLK            RCC_AHB_PERIPHEN_GPIOA
/**
 * @brief Joystick Left push-button
 */
#define LEFT_BUTTON_PIN                  GPIO_PIN_13
#define LEFT_BUTTON_GPIO_PORT            GPIOC
#define LEFT_BUTTON_GPIO_CLK             RCC_AHB_PERIPHEN_GPIOC
/**
 * @brief Joystick Up push-button
 */
#define UP_BUTTON_PIN                    GPIO_PIN_8
#define UP_BUTTON_GPIO_PORT              GPIOC
#define UP_BUTTON_GPIO_CLK               RCC_AHB_PERIPHEN_GPIOC
/**
 * @brief Joystick Down push-button
 */  
#define DOWN_BUTTON_PIN                  GPIO_PIN_0
#define DOWN_BUTTON_GPIO_PORT            GPIOA
#define DOWN_BUTTON_GPIO_CLK             RCC_AHB_PERIPHEN_GPIOA
/**
 * @brief Joystick Sel push-button
 */
#define SEL_BUTTON_PIN                   GPIO_PIN_12
#define SEL_BUTTON_GPIO_PORT             GPIOD
#define SEL_BUTTON_GPIO_CLK              RCC_AHB_PERIPHEN_GPIOD

#endif /*__HW_CONFIG_H__*/
