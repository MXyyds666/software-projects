/**
*     Copyright (c) 2022, Nations Technologies Inc.
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
 */
#include "main.h"
#include "log.h"
#include "delay.h"
#include "string.h"


typedef enum{
    Triming_Status_Start        = 0,
    Triming_CH1_Step_1          = 1,
    Triming_CH1_Step_2          = 2,
    Triming_CH2_Step_1          = 3,
    Triming_CH2_Step_2          = 4,
    Triming_Status_Completed    = 5
}Triming_Status;

typedef struct{
    uint8_t Triming_Select;     /*0 CH1 ; 1 ;CH2*/
    uint8_t Triming_CH1Data_P;
    uint8_t Triming_CH1Data_N;
    uint8_t Triming_CH2Data_P;
    uint8_t Triming_CH2Data_N;
    Triming_Status  Triming_CurrentStatus; 
}sTriming_Data;
sTriming_Data Triming_Data = {0,0,0,0,0,Triming_Status_Start};


USART_InitType USART_InitStructure;

uint8_t RxBuffer[10] = {0};

void RCC_Configuration(void);
void GPIO_Configuration(void);
void PGA_TrimConfiguration(void);
void PGA_TrimingProcessHandler(void);
void PGA_TrimNValueSet(PGAX PGAx, PGA_CHANNEL ch, uint8_t NTrimValue);
void PGA_TrimPValueSet(PGAX PGAx, PGA_CHANNEL ch, uint8_t PTrimValue);
void NVIC_Configuration(void);


void USART_Init_User(void)
{
    /* USARTy configuration */
		USART_StructInit(&USART_InitStructure);
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.OverSampling        = USART_16OVER;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    /* Configure USARTy  */
    USART_Init(USARTy, &USART_InitStructure);
    
    /* Enable USARTy Receive and Transmit interrupts */
    USART_ConfigInt(USARTy, USART_INT_RXDNE ,ENABLE);
    USART_ConfigInt(USARTy,  USART_INT_IDLEF,ENABLE);
    /* Enable the USARTy */
    USART_Enable(USARTy, ENABLE);
}
/*
*\*\name    main.
*\*\fun     main program.
*\*\return  none
**/
int main(void)
{
    /* System clocks configuration ---------------------------------------------*/
    RCC_Configuration();
    
    /*log init*/
    log_init();

    /* GPIO configuration ------------------------------------------------------*/
    GPIO_Configuration();
    
    NVIC_Configuration();
    USART_Init_User();
    printf(" PGA Triming Demo Begain，send AA 01 for starting!\r\n");
    while (1)
    {
        if((RxBuffer[0] == 0xAA)&&(RxBuffer[1] == 0x01)) 
        {
            memset(RxBuffer,0,10) ;
            PGA_TrimingProcessHandler();
        }
    }
}

