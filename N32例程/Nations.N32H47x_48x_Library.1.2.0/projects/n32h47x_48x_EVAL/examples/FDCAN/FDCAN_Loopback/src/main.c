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
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_fdcan.h"
#include "log.h"


#define MAX_KEY_CNT     (3000)  /*About 67ms*/

#define TEST_DATA_FRAME_SIZE    (20)   /* Data frame size,must not be greater than 64 */
#define MSG_RAM_SIZE            (0x100) /* Message ram size,must not be greater than 4480 */

uint8_t TxData0[TEST_DATA_FRAME_SIZE] = {   0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89, 0x9A, 
                                            0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0, 0x01, 0x12, 0x23, 0x34  };
uint8_t TxData1[TEST_DATA_FRAME_SIZE] = {   0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA,
                                            0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA  };
uint8_t TxData2[TEST_DATA_FRAME_SIZE] = {   0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 
                                            0x55, 0x44, 0x33, 0x22, 0x11, 0x00, 0xFF, 0xEE, 0xDD, 0xCC  };

FDCAN_RxHeaderType RxHeader;
FDCAN_TxEventFifoType TxEvent;
                                    
uint8_t RxBuf[64];
uint32_t FDCAN_ram[MSG_RAM_SIZE];  /* Used for FDCAN message ram, shared by all FDCAN modules */
FDCAN_MsgRamType Node1_msg;

/**
 *\*\name   main.
 *\*\fun    Main program.
 *\*\param  none
 *\*\return none
 */
int main(void)
{
    static uint32_t key_cnt = 0;
    
    log_init();
    Delay(SystemCoreClock/2);

    printf("\r\n FDCAN loopback demo!\r\n");

    /* LED port init */
    LED_Init(LED1_PORT,LED1_PIN,LED1_CLOCK);
    LED_Init(LED2_PORT,LED2_PIN,LED2_CLOCK);
    LED_Init(LED3_PORT,LED3_PIN,LED3_CLOCK);
    
    LED_Off(LED1_PORT,LED1_PIN);
    LED_Off(LED2_PORT,LED2_PIN);
    LED_Off(LED3_PORT,LED3_PIN);
    
    /* PB4 default used of JTAG port, must be remap as GPIO */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
    GPIO_ConfigPinRemap(0,0,GPIO_RMP_SWJ_SWD);
    
    /* KEY port init */
    Key_Init(KEY_PORT, KEY_PIN, KEY_CLOCK);
    
    FDCAN_clock_src_config();

    Node1_port_config();
    Node1_Config();
    Delay(SystemCoreClock/8);

    /* LED turn on one by one and repeat */
    while (1)
    {
        if(KEY_PORT->PID & KEY_PIN)
        {
            key_cnt++;
            if(key_cnt >= (SystemCoreClock/MAX_KEY_CNT))
            {
                key_cnt = 0;
                
                LED_On(LED1_PORT,LED1_PIN);
                Node1_SendMsg();
                LED_Off(LED1_PORT,LED1_PIN);
            }
        }
        else
        {
            key_cnt = 0;
        }

        if(FDCAN_CheckNewRxBufMsg(NODE1,FDCAN_RX_BUFFER0) == SET)
        {
            LED_On(LED2_PORT,LED2_PIN);
            FDCAN_GetRxMsg(NODE1,FDCAN_RX_BUFFER0,&RxHeader,RxBuf);
            log_info("\r\n Rx buffer0 recieved,RXTS=0x%08x:\r\n",RxHeader.RxTimestamp);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[0], RxBuf[1], RxBuf[2], RxBuf[3]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[4], RxBuf[5], RxBuf[6], RxBuf[7]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[8], RxBuf[9], RxBuf[10],RxBuf[11]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[12],RxBuf[13],RxBuf[14],RxBuf[15]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[16],RxBuf[17],RxBuf[18],RxBuf[19]);
            LED_Off(LED2_PORT,LED2_PIN);
        }
        

        if(FDCAN_GetRxFifoFillLevel(NODE1,FDCAN_RX_FIFO1) > 0)
        {
            LED_On(LED3_PORT,LED3_PIN);
            FDCAN_GetRxMsg(NODE1,FDCAN_RX_FIFO1,&RxHeader,RxBuf);
            log_info("\r\n Rx FIFO1 recieved,RXTS=0x%08x:\r\n",RxHeader.RxTimestamp);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[0], RxBuf[1], RxBuf[2], RxBuf[3]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[4], RxBuf[5], RxBuf[6], RxBuf[7]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[8], RxBuf[9], RxBuf[10],RxBuf[11]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[12],RxBuf[13],RxBuf[14],RxBuf[15]);
            log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[16],RxBuf[17],RxBuf[18],RxBuf[19]);
            LED_Off(LED3_PORT,LED3_PIN);
        }


        if(FDCAN_GetTxEvent(NODE1,&TxEvent) == SUCCESS)
        {
            log_info("\r\n Get Tx event:\r\n");
            log_info("\t ESI:0x%08x\r\n",TxEvent.ErrorState);
            log_info("\t XTD:0x%08x\r\n",TxEvent.IdType);
            log_info("\t RTR:0x%08x\r\n",TxEvent.TxFrameType);
            log_info("\t ID:0x%08x\r\n",TxEvent.ID);
            log_info("\t MM:0x%08x\r\n",TxEvent.MsgMarker);
            log_info("\t ET:0x%08x\r\n",TxEvent.EventType);
            log_info("\t FDF:0x%08x\r\n",TxEvent.FDFormat);
            log_info("\t BRS:0x%08x\r\n",TxEvent.BitRateSwitch);
            log_info("\t DLC:0x%08x\r\n",TxEvent.DataLength);
            log_info("\t TXTS:0x%08x\r\n",TxEvent.TxTimestamp);
        }
    }
}

