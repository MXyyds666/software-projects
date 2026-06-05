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

#include "n32h47x_48x_it.h"
#include "log.h"

GPIO_InitType GPIO_InitStructure;
RCC_ClocksType RCC_ClockFreq;

ErrorStatus SetSysClockToHSI(void);
ErrorStatus SetSysClockToHSE(void);
ErrorStatus SetSysClockToPLL(uint32_t PLL_src, uint32_t PLL_freq);
ErrorStatus SetSysClockToSHRTPLL(uint32_t SHRTPLL_source, uint64_t fin, uint64_t fout);
ErrorStatus SetSysClockToUSBHS240M(void);
/**
*\*\name    PrintfClockInfo.
*\*\fun     Printf clock information.
*\*\param   none
*\*\return  none 
**/
void PrintfClockInfo(const char* msg)
{
    log_init(); /* should reinit after sysclk changed */
    log_info("--------------------------------\n");
    log_info("%s:\n", msg);
    RCC_GetClocksFreqValue(&RCC_ClockFreq);
    log_info("SYSCLK: %d\n", RCC_ClockFreq.SysclkFreq);
    log_info("HCLK: %d\n", RCC_ClockFreq.HclkFreq);
    log_info("PCLK1: %d\n", RCC_ClockFreq.Pclk1Freq);
    log_info("PCLK2: %d\n", RCC_ClockFreq.Pclk2Freq);
}

