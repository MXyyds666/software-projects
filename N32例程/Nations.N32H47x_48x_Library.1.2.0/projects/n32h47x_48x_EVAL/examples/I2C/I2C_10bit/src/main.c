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


/** n32h47x_48x_StdPeriph_Examples **/

/** I2C_10bit **/

#define TEST_BUFFER_SIZE  100
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x8000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2C_FLAG_SCLHTO))
#define I2C_MASTER_ADDR   0x230
#define I2C_SLAVE_ADDR    0x2A0
#define I2C_FRAME_HEAD_TX 0xF0
#define I2C_FRAME_HEAD_RX 0xF1

#if defined (N32H475)
    #define I2Cx                I2C1
    #define I2Cx_RCC            RCC_APB1_PERIPH_I2C1
    #define I2Cx_SCL_PIN        GPIO_PIN_11
    #define I2Cx_SDA_PIN        GPIO_PIN_1
    #define I2Cx_SCL_GPIO       GPIOD
    #define I2Cx_SDA_GPIO       GPIOD
    #define I2Cx_SCL_AF         GPIO_AF5
    #define I2Cx_SDA_AF         GPIO_AF7

    #define I2Cx_clk_en         RCC_AHB_PERIPHEN_GPIOD

    #define Slave_SCL_PIN       GPIO_PIN_4
    #define Slave_SDA_PIN       GPIO_PIN_5
    #define Slave_GPIOx         GPIOA
    #define Slave_SCL_AF        GPIO_AF11
    #define Slave_SDA_AF        GPIO_AF11

    #define Slave_clk_en        RCC_AHB_PERIPHEN_GPIOA

#else
    #define I2Cx               I2C3
    #define I2Cx_RCC           RCC_APB1_PERIPH_I2C3

    #define I2Cx_SCL_PIN       GPIO_PIN_0
    #define I2Cx_SDA_PIN       GPIO_PIN_5
    #define I2Cx_SCL_GPIO      GPIOC
    #define I2Cx_SDA_GPIO      GPIOB
    #define I2Cx_SCL_AF        GPIO_AF8
    #define I2Cx_SDA_AF        GPIO_AF11

    #define I2Cx_clk_en        RCC_AHB_PERIPHEN_GPIOB|RCC_AHB_PERIPHEN_GPIOC

    #define Slave_SCL_PIN       GPIO_PIN_10
    #define Slave_SDA_PIN       GPIO_PIN_11
    #define Slave_GPIOx         GPIOB
    #define Slave_SCL_AF        GPIO_AF7
    #define Slave_SDA_AF        GPIO_AF7

    #define Slave_clk_en        RCC_AHB_PERIPHEN_GPIOB

#endif

static __IO uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

static uint8_t master_tx_buf[TEST_BUFFER_SIZE] = {0};
static uint8_t master_rx_buf[TEST_BUFFER_SIZE] = {0};
static uint8_t slave_tx_buf[TEST_BUFFER_SIZE] = {0};
static uint8_t slave_rx_buf[TEST_BUFFER_SIZE] = {0};
static CommCtrl_t Comm_Flag = C_READY;

static uint8_t rx_num = 0;
static uint8_t tx_num = 0;
volatile Status test_status = FAILED;
static uint8_t flag_master_complete = 0;
static TR_STA flag_master_dir = TRANSMIT;
static uint8_t flag_slave_complete = 0;
static TR_STA master_TR = TRANSMIT;

Status Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void Memset(void* s, uint8_t c, uint32_t count);
void CommTimeOut_CallBack(ErrCode_t errcode);