/**
 *\*\name   LED_Init.
 *\*\fun    Initialize a GPIO as LED port.
 *\*\param  GPIOx
 *\*\         - GPIOA
 *\*\         - GPIOB
 *\*\         - GPIOC
 *\*\         - GPIOD
 *\*\         - GPIOE
 *\*\         - GPIOF
 *\*\         - GPIOG
 *\*\         - GPIOH
 *\*\param  Pin
 *\*\         - GPIO_PIN_0
 *\*\         - GPIO_PIN_1
 *\*\         - GPIO_PIN_2
 *\*\         - GPIO_PIN_3
 *\*\         - GPIO_PIN_4
 *\*\         - GPIO_PIN_5
 *\*\         - GPIO_PIN_6
 *\*\         - GPIO_PIN_7
 *\*\         - GPIO_PIN_8
 *\*\         - GPIO_PIN_9
 *\*\         - GPIO_PIN_10
 *\*\         - GPIO_PIN_11
 *\*\         - GPIO_PIN_12
 *\*\         - GPIO_PIN_13
 *\*\         - GPIO_PIN_14
 *\*\         - GPIO_PIN_15
 *\*\         - GPIO_PIN_ALL
 *\*\param  clock
 *\*\         - RCC_AHB_PERIPHEN_GPIOA
 *\*\         - RCC_AHB_PERIPHEN_GPIOB
 *\*\         - RCC_AHB_PERIPHEN_GPIOC
 *\*\         - RCC_AHB_PERIPHEN_GPIOD
 *\*\         - RCC_AHB_PERIPHEN_GPIOE
 *\*\         - RCC_AHB_PERIPHEN_GPIOF
 *\*\         - RCC_AHB_PERIPHEN_GPIOG
 *\*\         - RCC_AHB_PERIPHEN_GPIOH
 *\*\return none
 */