int main(void)
{

    PrintfClockInfo("After reset");
	
/*** Select one of the following configuration methods ***/
#if SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_HSI
  /* Method 1  */	
    if(SetSysClockToHSI() == ERROR)
    {
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("HSI->SYSCLK, 8MHz");
#elif SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_HSE 
  /* Method 2  */		
    if(SetSysClockToHSE() == ERROR)
    {
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("HSE->SYSCLK, 8MHz");
#elif SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_PLL 
	/* Method 3  */	
    if(SetSysClockToPLL(RCC_PLL_SRC_HSE,160000000) == ERROR)
    {
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("HSE->PLL->SYSCLK, 160M");
#elif SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_SHRTPLL 
  /* Method 4  */	
    if(SetSysClockToSHRTPLL(RCC_SHRTPLL_SRC_HSI, HSI_VALUE, (240000000U)) == ERROR)
    {
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("HSI->SHRTPLL->SYSCLK, 240M");
#elif SYSCLK_SOURCE_SELECT == SYSCLK_SOURCE_USBHS240M 		
  /* Method 5  */	
    if(SetSysClockToUSBHS240M() == ERROR)
    {
        log_info("Clock configuration failure!\n");
    }
    PrintfClockInfo("HSE->SHRTPLL->USBHS240M->SYSCLK, 240M");
#endif
    
    /* Output HSE clock on MCO pin */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin             = GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode       = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate  = GPIO_AF13;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    
    /* If the MCO selects PLL, SHRTPLL or USBHS240M, the prescaler can be configured first */
    RCC_ConfigMcoClkPre(RCC_MCO_PLLCLK_DIV10);
    /* Select the corresponding MCO clock source */
    RCC_ConfigMco1(RCC_MCO_PLL_PRES);
    
    while (1);
}

/**
*\*\name    SetSysClockToHSI.
*\*\fun     Selects HSI as System clock source and configure HCLK, PCLK2
*\*\         and PCLK1 prescalers.
*\*\param   none
*\*\return  none 
**/
ErrorStatus SetSysClockToHSI(void)
{
    uint32_t timeout_value = 0xFFFFFFFF; 
    ErrorStatus ClockStatus;
    
    RCC_DeInit();

    RCC_EnableHsi(ENABLE);

    /* Wait till HSI is ready */
    ClockStatus = RCC_WaitHsiStable();

    if (ClockStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufSet(FLASH_PrefetchBuf_EN);

        /* Flash 0 wait state */
        FLASH_SetLatency(FLASH_LATENCY_0);

        /* HCLK = SYSCLK */
        RCC_ConfigHclk(RCC_SYSCLK_DIV1);

        /* PCLK2 = HCLK/2 */
        RCC_ConfigPclk2(RCC_HCLK_DIV2);

        /* PCLK1 = HCLK */
        RCC_ConfigPclk1(RCC_HCLK_DIV1);

        /* Select HSI as system clock source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSI);
           
        /* Wait till HSI is used as system clock source */
        while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_HSI)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* HSI fails  */
        return ERROR;
    }
    return SUCCESS;
}

/**
*\*\name    SetSysClockToHSE.
*\*\fun     Selects HSE as System clock source and configure HCLK, PCLK2
*\*\          and PCLK1 prescalers.
*\*\param   none
*\*\return  none 
**/
ErrorStatus SetSysClockToHSE(void)
{
    uint32_t timeout_value = 0xFFFFFFFF; 
    ErrorStatus ClockStatus;
    
    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    /* Enable HSE */
    RCC_ConfigHse(RCC_HSE_ENABLE);

    /* Wait till HSE is ready */
    ClockStatus = RCC_WaitHseStable();

    if (ClockStatus == SUCCESS)
    {
        /* Enable Prefetch Buffer */
        FLASH_PrefetchBufSet(FLASH_PrefetchBuf_EN);

        /* Flash 0 wait state */
        FLASH_SetLatency(FLASH_LATENCY_0);
  
        /* HCLK = SYSCLK */
        RCC_ConfigHclk(RCC_SYSCLK_DIV1);

        /* PCLK2 = HCLK/2 */
        RCC_ConfigPclk2(RCC_HCLK_DIV2);

        /* PCLK1 = HCLK */
        RCC_ConfigPclk1(RCC_HCLK_DIV1);

        /* Select HSE as system clock source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_HSE);
       
        /* Wait till HSE is used as system clock source */
        while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_HSE)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* HSE fails */
        return ERROR;
    }
    return SUCCESS;
}


/**
*\*\name    SetSysClockToPLL.
*\*\fun     Selects PLL clock as System clock source and configure HCLK, PCLK2
*\*\         and PCLK1 prescalers.  
*\*\param   PLL_src
*\*\         - RCC_PLL_SRC_HSI      
*\*\         - RCC_PLL_SRC_HSE  
**\*\param   PLL_freq
*\*\         - 160000000      
*\*\         - 240000000  
*\*\return  none 
*\*\note    Fin frequency requirement is in the range of 4MHz ~ 50MHz,
*\*\	    Fref frequency requirement is in the range of 4MHz ~ 25MHz,
*\*\	    Fvco frequency requirement is in the range of 64MHz ~ 500MHz. 
**/
ErrorStatus SetSysClockToPLL(uint32_t PLL_src, uint32_t PLL_freq)
{
    uint32_t timeout_value = 0xFFFFFFFF; 
    ErrorStatus ClockStatus;
    uint32_t pllmul;
    uint32_t latency;
    uint32_t pclk1div, pclk2div;
    uint32_t pllpre;

    if ((PLL_src == RCC_PLL_SRC_HSE)&&((HSE_VALUE != 8000000)&&(HSE_VALUE != 16000000)))
    {
        /* HSE_VALUE == 8000000 or 16000000 is needed in this project! */
        while (1);
    }
    
    if ((PLL_src == RCC_PLL_SRC_HSE)&&(HSE_VALUE == 16000000))
    {
        pllpre = RCC_PLL_PRE_2;
    }
    else
    {
        pllpre = RCC_PLL_PRE_1;
    }

    /* RCC system reset(for debug purpose) */
    RCC_DeInit();

    if(PLL_src == RCC_PLL_SRC_HSE)
    {
        /* Enable HSE */
        RCC_ConfigHse(RCC_HSE_ENABLE);

        /* Wait till HSE is ready */
        ClockStatus = RCC_WaitHseStable();
           
    }
    else
    {
        /* Enable HSI */
        RCC_EnableHsi(ENABLE);

        /* Wait till HSI is ready */
        ClockStatus = RCC_WaitHsiStable();
    }
    
    if(ClockStatus != SUCCESS)
    {
        /* clock source fails to start-up */
        return ERROR;
    }

    switch (PLL_freq)
    {
        case 160000000:
            latency  = FLASH_LATENCY_3;
            pllmul   = RCC_PLL_MUL_20;
            pclk1div = RCC_HCLK_DIV1;
            pclk2div = RCC_HCLK_DIV2;
            break;
        case 240000000:
            latency  = FLASH_LATENCY_5;
		    pllmul   =  RCC_PLL_MUL_30;
            pclk1div = RCC_HCLK_DIV2;
            pclk2div = RCC_HCLK_DIV2;
            break;
        default:
            while (1);
    }

    FLASH_SetLatency(latency);

    /* HCLK = SYSCLK */
    RCC_ConfigHclk(RCC_SYSCLK_DIV1);

    /* PCLK2 */
    RCC_ConfigPclk2(pclk2div);

    /* PCLK1 */
    RCC_ConfigPclk1(pclk1div);
    
    if(PLL_src == SYSCLK_SOURCE_HSE)
    {
        RCC_ConfigPll(RCC_PLL_SRC_HSE,pllpre,pllmul, RCC_PLLOUT_DIV_1);
    }
    else
    {
        RCC_ConfigPll(RCC_PLL_SRC_HSI,pllpre,pllmul, RCC_PLLOUT_DIV_1);
    }

    /* Enable PLL */
    RCC_EnablePll(ENABLE);

     /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDF) != SET)
    {
        if ((timeout_value--) == 0)
        {
            return ERROR;
        }
    }
    /* Select PLL as system clock source */
    RCC_ConfigSysclk(RCC_SYSCLK_SRC_PLL);

    /* Wait till PLL is used as system clock source */
    timeout_value = 0xFFFFFFFF;
    while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL)
    {
        if ((timeout_value--) == 0)
        {
            return ERROR;
        }
    }
    return SUCCESS;
}

