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
#include "n32h47x_48x.h"
#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "log.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_rtc.h"
#include "n32h47x_48x_pwr.h"
#include "User_RTCBKP_Config.h"

uint32_t Write_Backup_Datas[BACKUP_REGISTER_SIZE]={0x12345678,0x12345679,0x1234567A,0x1234567B,0x1234567C,\
                                                   0x1234567D,0x1234567E,0x1234567F,0x12345680,0x12345681,\
                                                   0x12345682,0x12345683,0x12345684,0x12345685,0x12345686,\
                                                   0x12345687,0x12345688,0x12345689,0x1234568A,0x1234568B};

uint16_t BKP_Datas[BACKUP_REGISTER_SIZE]={BKP_DAT1, BKP_DAT2, BKP_DAT3, BKP_DAT4, BKP_DAT5, \
                                          BKP_DAT6, BKP_DAT7, BKP_DAT8, BKP_DAT9, BKP_DAT10,\
                                          BKP_DAT11,BKP_DAT12,BKP_DAT13,BKP_DAT14,BKP_DAT15,\
                                          BKP_DAT16,BKP_DAT17,BKP_DAT18,BKP_DAT19,BKP_DAT20};

uint32_t Read_Backup_Datas[BACKUP_REGISTER_SIZE]={0x00};


/**
*\*\name    User_RTCBKP_Init.
*\*\fun     RTC Backup Initialize.
*\*\param   none
*\*\return  none
**/
void User_RTCBKP_Init(void)
{
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessEnable(ENABLE);
    RCC_EnableRtcClk(DISABLE);
    /* Enable the LSE OSC32_IN PC14 
       LSI is turned off here to ensure that only one clock is turned on */
    RCC_EnableLsi(DISABLE);  
    RCC_ConfigLse(RCC_LSE_ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDF) == RESET){}
    RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSE);
    /* Enable the RTC Clock */
    RCC_EnableRtcClk(ENABLE);
    RTC_WaitForSynchro();
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
    uint8_t i =0;
    User_RTCBKP_Init();
    /* Initialize USART */
    log_init();
    log_info("\r\n RTC Init \r\n");
    for(i=0;i<BACKUP_REGISTER_SIZE;i++)
    {
       RTC_BKUPRgWrite(i+1, Write_Backup_Datas[i]);
    }
    for(i=0;i<BACKUP_REGISTER_SIZE;i++)
    {
       Read_Backup_Datas[i]=RTC_BKUPRgRead(i+1);
       printf("\r\n BKP_DAT%u=%x\r\n",i+1,(unsigned int)Read_Backup_Datas[i]);
    }
    while (1);
}

