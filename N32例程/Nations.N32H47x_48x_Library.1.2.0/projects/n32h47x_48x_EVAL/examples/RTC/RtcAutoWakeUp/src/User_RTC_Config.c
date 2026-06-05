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
*\*\file User_RTC_Config.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/
#include <stdio.h>
#include <stdint.h>
#include "log.h"
#include "n32h47x_48x.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_rtc.h"
#include "n32h47x_48x_pwr.h"
#include "n32h47x_48x_exti.h"
#include "n32h47x_48x_dbg.h"
#include "misc.h"
#include "User_RTC_Config.h"
#include "delay.h"


RTC_DateType  RTC_DateStructure;
RTC_DateType  RTC_DateDefault;
RTC_TimeType  RTC_TimeStructure;
RTC_TimeType  RTC_TimeDefault;
RTC_InitType  RTC_InitStructure;
RTC_AlarmType RTC_AlarmStructure;
RTC_AlarmType RTC_AlarmDefault;
uint32_t SynchPrediv, AsynchPrediv;

/**
*\*\name    RTC_AlarmShow.
*\*\fun     Display the current alarm time on the Hyperterminal.
*\*\param   AlarmX
*\*\            - RTC_A_ALARM
*\*\            - RTC_B_ALARM
*\*\return  none
*\*\
**/
void RTC_AlarmShow(uint8_t AlarmX)
{
    /* Get the current Alarm */
    if (AlarmX == 0x01)
        RTC_GetAlarm(RTC_FORMAT_BIN, RTC_A_ALARM, &RTC_AlarmStructure);
    else
        RTC_GetAlarm(RTC_FORMAT_BIN, RTC_B_ALARM, &RTC_AlarmStructure);
    printf("\n\r //=========== Current Alarm Display ==============// \n\r");
    printf("\n\r The current alarm is :  %02u:%02u:%02u \n\r",\
           RTC_AlarmStructure.AlarmTime.Hours,\
           RTC_AlarmStructure.AlarmTime.Minutes,\
           RTC_AlarmStructure.AlarmTime.Seconds);
}

/**
*\*\name    RTC_DateShow.
*\*\fun     Display the current Date on the Hyperterminal.
*\*\param   none
*\*\return  none
*\*\
**/
void RTC_DateShow(void)
{
    /* Get the current Date */
    RTC_GetDate(RTC_FORMAT_BIN, &RTC_DateStructure);
    log_info("\n\r //=========== Current Date Display ==============// \n\r");
    log_info("\n\r The current date (WeekDay-Date-Month-Year) is :  %02u-%02u-%02u-%02u \n\r",\
             RTC_DateStructure.WeekDay,\
             RTC_DateStructure.Date,\
             RTC_DateStructure.Month,\
             RTC_DateStructure.Year);
}

/**
*\*\name    RTC_TimeShow.
*\*\fun     Display the current time on the Hyperterminal.
*\*\param   none
*\*\return  none
*\*\
**/
void RTC_TimeShow(void)
{
    /* Get the current Time and Date */
    RTC_GetTime(RTC_FORMAT_BIN, &RTC_TimeStructure);
    log_info("\n\r //============ Current Time Display ===============// \n\r");
    log_info("\n\r The current time (Hour-Minute-Second) is :  %02u:%02u:%02u \n\r",\
             RTC_TimeStructure.Hours,\
             RTC_TimeStructure.Minutes,\
             RTC_TimeStructure.Seconds);
    /* Unfreeze the RTC DAT Register */
    (void)RTC->DATE;
}