/**
*\*\name    SetSysClockToSHRTPLL.
*\*\fun     Selects SHRTPLL as System clock source and configure HCLK, PCLK2
*\*\         and PCLK1 prescalers.
*\*\param   SHRTPLL_source(SHRTPLL entry clock source):
*\*\   		  - RCC_SHRTPLL_SRC_HSI     
*\*\   		  - RCC_SHRTPLL_SRC_HSE
*\*\param   fin(SHRTPLL input frequency):
*\*\         1000000 ~ 50000000 (Hz)
*\*\param   fout(SHRTPLL output frequency):
*\*\         75000000 ~ 250000000 (Hz)
*\*\return  none 
**/
ErrorStatus SetSysClockToSHRTPLL(uint32_t SHRTPLL_source, uint64_t fin, uint64_t fout)
{
    uint32_t timeout_value = 0xFFFFFFFF; 
    ErrorStatus ClockStatus;
    
    RCC_DeInit();
    
    if(SHRTPLL_source == RCC_SHRTPLL_SRC_HSE)
    {
        /* Enable HSE */
        RCC_ConfigHse(RCC_HSE_ENABLE);

        /* Wait till HSE is ready */
        ClockStatus = RCC_WaitHseStable();
           
    }
    else
    {
        /* Enable HSI */
        RCC_EnableHsi(ENABLE);

        /* Wait till HSI is ready */
        ClockStatus = RCC_WaitHsiStable();
    }
    
    if(ClockStatus != SUCCESS)
    {
        /* clock source fails to start-up */
        return ERROR;
    }

    /* Enable phase reset to filter waveform burrs */
    RCC_EnableSHRTPLLPHAReset(ENABLE);
    /* Configures the SHRTPLL clock source and multiplication factor */
    if(RCC_ConfigSHRTPll(SHRTPLL_source, fin, fout, ENABLE) == SUCCESS)
    {
        
         /* Wait till SHRTPLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_SHRTPLLRDF) != SET)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
        /* Disable phase reset */
        RCC_EnableSHRTPLLPHAReset(DISABLE);
        
        /* Flash wait state */
        FLASH_SetLatency((uint32_t)((fout - 1) / 40000000));
                   
         /* HCLK = SYSCLK */
        RCC_ConfigHclk(RCC_SYSCLK_DIV1);

        /* PCLK2/PCLK1 max 180M */
        if (fout > 180000000)
        {
            RCC_ConfigPclk2(RCC_HCLK_DIV2);
            RCC_ConfigPclk1(RCC_HCLK_DIV2);
        }
        else
        {
            RCC_ConfigPclk2(RCC_HCLK_DIV2);
            RCC_ConfigPclk1(RCC_HCLK_DIV1);
        }

        /* Select SHRTPLL as system clock source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_SHRTPLL);
        
        /* Enable PLL */
        RCC_EnablePll(ENABLE);
           
        /* Wait till SHRTPLL is used as system clock source */
        timeout_value = 0xFFFFFFFF;
        while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* Disable phase reset */
        RCC_EnableSHRTPLLPHAReset(DISABLE);
        /* SHRTPLL fails  */
        return ERROR;
    }
    return SUCCESS;
}

