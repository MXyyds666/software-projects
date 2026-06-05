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
#include "misc.h"

#define TEST_DATA_FRAME_SIZE    (8)     /* Data frame size,must not be greater than 64 */
#define MSG_RAM_SIZE            (0x100) /* Message ram size,must not be greater than 4480 */

FDCAN_TxHeaderType TxHeader1,TxHeader2;
uint8_t TxData1[TEST_DATA_FRAME_SIZE] = {  0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
uint8_t TxData2[TEST_DATA_FRAME_SIZE] = {  0x55, 0xAA, 0x11, 0x22, 0x33, 0xBB, 0xDD, 0xEE };

FDCAN_RxHeaderType RxHeader;
uint8_t RxBuf[64];

uint32_t FDCAN_ram[MSG_RAM_SIZE];  /* Used for FDCAN message ram, shared by all FDCAN modules */
FDCAN_MsgRamType Node1_msg,Node2_msg;

uint32_t RxFlag[2] = {0,0};
uint32_t DataFlag[2] = {0,0};
uint32_t TxFlag[2] = {0,0};
uint32_t IntFlag = 0;

/**
 *\*\name   main.
 *\*\fun    Main program.
 *\*\param  none
 *\*\return none
 */
int main(void)
{
    uint32_t key_cnt = 0;
    
    log_init();
    Delay(SystemCoreClock/2);

    log_info("\r\n FDCAN classic frame demo!\r\n");

    /* LED port init */
    LED_Init(LED1_PORT,LED1_PIN,LED1_CLOCK);
    LED_Init(LED2_PORT,LED2_PIN,LED2_CLOCK);

    /* KEY port init */
    Key_Init(KEY_PORT, KEY_PIN, KEY_CLOCK);

    FDCAN_clock_src_config();

    Node1_port_config();
    Node2_port_config();

    Node1_Config();
    Node2_Config();

    while (1)
    {
        if(KEY_PORT->PID & KEY_PIN)
        {
            key_cnt++;
        }
        else
        {
            key_cnt = 0;
        }
        
        if(key_cnt >= (SystemCoreClock/1000))
        {
            key_cnt = 0;
            IntFlag++;
            
            /* Node1 send */
            if(IntFlag & 0x01UL)
            {
                FDCAN_EnableInt(NODE1,FDCAN_INT_RX_FIFO0_NEW_MESSAGE);
            }
            else
            {
                FDCAN_DisableInt(NODE1,FDCAN_INT_RX_FIFO0_NEW_MESSAGE);
            }
            
            FDCAN_AddMsgToTxFifoQ(NODE1, &TxHeader1, TxData1);
            log_info("\r\n NODE1 send!\r\n");
            
            /* Node2 send */
            if(IntFlag & 0x01UL)
            {
                FDCAN_EnableInt(NODE2,FDCAN_INT_RX_FIFO1_NEW_MESSAGE);
            }
            else
            {
                FDCAN_DisableInt(NODE2,FDCAN_INT_RX_FIFO1_NEW_MESSAGE);
            }
            
            FDCAN_AddMsgToTxFifoQ(NODE2, &TxHeader2, TxData2);
            log_info("\r\n NODE2 send!\r\n");
        }
       
        if(TxFlag[0])
        {
            log_info("\r\n NODE1 transmit conpleted!\r\n");
            TxFlag[0]--;
        }
        
        if(TxFlag[1])
        {
            log_info("\r\n NODE2 transmit conpleted!\r\n");
            TxFlag[1]--;
        }
        
        if(IntFlag & 0x01UL)    /* Recieve by interrupt */
        {
            if(RxFlag[0])
            {
                RxFlag[0]--;
                DataFlag[0]++;
            }
            
            if(RxFlag[1])
            {
                RxFlag[1]--;
                DataFlag[1]++;
            }
        }
        else    /* Recieve by polling RX FIFO */
        {
            if(FDCAN_GetRxFifoFillLevel(NODE1,FDCAN_RX_FIFO0) > 0)
            {
                DataFlag[0]++;
            }
            
            if(FDCAN_GetRxFifoFillLevel(NODE2,FDCAN_RX_FIFO1) > 0)
            {
                DataFlag[1]++;
            }
        }
        
        if(DataFlag[0])
        {
            DataFlag[0]--;
            
            buf_clear(RxBuf,TEST_DATA_FRAME_SIZE,0xFF);
            
            LED_On(LED1_PORT,LED1_PIN); 
            if(FDCAN_GetRxMsg(NODE1,FDCAN_RX_FIFO0,&RxHeader,RxBuf) != ERROR)
            {
                if(IntFlag & 0x01UL)
                {
                    log_info("\r\n NODE1 Rx FIFO0 recieved by interrupt,");
                }
                else
                {
                    log_info("\r\n NODE1 Rx FIFO0 recieved by polling,");
                }
                log_info("ID=0x%03x,RXTS=0x%04x,data:\r\n",RxHeader.ID,RxHeader.RxTimestamp);
                log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[0], RxBuf[1], RxBuf[2], RxBuf[3]);
                log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[4], RxBuf[5], RxBuf[6], RxBuf[7]);
            }
            
            LED_Off(LED1_PORT,LED1_PIN);
        }

        if(DataFlag[1])
        {
            DataFlag[1]--;
            
            buf_clear(RxBuf,TEST_DATA_FRAME_SIZE,0xEE);
            
            LED_On(LED2_PORT,LED2_PIN);
            if(FDCAN_GetRxMsg(NODE2,FDCAN_RX_FIFO1,&RxHeader,RxBuf) != ERROR)
            {
                if(IntFlag & 0x01UL)
                {
                    log_info("\r\n NODE2 Rx FIFO1 recieved by interrupt,");
                }
                else
                {
                    log_info("\r\n NODE2 Rx FIFO1 recieved by polling,");
                }
                log_info("ID=0x%03x,RXTS=0x%04x,data:\r\n",RxHeader.ID,RxHeader.RxTimestamp);
                log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[0], RxBuf[1], RxBuf[2], RxBuf[3]);
                log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[4], RxBuf[5], RxBuf[6], RxBuf[7]);
            }
            
            LED_Off(LED2_PORT,LED2_PIN);
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
 *\*\name   Delay.
 *\*\fun    Delay a short time.
 *\*\param  none
 *\*\return none
 */