/**
*\*\name    PGA_TrimConfiguration.
*\*\fun     Configures the PGA module.
*\*\return  none
**/
void PGA_TrimConfiguration(void)
{
    PGA_InitType PGA_Initial;
    PGA_EnableIBIAS(ENABLE);
    /*Initial comp*/
    PGA_StructInit(&PGA_Initial);
    PGA_Initial.DiffModeEn  = DISABLE;
    PGA_Initial.Gain1       = PGA_CTRL_CH1GAIN_2_DIFF_4;
    PGA_Initial.TimeAutoMuxEn  = DISABLE ;
    PGA_Initial.En1         = DISABLE;
    PGA_Initial.En2         = DISABLE;
    if(Triming_Data.Triming_Select == 0)
    {
        PGA_Initial.Vpsel       = PGA4_CTRL_VPSEL_PB11; 
        PGA_Init(PGA4, &PGA_Initial);
        //*(uint32_t *)PGA4_TRIM_EN  |= ((uint32_t)0x01 << 31);
        /*enable PGA4 CH1 Trim */
        PGA_TrimEnable(PGA4, ENABLE);
        /*enable PGA4 CH1 */
        PGA_Enable(PGA4,PGA_Channel1, ENABLE);
        Triming_Data.Triming_Select = 1;
        Triming_Data.Triming_CurrentStatus = Triming_CH1_Step_1;
        log_debug("Enter Triming_CH1_Step_1, Please ensure VINP is connected to (0.3)v, send AA 01 for confirming!\r\n");
    }
    else
    {
        PGA_Enable(PGA4,PGA_Channel1, DISABLE);
        PGA_Initial.Vmsel       = PGA4_CTRL_VMSEL_PB1; 
        PGA_Init(PGA4, &PGA_Initial);
        //*(uint32_t *)PGA4_TRIM_EN  |= ((uint32_t)0x01 << 31);
        /*enable PGA4 CH1 Trim */
        PGA_TrimEnable(PGA4, ENABLE);
        /*enable PGA4 CH2 */
        PGA_Enable(PGA4,PGA_Channel2, ENABLE);
        Triming_Data.Triming_Select = 0;
        Triming_Data.Triming_CurrentStatus = Triming_CH2_Step_1;
    }
    
}
void PGA_TrimPValueSet(PGAX PGAx, PGA_CHANNEL ch, uint8_t PTrimValue)
{
    PGA_SingleType* pCs = &PGA->Pga[PGAx];
    __IO uint32_t temp  = pCs->TRIM;

    if(ch == PGA_Channel1)
    {
        temp &= (~(PGA_TRIM_TRIMCH1P_MASK ));
        temp |= (uint32_t)PTrimValue   ;
    }
    else
    {
        temp &= (~(PGA_TRIM_TRIMCH2P_MASK  ));
        temp |= ( ( (uint32_t)PTrimValue)<< 12 );
    }
    pCs->TRIM = temp;
}