void LED_Init(GPIO_Module* GPIOx,uint16_t Pin, uint32_t clock)
{
    GPIO_InitType InitStruct;
    
    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(clock,ENABLE);
    
    /* Init GPIO as output push-pull mode */
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
 *\*\fun    Turn on LED by set GPIO pin.
 *\*\param  none
 *\*\return none
 */
void LED_On(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_SetBits(GPIOx,Pin);
}

/**
 *\*\name   LED_Off.
 *\*\fun    Turn off LED by reset GPIO pin.
 *\*\param  none
 *\*\return none
 */
void LED_Off(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_ResetBits(GPIOx,Pin);
}

/**
 *\*\name   LED_Blink.
 *\*\fun    Blink LED by toggle GPIO pin.
 *\*\param  none
 *\*\return none
 */
void LED_Blink(GPIO_Module* GPIOx,uint16_t Pin)
{
    GPIO_TogglePin(GPIOx,Pin);
}

/**
 *\*\name   Key_Init.
 *\*\fun    Initialize a GPIO as key port.
 *\*\param  GPIOx
 *\*\         - GPIOA
 *\*\         - GPIOB
 *\*\         - GPIOC
 *\*\         - GPIOD
 *\*\         - GPIOE
 *\*\         - GPIOF
 *\*\         - GPIOG
 *\*\         - GPIOH
 *\*\param  Pin
 *\*\         - GPIO_PIN_0
 *\*\         - GPIO_PIN_1
 *\*\         - GPIO_PIN_2
 *\*\         - GPIO_PIN_3
 *\*\         - GPIO_PIN_4
 *\*\         - GPIO_PIN_5
 *\*\         - GPIO_PIN_6
 *\*\         - GPIO_PIN_7
 *\*\         - GPIO_PIN_8
 *\*\         - GPIO_PIN_9
 *\*\         - GPIO_PIN_10
 *\*\         - GPIO_PIN_11
 *\*\         - GPIO_PIN_12
 *\*\         - GPIO_PIN_13
 *\*\         - GPIO_PIN_14
 *\*\         - GPIO_PIN_15
 *\*\         - GPIO_PIN_ALL
 *\*\param  clock
 *\*\         - RCC_AHB_PERIPHEN_GPIOA
 *\*\         - RCC_AHB_PERIPHEN_GPIOB
 *\*\         - RCC_AHB_PERIPHEN_GPIOC
 *\*\         - RCC_AHB_PERIPHEN_GPIOD
 *\*\         - RCC_AHB_PERIPHEN_GPIOE
 *\*\         - RCC_AHB_PERIPHEN_GPIOF
 *\*\         - RCC_AHB_PERIPHEN_GPIOG
 *\*\         - RCC_AHB_PERIPHEN_GPIOH
 *\*\return none
 */
void Key_Init(GPIO_Module* GPIOx,uint16_t Pin, uint32_t clock)
{
    GPIO_InitType InitStruct;
    
    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(clock,ENABLE);
    
    /* Init GPIO as input push-down mode */
    InitStruct.Pin            = Pin;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_AF0;
    InitStruct.GPIO_Pull      = GPIO_PULL_DOWN;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    
    GPIO_InitPeripheral(GPIOx, &InitStruct);
}

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
 *\*\name   FDCAN_clock_src_config.
 *\*\fun    Config the clock source of all FDCAN module.
 *\*\param  none
 *\*\return none
 */
void FDCAN_clock_src_config(void)
{
    /* Config  PLL prescaler for FDCAN */
#if (  defined(N32H473) \
    || defined(N32H474) \
    || defined(N32H475) \
    || defined(N32H482) \
    || defined(N32H487) \
    || defined(N32H481) \
    || defined (N32H488)    )
    RCC_ConfigFDCANPllClk(RCC_FDCAN_PLLSRC_DIV12);
#elif (defined (N32H480))
    RCC_ConfigFDCANPllClk(RCC_FDCAN_PLLSRC_DIV9);
#else
    #error MCU type is wrong!
#endif  

    /* Select PLL as FDCAN clock source */
    RCC_ConfigFDCANClksrc(RCC_FDCAN_CLKSRC_PLL);
}

/**
 *\*\name   Node1_port_config.
 *\*\fun    Config the GPIO used for FDCAN node 1.
 *\*\param  none
 *\*\return none
 */
void Node1_port_config(void)
{
    GPIO_InitType InitStruct;

    RCC_EnableAHB1PeriphClk(NODE1_TX_PORT_CLK,ENABLE);

    InitStruct.Pin            = NODE1_TX_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_AF_PP;
    InitStruct.GPIO_Alternate = NODE1_TX_PIN_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(NODE1_TX_PORT,&InitStruct);
}