void buf_clear(uint8_t *buf, uint32_t len,uint8_t data)
{
    uint32_t i;
    
    for(i=0;i<len;i++)
    {
        buf[i] = data;
    }
}

/**
 *\*\name   FDCAN_clock_src_config.
 *\*\fun    Config the clock source of all FDCAN module.
 *\*\param  none
 *\*\return none
 *\*\note   FDCAN clock source should be set to one of the following values: 20M,40M,80M.
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

    RCC_EnableAHB1PeriphClk(NODE1_TX_PORT_CLK|NODE1_RX_PORT_CLK,ENABLE);

    InitStruct.Pin            = NODE1_TX_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_AF_PP;
    InitStruct.GPIO_Alternate = NODE1_TX_PIN_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(NODE1_TX_PORT,&InitStruct);

    InitStruct.Pin            = NODE1_RX_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = NODE1_RX_PIN_AF;
    InitStruct.GPIO_Pull      = GPIO_PULL_UP;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(NODE1_RX_PORT,&InitStruct);
}

/**
 *\*\name   Node2_port_config.
 *\*\fun    Config the GPIO used for FDCAN node 2.
 *\*\param  none
 *\*\return none
 */
void Node2_port_config(void)
{
    GPIO_InitType InitStruct;

    RCC_EnableAHB1PeriphClk(NODE2_TX_PORT_CLK|NODE2_RX_PORT_CLK,ENABLE);

    InitStruct.Pin            = NODE2_TX_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_AF_PP;
    InitStruct.GPIO_Alternate = NODE2_TX_PIN_AF;
    InitStruct.GPIO_Pull      = GPIO_NO_PULL;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(NODE2_TX_PORT,&InitStruct);

    InitStruct.Pin            = NODE2_RX_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = NODE2_RX_PIN_AF;
    InitStruct.GPIO_Pull      = GPIO_PULL_UP;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(NODE2_RX_PORT,&InitStruct);
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
    NVIC_InitType NVIC_Struct;
    
    /* Enable NODE1 clock */
    RCC_EnableAPB1PeriphClk(NODE1_PERIPH,ENABLE);

    /* Reset NODE1 register */
    RCC_EnableAPB1PeriphReset(NODE1_PERIPH);

    /** FDCAN config parameter **/
    InitParam.FrameFormat           = FDCAN_FRAME_CLASSIC;          /* Frame format */
    InitParam.Mode                  = FDCAN_MODE_NORMAL; /* Work mode */
    InitParam.Prescaler             = 1;    /* Nominal timing  */
    InitParam.SyncJumpWidth         = 8;
    InitParam.TimeSeg1              = 31;
    InitParam.TimeSeg2              = 8;
    InitParam.MsgRamStrAddr         = (uint32_t)FDCAN_ram;   /* Msg ram start address, shared by all FDCAN modules */
    InitParam.MsgRamOffset          = 0;    /* Current NODE1 msg ram start offset  */
    InitParam.pMsgInfo              = &Node1_msg;
    InitParam.StdFilterSize         = 1;    /* Standard filter list */
    InitParam.ExtFilterSize         = 0;    /* Extended filter list */
    InitParam.RxFifo0Size           = 3;    /* Rx FIFO 0*/
    InitParam.RxFifo0DataSize       = FDCAN_DATA_BYTES_8;
    InitParam.RxFifo1Size           = 0;    /* Rx FIFO 1*/
    InitParam.RxFifo1DataSize       = FDCAN_DATA_BYTES_8;
    InitParam.RxBufferSize          = 0;    /* Dedicate Rx buffer */
    InitParam.RxBufferDataSize      = FDCAN_DATA_BYTES_8;
    InitParam.TxBufferSize          = 0;    /* Tx buffer*/
    InitParam.TxBufferDataSize      = FDCAN_DATA_BYTES_8;
    InitParam.TxFifoQueueSize       = 2;    /* Tx FIFO */
    InitParam.TxFifoQueueMode       = FDCAN_TX_FIFO_MODE;
    InitParam.TxEventSize           = 0;    /* Tx event fifo */
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
    FilterParam.FilterType      = FDCAN_FILTER_MASK;
    FilterParam.FilterConfig    = FDCAN_FILTER_TO_RXFIFO0;
    FilterParam.FilterID1       = 0x4AA;
    FilterParam.FilterID2       = 0x7FF;
    FDCAN_ConfigFilter(NODE1,&FilterParam);
    
    FDCAN_ConfigGlobalFilter(   NODE1,
                                FDCAN_REJECT_STD,
                                FDCAN_REJECT_EXT,
                                FDCAN_REJECT_STD_REMOTE,
                                FDCAN_REJECT_EXT_REMOTE);
                                
    FDCAN_ConfigTSPrescaler(NODE1,FDCAN_TIMESTAMP_PRESC_16);
    FDCAN_Config_TS(NODE1,FDCAN_TIMESTAMP_INTERNAL);
    
    FDCAN_ConfigIntLine(NODE1,FDCAN_INT_TX_COMPLETE,FDCAN_INTERRUPT_LINE1);
    FDCAN_ActivateInt(NODE1,FDCAN_INT_TX_COMPLETE,FDCAN_TX_BUFFER0|FDCAN_TX_BUFFER1);
    
    FDCAN_ConfigIntLine(NODE1,FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE,FDCAN_INTERRUPT_LINE1);
    FDCAN_ActivateInt(NODE1,FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE,FDCAN_TX_BUFFER0);
    
    /* Start the FDCAN module */
    FDCAN_Start(NODE1);
    
    TxHeader1.ID             = 0x555;
    TxHeader1.IdType         = FDCAN_STANDARD_ID;
    TxHeader1.TxFrameType    = FDCAN_DATA_FRAME;
    TxHeader1.DataLength     = FDCAN_DLC_BYTES_8;
    TxHeader1.ErrorState     = FDCAN_ESI_PASSIVE;
    TxHeader1.BitRateSwitch  = FDCAN_BRS_OFF;
    TxHeader1.FDFormat       = FDCAN_CLASSIC_CAN;
    TxHeader1.TxEventFifo    = FDCAN_NO_TX_EVENTS;
    TxHeader1.MsgMarker      = 0x55;
    
    NVIC_Struct.NVIC_IRQChannel = NODE1_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
}

