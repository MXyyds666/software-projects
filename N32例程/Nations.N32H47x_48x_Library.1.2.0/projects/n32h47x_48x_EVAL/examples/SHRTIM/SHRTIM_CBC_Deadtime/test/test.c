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
*\*\file test.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "test.h"
static void TestMspInit(void)
{
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);

    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    /* Configure SHRTIM output: TA1 (PA8) */
    GPIO_InitStructure.Pin = GPIO_PIN_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF6;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

void TestEntry(void)
{
    TestMspInit();
    
    CBCTest();
}

/* Connect TA1 to EXEV5 and EXEV8 to test. Default is level CBC,
   if you want to test edge CBC， software change EXEV5SRC bit field to 
   "0b'1 in debug(means that EXEV5's source is not from GPIO, means that disconnect the 
    GPIO and EXEV5 by software). 
    Since EXEV5 is level sensitive, EXEV8 is edge sensitive, 
    if EXEV5 is disconnected, the CBC is edge CBC 
   Note:  
    For testing convenience, the prescaler values of TIMA and TIMD were set to different values
    The master timer and TIMA..F timers can only use different prescaler ratios if the counter and output behavior does not depend on the information and signals from other timers.
*/
void CBCTest(void)
{
    /* Disble TIMA and TIMD, and clear the counter of TIMA and TIMD,
       then start the TIMA and TIMD at same time to make TIMA and TIMD synchronous*/
    SHRTIM_TIM_CounterDisable(SHRTIM1, SHRTIM_TIMER_A | SHRTIM_TIMER_D);
    SHRTIM1->sTimerxRegs[3].TxCNT =  (uint32_t)0;
    SHRTIM1->sTimerxRegs[0].TxCNT =  (uint32_t)0;
    
    /* Continuous mode, CK-PSC = 7, 
       as a result, when the TxPRD value is the same, 
       the period length of TIMA is 128 times that of TIMD. */
    SHRTIM1->sTimerxRegs[0].TxCTRL =  (uint32_t)0x0000000F;

    SHRTIM1->sTimerxRegs[0].TxPRD = TIMD_PERIOD;
    
    /* Create a long level to EXEV */
    SHRTIM1->sTimerxRegs[0].TxCMP1DAT = DEADTIME + 0x20;
    SHRTIM1->sTimerxRegs[0].TxCMP2DAT = TIMD_PERIOD/2 + 0x20;

    /* CMP1 set */
    SHRTIM1->sTimerxRegs[0].TxSET1 = (uint32_t)0x00000008;
    /* CMP2 reset */
    SHRTIM1->sTimerxRegs[0].TxRST1 = (uint32_t)0x00000010; 

    SHRTIM_EnableOutput(SHRTIM1, SHRTIM_OEN_TA1OEN);

    SHRTIM_TIM_CounterEnable(SHRTIM1, SHRTIM_TIMER_A | SHRTIM_TIMER_D);
}   


