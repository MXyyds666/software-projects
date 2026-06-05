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
 * @file main.c
 * @author Nations
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
 */
#include "main.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"
#include "misc.h"
#include "log.h"
#include <stdio.h>

uint32_t exti_int = 1;
uint32_t key_cnt = 0;

#if     defined(N32H473) || defined(N32H474) || defined(N32H475)
uint8_t KeyPortSrc[MAX_LED_KEY_CNT]={   GPIOA_PORT_SOURCE,
                                        GPIOA_PORT_SOURCE,
                                        GPIOA_PORT_SOURCE   };

uint8_t KeyPinSrc[MAX_LED_KEY_CNT]={    GPIO_PIN_SOURCE4,
                                        GPIO_PIN_SOURCE5,
                                        GPIO_PIN_SOURCE6   };

char *str[MAX_LED_KEY_CNT]={"PA4","PA5","PA6"};

#else
uint8_t KeyPortSrc[MAX_LED_KEY_CNT]={   GPIOC_PORT_SOURCE,
                                        GPIOA_PORT_SOURCE,
                                        GPIOC_PORT_SOURCE   };

uint8_t KeyPinSrc[MAX_LED_KEY_CNT]={    GPIO_PIN_SOURCE13,
                                        GPIO_PIN_SOURCE15,
                                        GPIO_PIN_SOURCE8   };

char *str[MAX_LED_KEY_CNT]={"PC13","PA15","PC8"};

#endif

GPIO_Module* LedPort[MAX_LED_KEY_CNT]={LED1_PORT, LED2_PORT, LED3_PORT};
uint16_t     LedPin[MAX_LED_KEY_CNT] ={LED1_PIN,  LED2_PIN,  LED3_PIN};


void EXTI0_Init(void);

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

/**
 * @brief  Main program.
 */
int main(void)
{
    log_init();
    Delay(SystemCoreClock/2);
    printf("\r\nEXTI key interrupt demo!\r\n");

    /* LED port init */
    LED_Init(LED1_PORT, LED1_PIN, LED1_CLOCK);
    LED_Init(LED2_PORT, LED2_PIN, LED2_CLOCK);
    LED_Init(LED3_PORT, LED3_PIN, LED3_CLOCK);

    KEY_Init(KEY1_PORT, KEY1_PIN, KEY1_CLOCK);
    KEY_Init(KEY2_PORT, KEY2_PIN, KEY2_CLOCK);
    KEY_Init(KEY3_PORT, KEY3_PIN, KEY3_CLOCK);
    
    EXTI0_Init();

    /* LED3 port is default used for JTAG, must remap first */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
    GPIO_ConfigPinRemap(0,0,GPIO_RMP_SWJ_SWD);
    
    /* Turn off all LED */
    LED_Off(LED1_PORT, LED1_PIN);
    LED_Off(LED2_PORT, LED2_PIN);
    LED_Off(LED3_PORT, LED3_PIN);

    while (1)
    {
        if(exti_int != 0)
        {
            printf("Current EXTI0 input source is:%s\r\n",str[key_cnt]);
            exti_int = 0;
        }

        LED_Blink(LedPort[key_cnt], LedPin[key_cnt]);
        Delay(SystemCoreClock/8);
    }
}

/**
 *\*\name   LED_Init.
 *\*\fun    Initialize a GPIO as LED port.
 *\*\param  none
 *\*\return none
 */
void LED_Init(GPIO_Module* GPIOx,uint16_t Pin, uint32_t clock)
{
    GPIO_InitType InitStruct;
    
    RCC_EnableAHB1PeriphClk(clock,ENABLE);
    
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
 *\*\fun    Turn on LED.
 *\*\param  none
 *\*\return none
 */
void LED_On(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_SetBits(GPIOx,Pin);
}

/**
 *\*\name   LED_Off.
 *\*\fun    Turn off LED.
 *\*\param  none
 *\*\return none
 */
void LED_Off(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_ResetBits(GPIOx,Pin);
}

/**
 *\*\name   LED_Blink.
 *\*\fun    Blink LED.
 *\*\param  none
 *\*\return none
 */
void LED_Blink(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_TogglePin(GPIOx,Pin);
}

/**
 *\*\name   LED_Init.
 *\*\fun    Initialize a GPIO as KEY port.
 *\*\param  none
 *\*\return none
 */
void KEY_Init(GPIO_Module* GPIOx,uint16_t Pin, uint32_t clock)
{
    GPIO_InitType InitStruct;
    
    RCC_EnableAHB1PeriphClk(clock,ENABLE);
    
    InitStruct.Pin            = Pin;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_NO_AF;
    InitStruct.GPIO_Pull      = GPIO_PULL_UP;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(GPIOx, &InitStruct);
}

/**
 *\*\name   EX.
 *\*\fun    Blink LED.
 *\*\param  none
 *\*\return none
 */
void EXTI0_Init(void)
{
    EXTI_InitType EXTI_Struct;
    NVIC_InitType NVIC_Struct;

    EXTI_Struct.EXTI_Line    = EXTI_LINE0;
    EXTI_Struct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Struct.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_Struct);

    NVIC_Struct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);

    GPIO_ConfigEXTILine(EXTI_LINE_SOURCE0,KeyPortSrc[0],KeyPinSrc[0]);
}