/**
*\*\name    RTC_DateAndTimeDefaultVale.
*\*\fun     RTC initalize default value.
*\*\param   none
*\*\return  none
*\*\
**/
void RTC_DateAndTimeDefaultVale(void)
{ // Date
    RTC_DateDefault.WeekDay = 4;
    RTC_DateDefault.Date    = 16;
    RTC_DateDefault.Month   = 11;
    RTC_DateDefault.Year    = 23;
    // Time
    RTC_TimeDefault.H12     = RTC_AM_H12;
    RTC_TimeDefault.Hours   = 4;
    RTC_TimeDefault.Minutes = 22;
    RTC_TimeDefault.Seconds = 55;
    // Alarm
    RTC_AlarmDefault.AlarmTime.H12     = RTC_AM_H12;
    RTC_AlarmDefault.AlarmTime.Hours   = 4;
    RTC_AlarmDefault.AlarmTime.Minutes = 23;
    RTC_AlarmDefault.AlarmTime.Seconds = 00;
    RTC_AlarmDefault.DateWeekMode      = RTC_ALARM_SEL_WEEKDAY_DATE;
    RTC_AlarmDefault.DateWeekValue     = 16;
    RTC_AlarmDefault.AlarmMask         = RTC_ALARMMASK_ALL;
}

/**
*\*\name    RTC_AlarmRegulate.
*\*\fun     RTC initalize default value.
*\*\param   RTC_Alarm
*\*\            - RTC_A_ALARM
*\*\            - RTC_B_ALARM
*\*\return  ErrorStatus
*\*\            - SUCCESS
*\*\            - ERROR
*\*\
**/
ErrorStatus RTC_AlarmRegulate(uint32_t RTC_Alarm)
{
    unsigned int tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;
    /* Disable the AlarmX */
    RTC_EnableAlarm(RTC_Alarm, DISABLE);
    printf("\n\r //==============Alarm X Settings================// \n\r");
    RTC_AlarmStructure.AlarmTime.H12 = RTC_AlarmDefault.AlarmTime.H12;
    RTC_TimeStructure.H12            = RTC_TimeDefault.H12;
    printf("\n\r Please Set Alarm Hours \n\r");
    tmp_hh = RTC_AlarmDefault.AlarmTime.Hours;
    if (tmp_hh == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_AlarmStructure.AlarmTime.Hours = tmp_hh;
    }
    printf(": %02u\n\r", tmp_hh);
    printf("\n\r Please Set Alarm Minutes \n\r");
    tmp_mm = RTC_AlarmDefault.AlarmTime.Minutes;
    if (tmp_mm == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_AlarmStructure.AlarmTime.Minutes = tmp_mm;
    }
    printf(": %02u\n\r", tmp_mm);
    printf("\n\r Please Set Alarm Seconds \n\r");
    tmp_ss = RTC_AlarmDefault.AlarmTime.Seconds;
    if (tmp_ss == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_AlarmStructure.AlarmTime.Seconds = tmp_ss;
    }
    printf(": %02u\n\r", tmp_ss);
    /* Set the Alarm X */
    RTC_AlarmStructure.DateWeekValue = RTC_AlarmDefault.DateWeekValue;
    RTC_AlarmStructure.DateWeekMode = RTC_AlarmDefault.DateWeekMode;
    RTC_AlarmStructure.AlarmMask = RTC_AlarmDefault.AlarmMask;
    /* Configure the RTC Alarm A register */
    RTC_SetAlarm(RTC_FORMAT_BIN, RTC_Alarm, &RTC_AlarmStructure);
    printf("\n\r>> !! RTC Set Alarm_X success. !! <<\n\r");
    if (RTC_Alarm == RTC_A_ALARM)
    {
        /* Enable the RTC Alarm A Interrupt */
        RTC_ConfigInt(RTC_INT_ALRA, ENABLE);
        RTC_AlarmShow(1);
    }
    else
    {
        /* Enable the RTC Alarm B Interrupt */
        RTC_ConfigInt(RTC_INT_ALRB, ENABLE);
        RTC_AlarmShow(2);
    }
    /* Enable the alarm   */
    RTC_EnableAlarm(RTC_Alarm, ENABLE);
    return SUCCESS;
}