/**
*\*\name    NVIC_ConfigurationMaster.
*\*\fun     NVIC Configuration.
*\*\param   ch:1-i2c1 2-i2c2
*\*\return  result
**/
void NVIC_Configuration(uint8_t ch)
{
    NVIC_InitType NVIC_InitStructure;

    if (ch == 3)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C3_EV_IRQn;
    }

    if (ch == 2)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
    }

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    if (ch == 3)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C3_ER_IRQn; // test err
    }

    if (ch == 2)
    {
        NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn; // test err
    }

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    i2c_master_init.
*\*\fun     master gpio/rcc/i2c initializes.
*\*\param   none
*\*\return  result
**/
int i2c_master_init(void)
{
    I2C_InitType i2cx_master;
    GPIO_InitType i2cx_gpio;
    RCC_EnableAPB1PeriphClk(I2Cx_RCC, ENABLE);
    RCC_EnableAHB1PeriphClk(I2Cx_clk_en, ENABLE);
    I2Cx_SCL_GPIO->POD |= (I2Cx_SCL_PIN );//pull up
    I2Cx_SDA_GPIO->POD |= (I2Cx_SDA_PIN);//pull up
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);


    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&i2cx_gpio);

    /*PC0 -- SCL; PB5 -- SDA*/
    i2cx_gpio.Pin        = I2Cx_SCL_PIN;
    i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SCL_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SCL_GPIO, &i2cx_gpio);

    i2cx_gpio.Pin        = I2Cx_SDA_PIN;
    i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SDA_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SDA_GPIO, &i2cx_gpio);

    I2C_DeInit(I2Cx);
    I2C_InitStruct(&i2cx_master);
    i2cx_master.BusMode     = I2C_BUSMODE_I2C;
    i2cx_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2cx_master.OwnAddr1    = I2C_MASTER_ADDR;
    i2cx_master.AckEnable   = I2C_ACKEN;
    i2cx_master.AddrMode    = I2C_ADDR_MODE_10BIT;
    i2cx_master.ClkSpeed    = 100000; //100K

    I2C_Init(I2Cx, &i2cx_master);
    // int enable
    I2C_ConfigInt(I2Cx, I2C_INT_EVENT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
    NVIC_Configuration(3);
    I2C_Enable(I2Cx, ENABLE);


    return 0;
}

/**
*\*\name    i2c_slave_init.
*\*\fun     slave gpio/rcc/i2c initializes.
*\*\param   none
*\*\return  result
**/
int i2c_slave_init(void)
{
    I2C_InitType i2c2_slave;
    GPIO_InitType i2c2_gpio;
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C2, ENABLE);
    RCC_EnableAHB1PeriphClk(Slave_clk_en, ENABLE);

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&i2c2_gpio);

    /*PB10 -- SCL; PB11 -- SDA*/
    i2c2_gpio.Pin        = Slave_SCL_PIN | Slave_SDA_PIN;
    i2c2_gpio.GPIO_Pull  = GPIO_PULL_UP;
    i2c2_gpio.GPIO_Alternate = Slave_SCL_AF;
    i2c2_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
    i2c2_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(Slave_GPIOx, &i2c2_gpio);

    I2C_DeInit(I2C2);
    I2C_InitStruct(&i2c2_slave);
    i2c2_slave.BusMode     = I2C_BUSMODE_I2C;
    i2c2_slave.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2c2_slave.OwnAddr1    = I2C_SLAVE_ADDR;
    i2c2_slave.AckEnable   = I2C_ACKEN;
    i2c2_slave.AddrMode    = I2C_ADDR_MODE_10BIT;
    i2c2_slave.ClkSpeed    = 100000; //100K

    I2C_Init(I2C2, &i2c2_slave);
    // int enable
    I2C_ConfigInt(I2C2, I2C_INT_EVENT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
    NVIC_Configuration(2);
    I2C_Enable(I2C2, ENABLE);

    return 0;
}


/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none
**/
int main(void)
{
    uint16_t i = 0;

    log_init();
    log_info("\nthis is a i2c(10bit) master/slave demo\r\n");
    /* Initialize the I2C driver ---------------------------------*/
    i2c_master_init();
    i2c_slave_init();

    /*----------I2C3 Master write and I2C2 Slave read data---------*/
    for (i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        master_tx_buf[i] = i;
    }

    I2CTimeout = I2CT_LONG_TIMEOUT;

    /*Wait until no busy*/
    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(MASTER_BUSY);
            return 1;
        }
    }

    /* I2C3 Write data and I2C2 Read data */
    if (Comm_Flag == C_READY)
    {
        Comm_Flag = C_START_BIT;
        I2C_GenerateStart(I2Cx, ENABLE);
    }

    I2CTimeout = I2CT_LONG_TIMEOUT * 1000;

    /* Wait write/Read data complete*/
    while((!flag_master_complete) || (!flag_slave_complete))
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(MASTER_UNKNOW);
            return 1;
        }
    }

    I2CTimeout = I2CT_LONG_TIMEOUT;

    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(MASTER_BUSY);
            return 1;
        }
    }

    Comm_Flag = C_READY;

    /* Check if the data written to the memory is read correctly */
    test_status = Buffercmp(master_tx_buf, slave_rx_buf, TEST_BUFFER_SIZE);

    if (test_status == PASSED)
    {
        log_info("i2c(10bit) master->slave test pass\r\n");
    }
    else
    {
        log_info("i2c(10bit) master->slave test fail\r\n");
    }

    systick_delay_ms(1);

    /*----------I2C3 Master Read and I2C2 Slave Write data---------*/
    for (i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        slave_tx_buf[i] = (0x80 + i);
    }

    flag_master_complete = 0;
    flag_slave_complete = 0;
    master_TR = RECEIVE;

    I2CTimeout = I2CT_LONG_TIMEOUT;

    /*Wait until no busy*/
    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(MASTER_BUSY);
            return 1;
        }
    }


    I2C_ConfigAck(I2Cx, ENABLE);

    /* I2C3 Read data and I2C2 Write data */
    if (Comm_Flag == C_READY)
    {
        Comm_Flag = C_START_BIT;
        I2Cx->CTRL1 &= I2C_NACK_POS_CURRENT; // reset ACKPOS  //==========
        I2C_GenerateStart(I2Cx, ENABLE);
    }

    I2CTimeout = I2CT_LONG_TIMEOUT * 1000;

    /* Wait write/Read data complete*/
    while((!flag_master_complete) || (!flag_slave_complete))
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(MASTER_UNKNOW);
            return 1;
        }
    }

    /* master recv finish */
    flag_master_complete = 0;
    I2CTimeout = I2CT_LONG_TIMEOUT;

    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(MASTER_BUSY);
            return 1;
        }
    }

    Comm_Flag = C_READY;

    /* Check if the data written to the memory is read correctly */
    test_status = Buffercmp(master_rx_buf, slave_tx_buf, TEST_BUFFER_SIZE);

    if (test_status == PASSED)
    {
        log_info("i2c(10bit) master<-slave test pass\r\n");
    }
    else
    {
        log_info("i2c(10bit) master<-slave test fail\r\n");
    }

    while (1)
    {
    }
}

