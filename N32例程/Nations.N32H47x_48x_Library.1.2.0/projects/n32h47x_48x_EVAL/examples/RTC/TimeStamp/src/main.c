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
#include <stdio.h>
#include <stdint.h>
#include "n32h47x_48x.h"
#include "log.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_rtc.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"
#include "misc.h"
#include "User_RTC_Config.h"
#include "delay.h"

#define USER_WRITE_BKP_DAT1_DATA        0xA5A5


/**
*\*\name    EXTI_PB8_TimeStamp_Configuration.
*\*\fun     EXTI PD8 I/O config and use the EXTI interrupt to trigger time stamp.
*\*\param   none
*\*\return  none
**/
void EXTI_PB8_TimeStamp_Configuration()
{
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB , ENABLE);
    /* Configure PB15 in alternate function mode */
    GPIO_InitStructure.Pin = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
    /* Connect PB15 to RTC_AF1 */
    EXTI_RTCTimeStampSel(EXTI_TSSEL_LINE8);
    /* Connect EXTI8 Line to PB15 pin */
    GPIO_ConfigEXTILine(EXTI_LINE_SOURCE8, GPIOB_PORT_SOURCE, GPIO_PIN_SOURCE8);
}

/**
*\*\name    EXTI19_TAMPER_IRQn_Configuration.
*\*\fun     Initialize Tamper NVIC and EXIT LINE.
*\*\param   none
*\*\return  none
**/
void EXTI19_TAMPER_IRQn_Configuration(FunctionalState NewState,uint32_t TSEdgeSel)
{
    //GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    EXTI_ClrITPendBit(EXTI_LINE19);
    EXTI_InitStructure.EXTI_Line                          = EXTI_LINE19;
    EXTI_InitStructure.EXTI_Mode                          = EXTI_Mode_Interrupt;
    if(TSEdgeSel == 0x01)
        EXTI_InitStructure.EXTI_Trigger   = EXTI_Trigger_Rising;  
    else if(TSEdgeSel == 0x02)
        EXTI_InitStructure.EXTI_Trigger   = EXTI_Trigger_Falling;
    else
        printf("\r\n The TSEdgeSel value is error! \r\n");
    EXTI_InitStructure.EXTI_LineCmd                       = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                    = RTC_TAMPER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                 = NewState;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    main.
*\*\fun     Main program.
*\*\
**/
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_n32h47x_48x_xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_n32h47x_48x.c file
       */
    /* Initialize USART */
    log_init();
    log_info("\r\n RTC Init \r\n");
    /* RTC date and time default value*/
    RTC_DateAndTimeDefaultVale();
    /* Enable the PWR clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessEnable(ENABLE);
    /* RTC clock source select */
    if(SUCCESS==RTC_CLKSourceConfig(RTC_CLK_SRC_TYPE_LSE, true, false))
    {
        if (USER_WRITE_BKP_DAT1_DATA != RTC_BKUPRgRead(1))
        {
           RTC_PrescalerConfig();
           /* Adjust time by values entered by the user on the hyperterminal */
           RTC_DateRegulate();
           RTC_TimeRegulate();
           RTC_BKUPRgWrite(1, USER_WRITE_BKP_DAT1_DATA);
           log_info("\r\n RTC Init Success\r\n");
        }
    }
    else
    {
        log_info("\r\n RTC Init Failed\r\n");
    }
    /* Adjust time by values entered by the user on the hyperterminal */
    RTC_ConfigCalibOutput(RTC_CALIB_OUTPUT_1HZ);
    /* Calibrate output config,push pull */
    RTC_ConfigOutputType(RTC_OUTPUT_PUSHPULL);
    /* Calibrate output enable*/
    RTC_EnableCalibOutput(ENABLE);
    /* Configure EXTI PD8 pin  connected to RTC TimeStamp
    (while externally feeding PD8 with 1HZ signal output from PC13)
    */
    EXTI_PB8_TimeStamp_Configuration();
    EXTI_ClrITPendBit(EXTI_LINE19);
    EXTI19_TAMPER_IRQn_Configuration(ENABLE,1);
    /* clear RTC time stamp flag  */
    RTC_ClrFlag(RTC_FLAG_TISF);
    RTC_ClrFlag(RTC_FLAG_TISOVF);
    RTC_EnableTimeStamp(RTC_TIMESTAMP_EDGE_FALLING, ENABLE);
    RTC_ConfigInt(RTC_INT_TS,ENABLE);
    while (1)
    {
    }
}


