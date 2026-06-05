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
#include "n32h47x_48x_exti.h"
#include "misc.h"
#include "log.h"

#define TEST_NOMINAL_PRESCALER  (2)
#define TEST_NOMINAL_SYNCJUMP   (8)
#define TEST_NOMINAL_TSEG1      (31)
#define TEST_NOMINAL_TSEG2      (8)
#define TEST_DATA_PRESCALER     (2)
#define TEST_DATA_SYNCJUMP      (4)
#define TEST_DATA_TSEG1         (15)
#define TEST_DATA_TSEG2         (4)
#define TEST_TIESTAMP_PRESCALER (FDCAN_TIMESTAMP_PRESC_16)

#define TEST_FRAME_NUMBER       (2)
#define TEST_DATA_FRAME_SIZE    (16)   /* Data frame size,must not be greater than 64 */
#define MSG_RAM_SIZE            (0x100) /* Message ram size,must not be greater than 4480 */
#define TEST_TXBUF_SIZE         (TEST_DATA_FRAME_SIZE*TEST_FRAME_NUMBER)

static uint32_t data_cnt = 0;
uint8_t TxFlag = 0;
FDCAN_TxHeaderType TxHeader;
uint8_t TxData[TEST_TXBUF_SIZE] = { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
                                    0x89, 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0,
                                    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 
                                    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF  };

FDCAN_RxHeaderType RxHeader;
uint8_t RxBuf[64];

uint32_t FDCAN_ram[MSG_RAM_SIZE];  /* Used for FDCAN message ram, shared by all FDCAN module */

FDCAN_MsgRamType Node1_msg,Node2_msg;

/**
 *\*\name   main.
 *\*\fun    Main program.
 *\*\param  none
 *\*\return none
 */