/**
*\*\name   Buffercmp.
*\*\fun    Compares two buffers.
*\*\param  pBuffer, pBuffer1: buffers to be compared.
*\*\param  BufferLength: buffer's length
*\*\return PASSED: pBuffer identical to pBuffer1
*\*\       FAILED: pBuffer differs from pBuffer1
**/
Status Buffercmp(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer != *pBuffer1)
        {
            return FAILED;
        }

        pBuffer++;
        pBuffer1++;
    }

    return PASSED;
}

/**
*\*\name  Memset.
*\*\fun   memery set a value
*\*\param s source
*\*\param c value
*\*\param count number
*\*\return pointer of the memery
**/
void Memset(void* s, uint8_t c, uint32_t count)
{
    char* xs = (char*)s;

    while (count--)
    {
        *xs++ = c;
    }

    return;
}

/**
*\*\name    I2C3_EV_IRQHandler.
*\*\fun     i2c event Interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2C3_EV_IRQHandler(void)
{
    unsigned int last_event;

    last_event = I2C_GetLastEvent(I2Cx);

    if ((last_event & I2C_ROLE_MASTER) == I2C_ROLE_MASTER) // master mode
    {
        switch (last_event)
        {
            case I2C_EVT_MASTER_MODE_FLAG: // EV5 start
                if (flag_master_dir == TRANSMIT)
                {
                    Memset(slave_rx_buf, 0, TEST_BUFFER_SIZE);
                    I2C_SendData(I2Cx, (I2C_FRAME_HEAD_TX | ((I2C_SLAVE_ADDR & 0x300) >> 7)));
                    tx_num = 0;
                }
                else if (flag_master_dir == RECEIVE)
                {
                    Memset(master_rx_buf, 0, TEST_BUFFER_SIZE);
                    I2C_SendData(I2Cx, (I2C_FRAME_HEAD_RX | ((I2C_SLAVE_ADDR & 0x300) >> 7)));
                    rx_num = 0;
                    flag_master_dir = TRANSMIT;
                }

                break;

            case I2C_EVT_MASTER_MODE_ADDRESS10_FLAG:
                I2C_SendData(I2Cx, (I2C_SLAVE_ADDR & 0xFF));
                break;

            case I2C_EVT_MASTER_TXMODE_FLAG: // 0x00070082.EV6
                Comm_Flag = C_READY;

                if (master_TR == RECEIVE)
                {
                    I2C_GenerateStart(I2Cx, ENABLE);
                    master_TR = NONE;
                    flag_master_dir = RECEIVE;
                }
                else
                {
                    I2C_SendData(I2Cx, master_tx_buf[tx_num++]);
                }

                break;

            case I2C_EVT_MASTER_DATA_SENDING: // 0x00070020. transmitting data
                if (tx_num < TEST_BUFFER_SIZE)
                {
                    I2C_SendData(I2Cx, master_tx_buf[tx_num++]);
                }

                break;

            case I2C_EVT_MASTER_DATA_SENDED: // 0x00070084.byte data send finish

                // bit2    BSF (Byte transfer finished)
                if (tx_num == TEST_BUFFER_SIZE)// data send finish
                {
                    if (Comm_Flag == C_READY)
                    {
                        Comm_Flag = C_STOP_BIT;
                        I2C_GenerateStop(I2Cx, ENABLE);
                        flag_master_complete = 1;
                    }
                }

                break;

            // MasterReceiver
            case I2C_EVT_MASTER_RXMODE_FLAG: // .EV6
                Comm_Flag = C_READY;

                if (TEST_BUFFER_SIZE == 1)
                {
                    I2C_ConfigAck(I2Cx, DISABLE);

                    if (Comm_Flag == C_READY)
                    {
                        Comm_Flag = C_STOP_BIT;
                        I2C_GenerateStop(I2Cx, ENABLE);
                    }
                }
                else if (TEST_BUFFER_SIZE == 2)
                {
                    I2Cx->CTRL1 |= I2C_NACK_POS_NEXT; /// set ACKPOS
                    I2C_ConfigAck(I2Cx, DISABLE);
                }

                break;

            case I2C_EVT_MASTER_DATA_RECVD_FLAG: // one byte recved :EV7.//BUSY,MSMODE and RXDATNE flags.
            case I2C_EVT_MASTER_SFT_DATA_RECVD_FLAG: // BUSY, MSMODE(Master) and Data register not empty, BSF(Byte transfer finished)flags.
                master_rx_buf[rx_num++] = I2C_RecvData(I2Cx);

                // before te last data,should config NACK and STOP
                if (rx_num == (TEST_BUFFER_SIZE - 1))
                {
                    I2C_ConfigAck(I2Cx, DISABLE);   // Disable I2C3 acknowledgement.

                    if (Comm_Flag == C_READY)
                    {
                        Comm_Flag = C_STOP_BIT;
                        I2C_GenerateStop(I2Cx, ENABLE); // Send I2C3 STOP Condition.
                    }
                }
                else if (rx_num == TEST_BUFFER_SIZE)
                {
                    flag_master_complete = 1;
                }

                break;

            case 0x00030201: // Arbitration lost
            case 0x00030401: // Acknowledge failure
            case 0x00030501: // Acknowledge failure and Bus error
                I2C_Enable(I2Cx, DISABLE);
                I2C_Enable(I2Cx, ENABLE);
                break;

            default:
                log_info("I2C error status:0x%04x\r\n", last_event);
                break;
        }
    }
}

/**
*\*\name    I2C3_ER_IRQHandler.
*\*\fun     i2c error Interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2C3_ER_IRQHandler(void)
{
    uint32_t last_event;
    last_event = I2C_GetLastEvent(I2Cx);

    if(last_event & I2C_EVT_SLAVE_ACK_MISS)
    {
        I2C_ClrFlag(I2Cx, I2C_FLAG_ACKFAIL);
    }
}
/**
*\*\name    I2C2_EV_IRQHandler.
*\*\fun     i2c event Interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2C2_EV_IRQHandler(void)
{
    uint8_t timeout_flag = 0;
    uint32_t last_event = 0;

    last_event = I2C_GetLastEvent(I2C2);

    switch (last_event)
    {
        case I2C_EVT_SLAVE_RECV_ADDR_MATCHED:
            rx_num = 0;
            break;

        case I2C_EVT_SLAVE_SEND_ADDR_MATCHED:
            tx_num = 0;
            I2C2->DAT = slave_tx_buf[tx_num++];
            break;

        case I2C_EVT_SLAVE_DATA_SENDING:
            break;

        case I2C_EVT_SLAVE_DATA_SENDED:
            I2C2->DAT = slave_tx_buf[tx_num++];
            break;

        case I2C_EVT_SLAVE_DATA_RECVD_NOBUSY:   //No busy when recv the last byte
        case I2C_EVT_SLAVE_DATA_RECVD:
            slave_rx_buf[rx_num++] = I2C2->DAT;
            break;

        case I2C_EVT_SLAVE_STOP_RECVD:
            I2C_Enable(I2C2, ENABLE);

            if((tx_num != 0) || (rx_num != 0))
            {
                flag_slave_complete = 1;
            }

            break;

        default:
            I2C_Enable(I2C2, ENABLE);
            timeout_flag = 1;
            break;
    }

    if (timeout_flag)
    {
        if ((I2CTimeout--) == 0)
        {
            CommTimeOut_CallBack(SLAVE_UNKNOW);
        }
    }
    else
        I2CTimeout = I2CT_LONG_TIMEOUT;
}

/**
*\*\name    I2C2_ER_IRQHandler.
*\*\fun     i2c error Interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2C2_ER_IRQHandler(void)
{
    uint32_t last_event;
    last_event = I2C_GetLastEvent(I2C2);

    //if(last_event == I2C_EVT_SLAVE_ACK_MISS)
    if(last_event & I2C_EVT_SLAVE_ACK_MISS)
    {
        I2C_ClrFlag(I2C2, I2C_FLAG_ACKFAIL);

        if(tx_num != 1)  //slave send the last data and recv NACK
        {
            flag_slave_complete = 1;
        }
        else //not the last data recv nack, send fail
        {
        }
    }
}

/**
*\*\name    IIC_RestoreSlaveByClock.
*\*\fun     Emulate 9 clock recovery slave by GPIO.
*\*\param   none
*\*\return  none
**/
void IIC_RestoreSlaveByClock(void)
{
    uint8_t i;
    GPIO_InitType i2cx_gpio;

    I2Cx_SCL_GPIO->POD |= (I2Cx_SCL_PIN );//pull up
    I2Cx_SDA_GPIO->POD |= (I2Cx_SDA_PIN);//pull up
    RCC_EnableAHB1PeriphClk(I2Cx_clk_en, ENABLE);
    I2Cx_SCL_GPIO->POD |= (I2Cx_SCL_PIN );//pull up
    I2Cx_SDA_GPIO->POD |= (I2Cx_SDA_PIN);//pull up

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&i2cx_gpio);

    i2cx_gpio.Pin        = I2Cx_SCL_PIN;
    i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SCL_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(I2Cx_SCL_GPIO, &i2cx_gpio);

    for (i = 0; i < 9; i++)
    {
        GPIO_SetBits(I2Cx_SCL_GPIO, I2Cx_SCL_PIN);
        systick_delay_us(5);
        GPIO_ResetBits(I2Cx_SCL_GPIO, I2Cx_SCL_PIN);
        systick_delay_us(5);
    }
}