/**
*\*\name    RTC_DateRegulate.
*\*\fun     RTC date regulate with the default value.
*\*\param   none
*\*\return  ErrorStatus
*\*\            - SUCCESS
*\*\            - ERROR
*\*\
**/
ErrorStatus RTC_DateRegulate(void)
{
    unsigned int tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;
    log_info("\n\r //=============Date Settings================// \n\r");
    log_info("\n\r Please Set WeekDay (01-07) \n\r");
    tmp_hh = RTC_DateDefault.WeekDay;
    if (tmp_hh == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_DateStructure.WeekDay = tmp_hh;
    }
    log_info(": %02u\n\r", tmp_hh);
    tmp_hh = 0xFF;
    log_info("\n\r Please Set Date (01-31) \n\r");
    tmp_hh = RTC_DateDefault.Date;
    if (tmp_hh == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_DateStructure.Date = tmp_hh;
    }
    log_info(": %02u\n\r", tmp_hh);
    log_info("\n\r Please Set Month (01-12)\n\r");
    tmp_mm = RTC_DateDefault.Month;
    if (tmp_mm == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_DateStructure.Month = tmp_mm;
    }
    log_info(": %02u\n\r", tmp_mm);
    log_info("\n\r Please Set Year (00-99)\n\r");
    tmp_ss = RTC_DateDefault.Year;
    if (tmp_ss == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_DateStructure.Year = tmp_ss;
    }
    log_info(": %02u\n\r", tmp_ss);
    /* Configure the RTC date register */
    if (RTC_SetDate(RTC_FORMAT_BIN, &RTC_DateStructure) == ERROR)
    {
        log_info("\n\r>> !! RTC Set Date failed. !! <<\n\r");
        return ERROR;
    }
    else
    {
        log_info("\n\r>> !! RTC Set Date success. !! <<\n\r");
        RTC_DateShow();
        return SUCCESS;
    }
}

/**
*\*\name    RTC_TimeRegulate.
*\*\fun     RTC time regulate with the default value.
*\*\param   none
*\*\return  ErrorStatus
*\*\            - SUCCESS
*\*\            - ERROR
*\*\
**/
ErrorStatus RTC_TimeRegulate(void)
{
    unsigned int tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;
    log_info("\n\r //==============Time Settings=================// \n\r");
    RTC_TimeStructure.H12 = RTC_TimeDefault.H12;
    log_info("\n\r Please Set Hours \n\r");
    tmp_hh = RTC_TimeDefault.Hours;
    if (tmp_hh == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_TimeStructure.Hours = tmp_hh;
    }
    log_info(": %02u\n\r", tmp_hh);
    log_info("\n\r Please Set Minutes \n\r");
    tmp_mm = RTC_TimeDefault.Minutes;
    if (tmp_mm == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_TimeStructure.Minutes = tmp_mm;
    }
    log_info(": %02u\n\r", tmp_mm);
    log_info("\n\r Please Set Seconds \n\r");
    tmp_ss = RTC_TimeDefault.Seconds;
    if (tmp_ss == 0xff)
    {
        return ERROR;
    }
    else
    {
        RTC_TimeStructure.Seconds = tmp_ss;
    }
    log_info(": %02u\n\r", tmp_ss);
    /* Configure the RTC time register */
    if (RTC_ConfigTime(RTC_FORMAT_BIN, &RTC_TimeStructure) == ERROR)
    {
        log_info("\n\r>> !! RTC Set Time failed. !! <<\n\r");
        return ERROR;
    }
    else
    {
        log_info("\n\r>> !! RTC Set Time success. !! <<\n\r");
        RTC_TimeShow();
        return SUCCESS;
    }
}

/**
*\*\name    RTC_PrescalerConfig.
*\*\fun     RTC prescaler config.
*\*\param   none
*\*\return  none
*\*\
**/
void RTC_PrescalerConfig(void)
{
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv  = SynchPrediv;
    RTC_InitStructure.RTC_HourFormat   = RTC_24HOUR_FORMAT;
    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
        log_info("\r\n //******* RTC Prescaler Config failed **********// \r\n");
    }
}

