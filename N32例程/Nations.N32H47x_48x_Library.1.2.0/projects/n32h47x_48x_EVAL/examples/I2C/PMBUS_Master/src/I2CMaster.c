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

/***************************************************************************//**
 * @file       I2CMaster.c
 * @addtogroup I2CMaster
 * @{
 ******************************************************************************/
#include "n32h47x_48x_i2c.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "misc.h"
#include "I2CMaster.h"
#include "delay.h"
#include "log.h"


unsigned char *I2CMaster_ReceiveField;
unsigned char *I2CMaster_TransmitField;
static __IO uint32_t I2CTimeout;
uint8_t  flag_master_recv_finish = 0;
uint8_t  flag_master_send_finish = 0;
uint8_t  flag_trans_direct       = 0; // write
uint8_t  *I2CMaster_ReceiveBuf;
uint8_t  *I2CMaster_TransmitBuf;
uint8_t  I2CMaster_TransmitLen = 0;
uint8_t  I2CMaster_ReceiveLen = 0;
uint8_t  rx_num = 0;
uint8_t  tx_num = 0;
CommCtrl_t Comm_Flag = C_READY;
extern uint8_t PMBusMaster_CommandGroup;

void Memset(void* s, uint8_t c, uint32_t count);
void CommTimeOut_CallBack(ErrCode_t errcode);

/**
*\*\name    NVIC_ConfigurationMaster.
*\*\fun     NVIC Configuration.
*\*\param   none
*\*\return  result
**/
void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = I2Cx_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = I2Cx_ER_IRQn; // test err
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    i2c_master_init.
*\*\fun     master gpio/rcc/i2c/nvic initializes.
*\*\param   none
*\*\return  result
**/
int i2c_master_init(void)
{
    I2C_InitType i2cx_master;
    GPIO_InitType i2cx_gpio;
    RCC_EnableAPB1PeriphClk(I2Cx_RCC, ENABLE);
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    RCC_EnableAHB1PeriphClk(I2Cx_clk_en, ENABLE);
    I2Cx_SCL_GPIO->POD |= (I2Cx_SCL_PIN );//pull up
    I2Cx_SDA_GPIO->POD |= (I2Cx_SDA_PIN);//pull up

    /* Initialize GPIO_InitStructure */
    GPIO_InitStruct(&i2cx_gpio);

    i2cx_gpio.Pin            = I2Cx_SCL_PIN ;
    i2cx_gpio.GPIO_Pull      = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SCL_AF;
    i2cx_gpio.GPIO_Mode      = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SCL_GPIO, &i2cx_gpio);

    i2cx_gpio.Pin            = I2Cx_SDA_PIN;
    i2cx_gpio.GPIO_Pull      = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SDA_AF;
    i2cx_gpio.GPIO_Mode      = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SDA_GPIO, &i2cx_gpio);

    I2C_DeInit(I2Cx);
    I2C_InitStruct(&i2cx_master); 
    i2cx_master.BusMode      = I2C_BUSMODE_I2C;
    i2cx_master.FmDutyCycle  = I2C_FMDUTYCYCLE_2;
    i2cx_master.OwnAddr1     = I2C_MASTER_ADDR;
    i2cx_master.AckEnable    = I2C_ACKEN;
    i2cx_master.AddrMode     = I2C_ADDR_MODE_7BIT;
    i2cx_master.ClkSpeed     = 100000; // 100000 100K

    I2C_Init(I2Cx, &i2cx_master);

    // int enable
    I2C_ConfigInt(I2Cx, I2C_INT_EVENT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
    NVIC_Configuration();
    I2C_Enable(I2Cx, ENABLE);
    return 0;
}