/**
 *\*\name   Node2_Config.
 *\*\fun    Config FDCAN node 2.
 *\*\param  none
 *\*\return none
 */
void Node2_Config(void)
{
    FDCAN_InitType InitParam;
    FDCAN_FilterType FilterParam;
    NVIC_InitType NVIC_Struct;
    
    /* Enable NODE2 clock */
    RCC_EnableAPB1PeriphClk(NODE2_PERIPH,ENABLE);

    /* Reset NODE2 register */
    RCC_EnableAPB1PeriphReset(NODE2_PERIPH);

    /** FDCAN config parameter **/
    InitParam.FrameFormat           = FDCAN_FRAME_CLASSIC;          /* Frame format */
    InitParam.Mode                  = FDCAN_MODE_NORMAL; /* Work mode */
    InitParam.Prescaler             = 1;    /* Nominal timing  */
    InitParam.SyncJumpWidth         = 8;
    InitParam.TimeSeg1              = 31;
    InitParam.TimeSeg2              = 8;
    InitParam.MsgRamStrAddr         = (uint32_t)FDCAN_ram;   /* Msg ram start address, shared by all FDCAN modules */
    InitParam.MsgRamOffset          = 80;    /* Current NODE2 msg ram start offset  */
    InitParam.pMsgInfo              = &Node2_msg;
    InitParam.StdFilterSize         = 1;    /* Standard filter list */
    InitParam.ExtFilterSize         = 0;    /* Extended filter list */
    InitParam.RxFifo0Size           = 0;    /* Rx FIFO 0*/
    InitParam.RxFifo0DataSize       = FDCAN_DATA_BYTES_8;
    InitParam.RxFifo1Size           = 3;    /* Rx FIFO 1*/
    InitParam.RxFifo1DataSize       = FDCAN_DATA_BYTES_8;
    InitParam.RxBufferSize          = 0;    /* Dedicate Rx buffer */
    InitParam.RxBufferDataSize      = FDCAN_DATA_BYTES_8;
    InitParam.TxBufferSize          = 0;    /* Tx buffer*/
    InitParam.TxBufferDataSize      = FDCAN_DATA_BYTES_8;
    InitParam.TxFifoQueueSize       = 2;    /* Tx FIFO */
    InitParam.TxFifoQueueMode       = FDCAN_TX_FIFO_MODE;
    InitParam.TxEventSize           = 0;    /* Tx event fifo */
    InitParam.AutoRetransmission    = ENABLE;   /* Enable auto retransmission */
    InitParam.TransmitPause         = DISABLE;  /* Disable transmit pause*/
    InitParam.ProtocolException     = ENABLE;   /* Enable Protocol Exception Handling */

    /* Init NODE2 */
    FDCAN_Init(NODE2,&InitParam);

    /* Check message ram size */
    if(Node2_msg.EndAddress > ((uint32_t)FDCAN_ram + (MSG_RAM_SIZE*4U)))
    {
        log_info("\r\n NODE2 init error:message ram is too small!\r\n");
        while(1);
    }
    
    /* Configure standard ID reception filter to Rx buffer 0 */
    FilterParam.IdType          = FDCAN_STANDARD_ID;
    FilterParam.FilterIndex     = 0;
    FilterParam.FilterType      = FDCAN_FILTER_MASK;
    FilterParam.FilterConfig    = FDCAN_FILTER_TO_RXFIFO1;
    FilterParam.FilterID1       = 0x555;
    FilterParam.FilterID2       = 0x7FF;
    FDCAN_ConfigFilter(NODE2,&FilterParam);
    
    FDCAN_ConfigGlobalFilter(   NODE2,
                                FDCAN_REJECT_STD,
                                FDCAN_REJECT_EXT,
                                FDCAN_REJECT_STD_REMOTE,
                                FDCAN_REJECT_EXT_REMOTE);
                                
    FDCAN_ConfigTSPrescaler(NODE2,FDCAN_TIMESTAMP_PRESC_16);
    FDCAN_Config_TS(NODE2,FDCAN_TIMESTAMP_INTERNAL);
    
    FDCAN_ConfigIntLine(NODE2,FDCAN_INT_TX_COMPLETE,FDCAN_INTERRUPT_LINE1);
    FDCAN_ActivateInt(NODE2,FDCAN_INT_TX_COMPLETE,FDCAN_TX_BUFFER0|FDCAN_TX_BUFFER1);
    
    FDCAN_ConfigIntLine(NODE2,FDCAN_FLAG_RX_FIFO1_NEW_MESSAGE,FDCAN_INTERRUPT_LINE1);
    FDCAN_ActivateInt(NODE2,FDCAN_FLAG_RX_FIFO1_NEW_MESSAGE,FDCAN_TX_BUFFER0);
    
    /* Start the FDCAN module */
    FDCAN_Start(NODE2);

    TxHeader2.ID             = 0x4AA;
    TxHeader2.IdType         = FDCAN_STANDARD_ID;
    TxHeader2.TxFrameType    = FDCAN_DATA_FRAME;
    TxHeader2.DataLength     = FDCAN_DLC_BYTES_8;
    TxHeader2.ErrorState     = FDCAN_ESI_ACTIVE;
    TxHeader2.BitRateSwitch  = FDCAN_BRS_OFF;
    TxHeader2.FDFormat       = FDCAN_CLASSIC_CAN;
    TxHeader2.TxEventFifo    = FDCAN_NO_TX_EVENTS;
    TxHeader2.MsgMarker      = 0xAA;
    
    NVIC_Struct.NVIC_IRQChannel = NODE2_IRQN;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);
}