/**
 *\*\name   Node1_Config.
 *\*\fun    Config FDCAN node 1.
 *\*\param  none
 *\*\return none
 */
void Node1_Config(void)
{
    FDCAN_InitType InitParam;
    FDCAN_FilterType FilterParam;
    
    /* Enable NODE1 clock */
    RCC_EnableAPB1PeriphClk(NODE1_PERIPH,ENABLE);

    /* Reset NODE1 register */
    RCC_EnableAPB1PeriphReset(NODE1_PERIPH);

    /** FDCAN config parameter **/
    InitParam.FrameFormat           = FDCAN_FRAME_FD_BRS;          /* Frame format */
    InitParam.Mode                  = FDCAN_MODE_EXTERNAL_LOOPBACK; /* Work mode */
    InitParam.Prescaler             = 2;    /* Nominal timing  */
    InitParam.SyncJumpWidth         = 8;
    InitParam.TimeSeg1              = 31;
    InitParam.TimeSeg2              = 8;
    InitParam.DataPrescaler         = 2;    /* Data timing  */
    InitParam.DataSyncJumpWidth     = 4;
    InitParam.DataTimeSeg1          = 15;
    InitParam.DataTimeSeg2          = 4;
    InitParam.MsgRamStrAddr         = (uint32_t)FDCAN_ram;   /* Msg ram start address, shared by all FDCAN modules */
    InitParam.MsgRamOffset          = 0;    /* Current NODE1 msg ram start offset  */
    InitParam.pMsgInfo              = &Node1_msg;
    InitParam.StdFilterSize         = 1;    /* Standard filter list */
    InitParam.ExtFilterSize         = 1;    /* Extended filter list */
    InitParam.RxFifo0Size           = 0;    /* Rx FIFO 0*/
    InitParam.RxFifo0DataSize       = FDCAN_DATA_BYTES_20;
    InitParam.RxFifo1Size           = 3;    /* Rx FIFO 1*/
    InitParam.RxFifo1DataSize       = FDCAN_DATA_BYTES_20;
    InitParam.RxBufferSize          = 1;    /* Dedicate Rx buffer */
    InitParam.RxBufferDataSize      = FDCAN_DATA_BYTES_20;
    InitParam.TxBufferSize          = 1;    /* Tx buffer*/
    InitParam.TxBufferDataSize      = FDCAN_DATA_BYTES_20;
    InitParam.TxFifoQueueSize       = 2;    /* Tx FIFO */
    InitParam.TxFifoQueueMode       = FDCAN_TX_FIFO_MODE;
    InitParam.TxEventSize           = 9;    /* Tx event fifo */
    InitParam.AutoRetransmission    = ENABLE;   /* Enable auto retransmission */
    InitParam.TransmitPause         = DISABLE;  /* Disable transmit pause*/
    InitParam.ProtocolException     = ENABLE;   /* Enable Protocol Exception Handling */

    /* Init NODE1 */
    FDCAN_Init(NODE1,&InitParam);

    /* Check message ram size */
    if(Node1_msg.EndAddress > ((uint32_t)FDCAN_ram + (MSG_RAM_SIZE*4U)))
    {
        log_info("\r\n NODE1 init error:message ram is too small!\r\n");
        while(1);
    }
    
    /* Configure standard ID reception filter to Rx buffer 0 */
    FilterParam.IdType          = FDCAN_STANDARD_ID;
    FilterParam.FilterIndex     = 0;
    FilterParam.FilterType      = FDCAN_FILTER_DUAL;
    FilterParam.FilterConfig    = FDCAN_FILTER_TO_RXBUFFER;
    FilterParam.FilterID1       = 0x111;
    FilterParam.FilterID2       = 0x555;
    FilterParam.RxBufferIndex   = 0;
    FDCAN_ConfigFilter(NODE1,&FilterParam);

    /* Configure extended ID reception filter to Rx FIFO 1 */
    FilterParam.IdType          = FDCAN_EXTENDED_ID;
    FilterParam.FilterIndex     = 0;
    FilterParam.FilterType      = FDCAN_FILTER_RANGE_NO_EIDM;
    FilterParam.FilterConfig    = FDCAN_FILTER_TO_RXFIFO1;
    FilterParam.FilterID1       = 0x1111111;
    FilterParam.FilterID2       = 0x2222222;
    FDCAN_ConfigFilter(NODE1,&FilterParam);
  
    FDCAN_ConfigGlobalFilter(   NODE1,
                                FDCAN_REJECT_STD,
                                FDCAN_REJECT_EXT,
                                FDCAN_REJECT_STD_REMOTE,
                                FDCAN_REJECT_EXT_REMOTE);
                                
    FDCAN_ConfigTSPrescaler(NODE1,FDCAN_TIMESTAMP_PRESC_16);
    FDCAN_Config_TS(NODE1,FDCAN_TIMESTAMP_INTERNAL);
                                
    /* Start the FDCAN module */
    FDCAN_Start(NODE1);
}