/**
*\*\name    SetSysClockToUSBHS240M.
*\*\fun     Selects USBHS240M as System clock source and configure HCLK, PCLK2
*\*\          and PCLK1 prescalers.
*\*\param   none
*\*\return  none 
**/
ErrorStatus SetSysClockToUSBHS240M(void)
{
    uint32_t timeout_value = 0xFFFFFFFF; 
    ErrorStatus ClockStatus;
    
    RCC_DeInit();
 
    /* Enable HSE */
    RCC_ConfigHse(RCC_HSE_ENABLE);

    /* Wait till HSE is ready */
    ClockStatus = RCC_WaitHseStable();
              
    if(ClockStatus != SUCCESS)
    {
        /* clock source fails to start-up */
        return ERROR;
    }

    /* Enable phase reset to filter waveform burrs */
    RCC_EnableSHRTPLLPHAReset(ENABLE);
    /* Configures the SHRTPLL clock source and multiplication factor */
    if(RCC_ConfigSHRTPll(RCC_SHRTPLL_SRC_HSE, HSE_VALUE, 160000000, ENABLE) == SUCCESS)
    {
        
         /* Wait till SHRTPLLRDF is used as system clock source */
        while (RCC_GetFlagStatus(RCC_FLAG_SHRTPLLRDF) != SET)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
        /* Disable phase reset */
        RCC_EnableSHRTPLLPHAReset(DISABLE);
        /* Select SHRTPLL clock 10 divisions  = 16M*/
        RCC_ConfigUSBPLLPresClk(RCC_USBPLLCLK_SRC_SHRTPLL,RCC_USBPLLCLK_DIV10);
        
        /* Select the corresponding bandwidth and frequency*/
        RCC_ConfigUSBHSBandwidth(RCC_USBHS_BW_16M);
        RCC_ConfigUSBHSFrequency(RCC_USBHS_FREQ_16_OR_32M);
        
        /* Select SHRTPLL division clock to USBHS PLL source*/
        RCC_ConfigUSBHSClk(RCC_USBHS_CLKSRC_PLLPRES);
        
        /* Reset the USBHS phy clock*/
        RCC_EnableAHBPeriphReset(RCC_AHB_PERIPHRST_USBHSPHY);
        /* Enables the USBHS peripheral clock*/
        RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_USBHS,ENABLE);
        
        
        /* Flash wait state */
        FLASH_SetLatency(FLASH_LATENCY_5);
                   
         /* HCLK = SYSCLK */
        RCC_ConfigHclk(RCC_SYSCLK_DIV1);

        /* PCLK2/PCLK1 = HCLK/2  */
        RCC_ConfigPclk2(RCC_HCLK_DIV2);
        RCC_ConfigPclk1(RCC_HCLK_DIV2);
        
        /* Select USBHS240M as system clock source */
        RCC_ConfigSysclk(RCC_SYSCLK_SRC_USBHS240M);
        
         /* Enable PLL */
        RCC_EnablePll(ENABLE);
           
        /* Wait till USBHS240M is used as system clock source */
        timeout_value = 0xFFFFFFFF;
        while (RCC_GetSysclkSrc() != RCC_CFG_SCLKSTS_PLL)
        {
            if ((timeout_value--) == 0)
            {
                return ERROR;
            }
        }
    }
    else
    {
        /* Disable phase reset */
        RCC_EnableSHRTPLLPHAReset(DISABLE);
        /* SHRTPLL fails  */
        return ERROR;
    }
    return SUCCESS;
   
}