/**
*\*\name    SystemNVICReset.
*\*\fun     System NVIC reset.
*\*\return  result
**/
void SystemNVICReset(void)
{
    __set_FAULTMASK((uint32_t)1);
    log_info("***** NVIC system reset! *****\r\n");
    __NVIC_SystemReset();
}

/**
*\*\name    IIC_RCCReset.
*\*\fun     RCC clock reset.
*\*\param   none
*\*\return  none
**/
void IIC_RCCReset(void)
{

    RCC_EnableAPB1PeriphReset(I2Cx_RCC);

    IIC_RestoreSlaveByClock();

    log_info("***** IIC module by RCC reset! *****\r\n");
    i2c_master_init();

}

/**
*\*\name    IIC_SWReset.
*\*\fun     I2c software reset.
*\*\param   none
*\*\return  none
**/
void IIC_SWReset(void)
{
    GPIO_InitType i2cx_gpio;

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&i2cx_gpio);

    i2cx_gpio.Pin        = I2Cx_SCL_PIN;
    i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SCL_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(I2Cx_SCL_GPIO, &i2cx_gpio);

    i2cx_gpio.Pin        = I2Cx_SDA_PIN;
    i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SDA_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_INPUT;
    GPIO_InitPeripheral(I2Cx_SDA_GPIO, &i2cx_gpio);

    I2CTimeout = I2CT_LONG_TIMEOUT;

    for (;;)
    {
        if ((I2Cx_SCL_PIN | I2Cx_SDA_PIN) == ((I2Cx_SCL_GPIO->PID & I2Cx_SCL_PIN) | (I2Cx_SDA_GPIO->PID & I2Cx_SDA_PIN)))
        {
            I2Cx->CTRL1 |= 0x1000;
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            I2Cx->CTRL1 &= ~0x1000;

            log_info("***** IIC module self reset! *****\r\n");
            break;
        }
        else
        {
            if ((I2CTimeout--) == 0)
            {
                IIC_RCCReset();
            }
        }
    }
}

/**
*\*\name    CommTimeOut_CallBack.
*\*\fun     Callback function.
*\*\param   none
*\*\return  none
**/
void CommTimeOut_CallBack(ErrCode_t errcode)
{
    log_info("...ErrCode:%d\r\n", errcode);
    Comm_Flag = C_READY;
    #if (COMM_RECOVER_MODE == MODULE_SELF_RESET)
    IIC_SWReset();
    #elif (COMM_RECOVER_MODE == MODULE_RCC_RESET)
    IIC_RCCReset();
    #elif (COMM_RECOVER_MODE == SYSTEM_NVIC_RESET)
    SystemNVICReset();
    #endif
}