/**
 *\*\name   Node1_SendMsg.
 *\*\fun    Send some messages.
 *\*\param  none
 *\*\return none
 */
void Node1_SendMsg(void)
{
    FDCAN_TxHeaderType TxHeader;
    static uint32_t send_cnt = 0;
    
    if(0 == send_cnt)
    {
        /* Add message to Tx buffer */
        TxHeader.ID             = 0x111;
        TxHeader.IdType         = FDCAN_STANDARD_ID;
        TxHeader.TxFrameType    = FDCAN_DATA_FRAME;
        TxHeader.DataLength     = FDCAN_DLC_BYTES_20;
        TxHeader.ErrorState     = FDCAN_ESI_ACTIVE;
        TxHeader.BitRateSwitch  = FDCAN_BRS_ON;
        TxHeader.FDFormat       = FDCAN_FD_CAN;
        TxHeader.TxEventFifo    = FDCAN_STORE_TX_EVENTS;
        TxHeader.MsgMarker      = 0x52;
        FDCAN_AddMsgToTxBuffer(NODE1, &TxHeader, TxData0, FDCAN_TX_BUFFER0);
        FDCAN_EnableTxBufferRequest(NODE1,FDCAN_TX_BUFFER0);
        log_info("Standard frame send,ID=0x111");
    }
    else if(1 == send_cnt)
    {
        /* Add message to Tx FIFO */
        TxHeader.ID             = 0x1111112;
        TxHeader.IdType         = FDCAN_EXTENDED_ID;
        TxHeader.TxFrameType    = FDCAN_DATA_FRAME;
        TxHeader.DataLength     = FDCAN_DLC_BYTES_20;
        TxHeader.ErrorState     = FDCAN_ESI_PASSIVE;
        TxHeader.BitRateSwitch  = FDCAN_BRS_ON;
        TxHeader.FDFormat       = FDCAN_FD_CAN;
        TxHeader.TxEventFifo    = FDCAN_STORE_TX_EVENTS;
        TxHeader.MsgMarker      = 0xCC;
        FDCAN_AddMsgToTxFifoQ(NODE1, &TxHeader, TxData1);
        log_info("Extended frame send,ID=0x1111112");
    }
    else    //if(2 == send_cnt)
    {
        /* Add second message to Tx FIFO */
        TxHeader.ID             = 0x2222222;
        TxHeader.IdType         = FDCAN_EXTENDED_ID;
        TxHeader.TxFrameType    = FDCAN_DATA_FRAME;
        TxHeader.DataLength     = FDCAN_DLC_BYTES_20;
        TxHeader.ErrorState     = FDCAN_ESI_PASSIVE;
        TxHeader.BitRateSwitch  = FDCAN_BRS_ON;
        TxHeader.FDFormat       = FDCAN_FD_CAN;
        TxHeader.TxEventFifo    = FDCAN_STORE_TX_EVENTS;
        TxHeader.MsgMarker      = 0xDD;
        FDCAN_AddMsgToTxFifoQ(NODE1, &TxHeader, TxData2);
        log_info("Extended frame send,ID=0x2222222");
    }
    
    if(send_cnt >1)
        send_cnt = 0;
    else
        send_cnt++;
}