void PGA_TrimNValueSet(PGAX PGAx, PGA_CHANNEL ch, uint8_t NTrimValue)
{
    PGA_SingleType* pCs = &PGA->Pga[PGAx];
    __IO uint32_t temp  = pCs->TRIM;

    if(ch == PGA_Channel1)
    {
        temp &= (~(PGA_TRIM_TRIMCH1N_MASK));
        temp |= ( ((uint32_t)NTrimValue)<<6 );
    }
    else
    {
        temp &= (~ PGA_TRIM_TRIMCH2N_MASK);
        temp |= (   ((uint32_t)NTrimValue)<< 18 );
    }
    pCs->TRIM = temp;
}
void PGA_TrimingProcessHandler(void)
{
    uint32_t i = 0;
    FlagStatus flag_status = RESET;
    switch(Triming_Data.Triming_CurrentStatus)
    {
        case Triming_Status_Start:
        {
            PGA_TrimConfiguration();
            
            break;
        }
        case Triming_CH1_Step_1:
        {
            PGA_TrimPValueSet(PGA4,PGA_Channel1,0);  
            systick_delay_us(120);
            flag_status = PGA_TrimStatusGet(PGA4,PGA_Channel1);
            for(i = 0;i< (0x3F); i++)
            {
                PGA_TrimPValueSet(PGA4,PGA_Channel1,i+1);  
                systick_delay_us(120);
                if(PGA_TrimStatusGet(PGA4,PGA_Channel1) != flag_status )
                {
                    Triming_Data.Triming_CH1Data_P = i+1;
                    break;
                }
            }
            log_debug("Enter Triming_CH1_Step_2, Please ensure VINP is connected to (VDDA-0.3)v,send AA 01 for confirming!\r\n");
            Triming_Data.Triming_CurrentStatus = Triming_CH1_Step_2;
            break;
        }
        case Triming_CH1_Step_2:
        {
            PGA_TrimNValueSet(PGA4,PGA_Channel1,0);  
            systick_delay_us(120);
            flag_status = PGA_TrimStatusGet(PGA4,PGA_Channel1);
            for(i = 0;i< (0x3F); i++)
            {
                PGA_TrimNValueSet(PGA4,PGA_Channel1,i+1);  
                systick_delay_us(120);
                if(PGA_TrimStatusGet(PGA4,PGA_Channel1) != flag_status )
                {
                    Triming_Data.Triming_CH1Data_N = (i+1);
                    break;
                }
            }
            log_debug("Enter Triming_CH2_Step_1, Please ensure VINM is connected to (0.3)v, send AA 01 for confirming!\r\n");
            Triming_Data.Triming_CurrentStatus = Triming_CH2_Step_1;
            break;
        }
        case Triming_CH2_Step_1:
        {
            PGA_TrimConfiguration();
            PGA_TrimPValueSet(PGA4,PGA_Channel2,0);  
            systick_delay_us(120);
            flag_status = PGA_TrimStatusGet(PGA4,PGA_Channel2);
            for(i = 0;i< (0x3F); i++)
            {
                PGA_TrimPValueSet(PGA4,PGA_Channel2,i+1);  
                systick_delay_us(120);
                if(PGA_TrimStatusGet(PGA4,PGA_Channel2) != flag_status )
                {
                    Triming_Data.Triming_CH2Data_P = i+1;
                    break;
                }
            }
            log_debug("Enter Triming_CH2_Step_2, Please ensure VINM is connected to (VDDA-0.3)v,send AA 01 for confirming!\r\n");
            Triming_Data.Triming_CurrentStatus = Triming_CH2_Step_2;
            break;
        }
        case Triming_CH2_Step_2:
        {
            PGA_TrimNValueSet(PGA4,PGA_Channel2,0);  
            systick_delay_us(120);
            flag_status = PGA_TrimStatusGet(PGA4,PGA_Channel2);
            for(i = 0;i< (0x3F); i++)
            {
                PGA_TrimNValueSet(PGA4,PGA_Channel2,i+1);  
                systick_delay_us(120);
                if(PGA_TrimStatusGet(PGA4,PGA_Channel2) != flag_status )
                {
                    Triming_Data.Triming_CH2Data_N = (i+1);
                    break;
                }
            }
            log_debug("Enter Triming complete, send AA 01 for saving data! \r\n");
            Triming_Data.Triming_CurrentStatus = Triming_Status_Completed;
            break;
        }
        case Triming_Status_Completed:
        {
            log_debug("Calibration over, Result= %x,%x,%x,%x\r\n",Triming_Data.Triming_CH1Data_P,Triming_Data.Triming_CH1Data_N,Triming_Data.Triming_CH2Data_P,Triming_Data.Triming_CH1Data_N);
            /* Save triming data*/
            break;
        }
        default:
            break;
    
    }
}
/**
*\*\name    RCC_Configuration.
*\*\fun     Configures the different system clocks.
*\*\return  none
**/
void RCC_Configuration(void)
{
    /* Enable GPIOA, GPIOB, GPIOC clocks */
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA | RCC_AHB_PERIPHEN_GPIOB | RCC_AHB_PERIPHEN_GPIOC | RCC_AHB_PERIPHEN_GPIOD, ENABLE);
    /* Enable PGA clocks */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_PGA ,ENABLE);
    
    RCC_EnableAPB1PeriphClk(USARTy_CLK,ENABLE);
}

/**
*\*\name    GPIO_Configuration.
*\*\fun     Configures the different GPIO ports.
*\*\return  none
**/
void GPIO_Configuration(void)
{
    GPIO_InitType GPIO_InitStructure;
    // INP INM
    GPIO_InitStruct(&GPIO_InitStructure);
    GPIO_InitStructure.Pin        = GPIO_PIN_11 | GPIO_PIN_1  ;
    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_ANALOG;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
    
    /* Configure USARTy Rx as alternate function input and pull-up */
    GPIO_InitStructure.Pin            = USARTy_RxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = USARTy_Rx_GPIO_AF;
    GPIO_InitPeripheral(USARTy_GPIO, &GPIO_InitStructure);

    /* Configure USARTy Tx as alternate function push-pull */
    GPIO_InitStructure.Pin            = USARTy_TxPin;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStructure.GPIO_Alternate = USARTy_Tx_GPIO_AF;
    GPIO_InitPeripheral(USARTy_GPIO, &GPIO_InitStructure);
}
/**
*\*\name    NVIC_Configuration.
*\*\fun     Configures NVIC and Vector Table base location.
*\*\return  none
**/
void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = USARTy_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
