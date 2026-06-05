/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People¡¯s Republic of China and other applicable jurisdictions worldwide.
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
* damage. Such applications are deemed, ¡°Insecure Usage¡±.
*
*     All Insecure Usage shall be made at user¡¯s risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer¡¯s Insecure Usage.

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
*\*\file delay.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "delay.h"

static uint32_t  fac_us=0; 
static uint32_t  fac_ms=0; 
static uint8_t   max_ms=116;

void delay_init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);  
    fac_us=SystemCoreClock/1000000;                    
    fac_ms=fac_us*1000;                                 
}


void delay_us(uint32_t nus)
{
    uint32_t temp;
    SysTick->LOAD=nus*fac_us;                 
    SysTick->VAL=0x00;                         
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;   
    do
    {
        temp = SysTick->CTRL;
    }
    while((temp&0x01) && !(temp&(1<<16)));    

    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;  
    SysTick->VAL =0X00;                      
}


void delay_ms(uint32_t nms)
{
    uint32_t temp;
    SysTick->LOAD=(uint32_t)nms*fac_ms;      
    SysTick->VAL =0x00;                     
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;   
    do
    {
        temp=SysTick->CTRL;
    }while((temp&0x01)&&!(temp&(1<<16)));     
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;  
    SysTick->VAL =0X00;                      
}

void delay_xms(uint32_t nms)
{
    uint16_t i;
    uint16_t count_1 = nms/max_ms;
    uint16_t count_2 = nms%max_ms;

    if(0 == count_1)
    {
        delay_ms(nms);
    }
    else
    {
        for(i=0;i<count_1;i++)
        {
            delay_ms(max_ms);
        }
        
        if(count_2 != 0)
        {
            delay_ms(count_2);
        }
    }
}