/**
*\*\name    I2C1_EV_IRQHandler.
*\*\fun     i2c event Interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2Cx_EV_IRQHandler(void)
{
    unsigned int last_event = 0;

    last_event = I2C_GetLastEvent(I2Cx);
    if ((last_event & I2C_ROLE_MASTER) == I2C_ROLE_MASTER) // master mode
    {
        switch (last_event)
        {
        case I2C_EVT_MASTER_MODE_FLAG: // EV5 start
            if (flag_trans_direct)     // read
            {
                I2C_SendAddr7bit(I2Cx, I2C_SLAVE_ADDR, I2C_DIRECTION_RECV);
                rx_num = 0;
            }
            else // write
            {
                I2C_SendAddr7bit(I2Cx, I2C_SLAVE_ADDR, I2C_DIRECTION_SEND);
                tx_num = 0;
            }
            break;
        case I2C_EVT_MASTER_TXMODE_FLAG: // EV8 justafter EV6
            Comm_Flag = C_READY;
            I2C_SendData(I2Cx, *I2CMaster_TransmitBuf++);
            tx_num++;
            break;
        case I2C_EVT_MASTER_DATA_SENDING: //  transmitting data
            if (tx_num < I2CMaster_TransmitLen)
            {
                I2C_SendData(I2Cx, *I2CMaster_TransmitBuf++);
                tx_num++;
            }
            break;
        case I2C_EVT_MASTER_DATA_SENDED: // byte data send finish
            // bit2    BSF (Byte transfer finished)
            if (tx_num == I2CMaster_TransmitLen)  // data send finish
            {
                if (Comm_Flag == C_READY)
                {
                    if((PMBusMaster_CommandGroup == 0/*READBYTE*/) || (PMBusMaster_CommandGroup == 2/*READWORD*/))
                    {
                        I2C_ConfigAck(I2Cx, ENABLE);
                        Comm_Flag = C_START_BIT;
                        flag_trans_direct = 1;
                        I2Cx->CTRL1 &= I2C_NACK_POS_CURRENT; // reset ACKPOS
                        I2C_GenerateStart(I2Cx, ENABLE);
                    }
                    else
                    {
                        Comm_Flag = C_STOP_BIT;
                        I2C_GenerateStop(I2Cx, ENABLE);
                    }
                    flag_master_send_finish = 1;
                }
            }
            break;
        // Master Receiver
        case I2C_EVT_MASTER_RXMODE_FLAG: // EV6
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
        case I2C_EVT_MASTER_DATA_RECVD_FLAG: // one byte recved :EV7.//BUSY,MSL and RXDATNE flags.
            *I2CMaster_ReceiveBuf++ = I2C_RecvData(I2Cx);
            rx_num++;

            if (rx_num == (I2CMaster_ReceiveLen - 1))
            {
                I2C_ConfigAck(I2Cx, DISABLE);   // Disable I2Cx acknowledgement.
                if (Comm_Flag == C_READY)
                {
                    Comm_Flag = C_STOP_BIT;
                    I2C_GenerateStop(I2Cx, ENABLE); // Send I2Cx STOP Condition.
                }
            }
            else if (rx_num == I2CMaster_ReceiveLen)
            {
                flag_master_recv_finish = 1;
            }
            break;
        case 0x00030201: // Arbitration lost
        case 0x00030401: // Acknowledge failure
        case 0x00030501: // Acknowledge failure and Bus error
            I2C_Enable(I2Cx, DISABLE);
            I2C_Enable(I2Cx, ENABLE);
            break;
        default:
            //log_info("I2C error status:0x%x\r\n", last_event);
            break;
        }
    }
}

/**
*\*\name    I2CMaster_Transmit.
*\*\fun     Performs transmit and receive transactions in master mode
*\*\param   I2CMaster_ByteCountTx The number of bytes to transmit.
*\*\param   *I2CMaster_TxArray Pointer to the data to transmit.
*\*\param   I2CMaster_ByteCountRx The number of bytes to receive.
*\*\param   *I2CMaster_RxArray Pointer to structure to store received data.
*\*\return  none
**/
void I2CMaster_Transmit(uint16_t I2CMaster_ByteCountTx, uint8_t*I2CMaster_TxArray, uint16_t I2CMaster_ByteCountRx, uint8_t *I2CMaster_RxArray)
{
    /***************Write******************/
    if(I2CMaster_ByteCountTx != 0)
    {
        //i2c_master_send(I2CMaster_TxArray, I2CMaster_ByteCountTx);
        I2CMaster_TransmitBuf = I2CMaster_TxArray;
        I2CMaster_TransmitLen = I2CMaster_ByteCountTx;
        flag_trans_direct = 0;
        if (Comm_Flag == C_READY)
        {
            Comm_Flag = C_START_BIT;
            I2C_GenerateStart(I2Cx, ENABLE);
        }

        I2CTimeout = I2CT_LONG_TIMEOUT * 1000;
        while (flag_master_send_finish == 0)
        {
            if ((I2CTimeout--) == 0)
            {
                CommTimeOut_CallBack(MASTER_UNKNOW);
            }
        }

        /* master send finish */
        flag_master_send_finish = 0;
        Comm_Flag = C_READY;
        I2C_ConfigAck(I2Cx, ENABLE);
    }
    if(I2CMaster_ByteCountRx != 0)
    {
        I2CMaster_ReceiveBuf = I2CMaster_RxArray;
        I2CMaster_ReceiveLen = I2CMaster_ByteCountRx;
        I2CTimeout = I2CT_LONG_TIMEOUT * 1000;
        while (flag_master_recv_finish == 0)
        {
            if ((I2CTimeout--) == 0)
            {
                CommTimeOut_CallBack(MASTER_UNKNOW);
            }
        }
        /* master recv finish */
        flag_master_recv_finish = 0;
        Comm_Flag = C_READY;
    }
}

/**
*\*\name    I2C1_ER_IRQHandler.
*\*\fun     i2c error Interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2Cx_ER_IRQHandler(void)
{
    if(I2C_GetFlag(I2Cx, I2C_FLAG_ACKFAIL))
    {
        I2C_ClrFlag(I2Cx, I2C_FLAG_ACKFAIL);
        I2C_GenerateStop(I2Cx, ENABLE); // Send I2Cx STOP Condition.
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
*\*\fun     System software reset.
*\*\param   none
*\*\return  none
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

/***************************************************************************//**
 * @brief   See if the I2C master is ready to communicate.
 * @param   None
 * @return  Busy Bit (Busy = 1, Free = 0)
 ******************************************************************************/
uint8_t I2CMaster_NotReady(void)
{
    if(I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
        return 1;
    else
        return 0;
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