int main(void)
{
    uint32_t i;
    
    log_init();
    Delay(SystemCoreClock/2);

    log_info("\r\n FDCAN polling demo!\r\n");

    /* LED port init */
    LED_Init(LED1_PORT,LED1_PIN,LED1_CLOCK);
    LED_Init(LED2_PORT,LED2_PIN,LED2_CLOCK);

    /* KEY port init */
    Key_Int_Init();

    FDCAN_clock_src_config();

    Node1_port_config();
    Node2_port_config();

    Node1_Config();
    Node2_Config();

    while (1)
    {
        if(TxFlag)
        {
            LED_On(LED1_PORT,LED1_PIN);

            FDCAN_AddMsgToTxFifoQ(NODE1, &TxHeader, &TxData[data_cnt]);
            
            data_cnt += TEST_DATA_FRAME_SIZE;
            if(data_cnt >= TEST_TXBUF_SIZE)
            {
                data_cnt = 0;
            }
            
            log_info("\r\n NODE1 send a frame!\r\n");
            LED_Off(LED1_PORT,LED1_PIN);
            TxFlag = 0;
        }
        
        if(FDCAN_GetRxFifoFillLevel(NODE2,FDCAN_RX_FIFO1) > 0)
        {
            LED_On(LED2_PORT,LED2_PIN);
            buf_clear(RxBuf,TEST_DATA_FRAME_SIZE,0xEE);
            FDCAN_GetRxMsg(NODE2,FDCAN_RX_FIFO1,&RxHeader,RxBuf);
            log_info("\r\n NODE2 Rx FIFO1 recieved a frame\r\nID=0x%03x,RXTS=0x%04x,data:\r\n",RxHeader.ID,RxHeader.RxTimestamp);
            for(i=0;i<TEST_DATA_FRAME_SIZE;)
            {
                log_info("\t 0x%02x,0x%02x,0x%02x,0x%02x\r\n",RxBuf[i], RxBuf[i+1], RxBuf[i+2], RxBuf[i+3]);
                i += 4;
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
void Key_Int_Init(void)
{
    GPIO_InitType InitStruct;
    EXTI_InitType EXTI_Struct;
    NVIC_InitType NVIC_Struct;
    
    /* Enable GPIO clock */
    RCC_EnableAHB1PeriphClk(KEY_CLOCK,ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO,ENABLE);
    
    AFIO_Config_EXTI_Filter(AFIO_EXTI_FILTER_ENABLE);
    
    /* Init GPIO as input push-down mode */
    InitStruct.Pin            = KEY_PIN;
    InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    InitStruct.GPIO_Mode      = GPIO_MODE_INPUT;
    InitStruct.GPIO_Alternate = GPIO_AF0;
    InitStruct.GPIO_Pull      = GPIO_PULL_DOWN;
    InitStruct.GPIO_Current   = GPIO_DS_8mA;
    GPIO_InitPeripheral(KEY_PORT, &InitStruct);

    EXTI_Struct.EXTI_Line    = EXTI_LINE1;
    EXTI_Struct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_Struct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Struct.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_Struct);

    NVIC_Struct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_Struct.NVIC_IRQChannelSubPriority = NVIC_SUB_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelPreemptionPriority = NVIC_PRE_PRIORITY_0;
    NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Struct);

    GPIO_ConfigEXTILine(EXTI_LINE_SOURCE1,KEY_PORT_SOURCE,KEY_PIN_SOURCE);
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
    
    /* Enable NODE1 clock */
    RCC_EnableAPB1PeriphClk(NODE1_PERIPH,ENABLE);

    /* Reset NODE1 register */
    RCC_EnableAPB1PeriphReset(NODE1_PERIPH);

    /** FDCAN config parameter **/
    InitParam.FrameFormat           = FDCAN_FRAME_FD_BRS;          /* Frame format */
    InitParam.Mode                  = FDCAN_MODE_NORMAL; /* Work mode */
    InitParam.Prescaler             = TEST_NOMINAL_PRESCALER;    /* Nominal timing  */
    InitParam.SyncJumpWidth         = TEST_NOMINAL_SYNCJUMP;
    InitParam.TimeSeg1              = TEST_NOMINAL_TSEG1;
    InitParam.TimeSeg2              = TEST_NOMINAL_TSEG2;
    InitParam.DataPrescaler         = TEST_DATA_PRESCALER;    /* Data timing  */
    InitParam.DataSyncJumpWidth     = TEST_DATA_SYNCJUMP;
    InitParam.DataTimeSeg1          = TEST_DATA_TSEG1;
    InitParam.DataTimeSeg2          = TEST_DATA_TSEG2;
    InitParam.MsgRamStrAddr         = (uint32_t)FDCAN_ram;   /* Msg ram start address, shared by all FDCAN modules */
    InitParam.MsgRamOffset          = 0;    /* Current NODE1 msg ram start offset  */
    InitParam.pMsgInfo              = &Node1_msg;
    InitParam.StdFilterSize         = 0;    /* Standard filter list */
    InitParam.ExtFilterSize         = 1;    /* Extended filter list */
    InitParam.RxFifo0Size           = 2;    /* Rx FIFO 0*/
    InitParam.RxFifo0DataSize       = FDCAN_DATA_BYTES_16;
    InitParam.RxFifo1Size           = 0;    /* Rx FIFO 1*/
    InitParam.RxFifo1DataSize       = FDCAN_DATA_BYTES_16;
    InitParam.RxBufferSize          = 0;    /* Dedicate Rx buffer */
    InitParam.RxBufferDataSize      = FDCAN_DATA_BYTES_16;
    InitParam.TxBufferSize          = 0;    /* Tx buffer*/
    InitParam.TxBufferDataSize      = FDCAN_DATA_BYTES_16;
    InitParam.TxFifoQueueSize       = TEST_FRAME_NUMBER*3;    /* Tx FIFO */
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
    FilterParam.IdType          = FDCAN_EXTENDED_ID;
    FilterParam.FilterIndex     = 0;
    FilterParam.FilterType      = FDCAN_FILTER_MASK;
    FilterParam.FilterConfig    = FDCAN_FILTER_TO_RXFIFO0;
    FilterParam.FilterID1       = 0x0AAAAAAA;
    FilterParam.FilterID2       = 0x1FFFFFFF;
    FDCAN_ConfigFilter(NODE1,&FilterParam);
    
    FDCAN_ConfigGlobalFilter(   NODE1,
                                FDCAN_REJECT_STD,
                                FDCAN_REJECT_EXT,
                                FDCAN_REJECT_STD_REMOTE,
                                FDCAN_REJECT_EXT_REMOTE);
                                
    FDCAN_ConfigTSPrescaler(NODE1,TEST_TIESTAMP_PRESCALER);
    FDCAN_Config_TS(NODE1,FDCAN_TIMESTAMP_INTERNAL);
    
    /* Start the FDCAN module */
    FDCAN_Start(NODE1);
    
    TxHeader.ID             = 0x15555555;
    TxHeader.IdType         = FDCAN_EXTENDED_ID;
    TxHeader.TxFrameType    = FDCAN_DATA_FRAME;
    TxHeader.DataLength     = FDCAN_DLC_BYTES_16;
    TxHeader.ErrorState     = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch  = FDCAN_BRS_ON;
    TxHeader.FDFormat       = FDCAN_FD_CAN;
    TxHeader.TxEventFifo    = FDCAN_NO_TX_EVENTS;
    TxHeader.MsgMarker      = 0x55;
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
    
    /* Enable NODE2 clock */
    RCC_EnableAPB1PeriphClk(NODE2_PERIPH,ENABLE);

    /* Reset NODE2 register */
    RCC_EnableAPB1PeriphReset(NODE2_PERIPH);

    /** FDCAN config parameter **/
    InitParam.FrameFormat           = FDCAN_FRAME_FD_BRS;          /* Frame format */
    InitParam.Mode                  = FDCAN_MODE_NORMAL; /* Work mode */
    InitParam.Prescaler             = TEST_NOMINAL_PRESCALER;    /* Nominal timing  */
    InitParam.SyncJumpWidth         = TEST_NOMINAL_SYNCJUMP;
    InitParam.TimeSeg1              = TEST_NOMINAL_TSEG1;
    InitParam.TimeSeg2              = TEST_NOMINAL_TSEG2;
    InitParam.DataPrescaler         = TEST_DATA_PRESCALER;    /* Data timing  */
    InitParam.DataSyncJumpWidth     = TEST_DATA_SYNCJUMP;
    InitParam.DataTimeSeg1          = TEST_DATA_TSEG1;
    InitParam.DataTimeSeg2          = TEST_DATA_TSEG2;
    InitParam.MsgRamStrAddr         = (uint32_t)FDCAN_ram;   /* Msg ram start address, shared by all FDCAN modules */
    InitParam.MsgRamOffset          = 80;    /* Current NODE2 msg ram start offset  */
    InitParam.pMsgInfo              = &Node2_msg;
    InitParam.StdFilterSize         = 0;    /* Standard filter list */
    InitParam.ExtFilterSize         = 1;    /* Extended filter list */
    InitParam.RxFifo0Size           = 0;    /* Rx FIFO 0*/
    InitParam.RxFifo0DataSize       = FDCAN_DATA_BYTES_16;
    InitParam.RxFifo1Size           = TEST_FRAME_NUMBER*3;    /* Rx FIFO 1*/
    InitParam.RxFifo1DataSize       = FDCAN_DATA_BYTES_16;
    InitParam.RxBufferSize          = 0;    /* Dedicate Rx buffer */
    InitParam.RxBufferDataSize      = FDCAN_DATA_BYTES_16;
    InitParam.TxBufferSize          = 0;    /* Tx buffer*/
    InitParam.TxBufferDataSize      = FDCAN_DATA_BYTES_16;
    InitParam.TxFifoQueueSize       = 0;    /* Tx FIFO */
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
        log_info("\r\n NODE1 init error:message ram is too small!\r\n");
        while(1);
    }
    
    /* Configure standard ID reception filter to Rx buffer 0 */
    FilterParam.IdType          = FDCAN_EXTENDED_ID;
    FilterParam.FilterIndex     = 0;
    FilterParam.FilterType      = FDCAN_FILTER_MASK;
    FilterParam.FilterConfig    = FDCAN_FILTER_TO_RXFIFO1;
    FilterParam.FilterID1       = 0x15555555;
    FilterParam.FilterID2       = 0x1FFFFFFF;
    FDCAN_ConfigFilter(NODE2,&FilterParam);
    
    FDCAN_ConfigGlobalFilter(   NODE2,
                                FDCAN_REJECT_STD,
                                FDCAN_REJECT_EXT,
                                FDCAN_REJECT_STD_REMOTE,
                                FDCAN_REJECT_EXT_REMOTE);
                                
    FDCAN_ConfigTSPrescaler(NODE2,TEST_TIESTAMP_PRESCALER);
    FDCAN_Config_TS(NODE2,FDCAN_TIMESTAMP_INTERNAL);
    
    /* Start the FDCAN module */
    FDCAN_Start(NODE2);
}