/**
*\*\name    RTC_CLKSourceConfig.
*\*\fun     Clk_Src_Type specifies RTC Source Clock Type.
*\*\param   RTC_CLK_SRC_TYPE
*\*\            - RTC_CLK_SRC_TYPE_HSE_DIV128
*\*\            - RTC_CLK_SRC_TYPE_LSE
*\*\            - RTC_CLK_SRC_TYPE_LSI
*\*\param   Is_First_Cfg_RCC specifies Is First Config RCC Module.
*\*\            - true
*\*\            - false
*\*\param   RstBKP specifies Is reset backup domain.
*\*\            - true
*\*\            - false
*\*\return  ErrorStatus
*\*\            - SUCCESS: RTC clock configure success
*\*\            - ERROR: RTC clock configure failed
*\*\
**/
ErrorStatus RTC_CLKSourceConfig(RTC_CLK_SRC_TYPE Clk_Src_Type, bool Is_First_Cfg_RCC, bool RstBKP)
{
    uint8_t lse_ready_count=0;
    uint32_t rev_num;
    ErrorStatus Status=SUCCESS;
    /* Enable the PWR clock */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR | RCC_APB1_PERIPH_BKP, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    /* Allow access to RTC */
    PWR_BackupAccessEnable(ENABLE);
    
    if(RstBKP == true)
    {
        RCC_EnableBackupReset();
    }
    
    /* Disable RTC clock */
    RCC_EnableRtcClk(DISABLE);
    
    if (RTC_CLK_SRC_TYPE_HSE_DIV128 == Clk_Src_Type)
    {
        log_info("\r\n RTC_ClkSrc Is Set HSE/128! \r\n");
        if (true == Is_First_Cfg_RCC)
        {
            /* Enable HSE */
            RCC_EnableLsi(DISABLE);
            RCC_ConfigHse(RCC_HSE_ENABLE);
            while (RCC_WaitHseStable() == ERROR)
            {
            }
            RCC_ConfigRtcClk(RCC_RTCCLK_SRC_HSE_DIV128);
        }
        else
        {
            RCC_EnableLsi(DISABLE);
            RCC_ConfigRtcClk(RCC_RTCCLK_SRC_HSE_DIV128);
            /* Enable HSE */
            RCC_ConfigHse(RCC_HSE_ENABLE);
            while (RCC_WaitHseStable() == ERROR)
            {
            }
        }
        SynchPrediv  = 0x3E7; // 16M/128 = 125KHz
        AsynchPrediv = 0x7C;  // value range: 0-7F
    }
    else if (RTC_CLK_SRC_TYPE_LSE == Clk_Src_Type)
    {
        log_info("\r\n RTC_ClkSrc Is Set LSE! \r\n");
        if (true == Is_First_Cfg_RCC)
        {
            rev_num = DBG_GetRevNum();
            if(rev_num > 0x10 && rev_num != 0xFF)
            {
                /* Disable the LSI */
                RCC_EnableLsi(DISABLE); // LSI is turned off
                
            }
            else
            {
                /* Enable the LSI*/
                RCC_EnableLsi(ENABLE); // LSI is turned on
            }
#if (_TEST_LSE_BYPASS_)
            RCC_ConfigLse(RCC_LSE_BYPASS);
#else
            RCC_ConfigLse(RCC_LSE_ENABLE);
#endif
            lse_ready_count=0;
            /****Waite LSE Ready *****/
            while((RCC_GetFlagStatus(RCC_FLAG_LSERDF) == RESET) && (lse_ready_count<RTC_LSE_TRY_COUNT))
            {
                lse_ready_count++;
                systick_delay_ms(10);
                /****LSE Ready failed or timeout*****/
                if(lse_ready_count>=RTC_LSE_TRY_COUNT)
                {
                    Status = ERROR;
                    log_info("\r\n RTC_ClkSrc Set LSE Failed! \r\n");
                    break;
                }
            }
            RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSE);
        }
        else
        {
            rev_num = DBG_GetRevNum();
            if(rev_num > 0x10 && rev_num != 0xFF)
            {
                /* Disable the LSI */
                RCC_EnableLsi(DISABLE); // LSI is turned off
                
            }
            else
            {
                /* Enable the LSI*/
                RCC_EnableLsi(ENABLE); // LSI is turned on
            }
            RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSE);
#if (_TEST_LSE_BYPASS_)
            RCC_ConfigLse(RCC_LSE_BYPASS);
#else
            RCC_ConfigLse(RCC_LSE_ENABLE);
#endif
            lse_ready_count=0;
            /****Waite LSE Ready *****/
            while((RCC_GetFlagStatus(RCC_FLAG_LSERDF) == RESET) && (lse_ready_count<RTC_LSE_TRY_COUNT))
            {
                lse_ready_count++;
                systick_delay_ms(10);
                /****LSE Ready failed or timeout*****/
                if(lse_ready_count>=RTC_LSE_TRY_COUNT)
                {
                   Status = ERROR;
                   log_info("\r\n RTC_ClkSrc Set LSE Failed! \r\n");
                   break;
                }
            }
        }
        SynchPrediv  = 0xFF; // 32.768KHz
        AsynchPrediv = 0x7F; // value range: 0-7F
    }
    else if (RTC_CLK_SRC_TYPE_LSI == Clk_Src_Type)
    {
        log_info("\r\n RTC_ClkSrc Is Set LSI! \r\n");
        if (true == Is_First_Cfg_RCC)
        {
            /* Enable the LSI OSC */
            RCC_EnableLsi(ENABLE);
            while (RCC_GetFlagStatus(RCC_FLAG_LSIRDF) == RESET)
            {
            }
            RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSI);
        }
        else
        {
            RCC_ConfigRtcClk(RCC_RTCCLK_SRC_LSI);
            /* Enable the LSI OSC */
            RCC_EnableLsi(ENABLE);
            while (RCC_GetFlagStatus(RCC_FLAG_LSIRDF) == RESET)
            {
            }
        }
        SynchPrediv  = 0xFB; // 32KHz
        AsynchPrediv = 0x7F;  // value range: 0-7F
    }
    else
    {
        log_info("\r\n RTC_ClkSrc Value is error! \r\n");
    }
    /* Enable the RTC Clock */
    RCC_EnableRtcClk(ENABLE);
    Status = RTC_WaitForSynchro();
    return Status;
}

/**
*\*\name    EXTI17_RTCAlarm_Configuration.
*\*\fun     RTC alarm config EXTI Interrupt.
*\*\param  Cmd :
*\*\          - ENABLE
*\*\          - DISABLE
*\*\return  none
*\*\
**/
void EXTI17_RTCAlarm_Configuration(FunctionalState Cmd)
{
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    EXTI_ClrITPendBit(EXTI_LINE17);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE17;
    #ifdef __TEST_SEVONPEND_WFE_NVIC_DIS__
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    #else
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    #endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_Init(&NVIC_InitStructure);
    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = Cmd;
    NVIC_Init(&NVIC_InitStructure);
}


/**
*\*\name    EXTI20_RTCWKUP_Configuration.
*\*\fun     Config RTC wake up Interrupt.
*\*\param  Cmd :
*\*\          - ENABLE
*\*\          - DISABLE
*\*\return  none
*\*\
**/
void EXTI20_RTCWKUP_Configuration(FunctionalState Cmd)
{
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    EXTI_ClrITPendBit(EXTI_LINE20);
    EXTI_InitStructure.EXTI_Line = EXTI_LINE20;

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* Enable the RTC WakeUp Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = Cmd;
    NVIC_Init(&NVIC_InitStructure);
}
