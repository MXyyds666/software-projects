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
*\*\file PMBusSlave.c
*\*\author Nations
*\*\version v1.0.0
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "PMBusSlave.h"
#include "PMBus.h"
#include "I2CSlave.h"

struct STATUS_REGS StatusRegs;

#define I2CT_FLAG_TIMEOUT  ((uint32_t)0x5000)
#define I2CT_LONG_TIMEOUT  ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
#define I2C_SLAVE_ADDR     0x10

static __IO uint32_t I2CTimeout;
uint8_t flag_slave_recv_finish     = 0;
uint8_t flag_slave_send_finish     = 0;
uint8_t pmbus_tx_num               = 0;

#if defined (N32H475)
    #define I2Cx I2C1
    #define I2Cx_RCC         RCC_APB1_PERIPH_I2C1
    #define I2Cx_SCL_PIN     GPIO_PIN_11
    #define I2Cx_SDA_PIN     GPIO_PIN_1
    #define I2Cx_SCL_GPIO    GPIOD
    #define I2Cx_SDA_GPIO    GPIOD
    #define I2Cx_SCL_AF      GPIO_AF5
    #define I2Cx_SDA_AF      GPIO_AF7

    #define I2Cx_clk_en      RCC_AHB_PERIPHEN_GPIOD

    #define I2Cx_EV_IRQn  			I2C1_EV_IRQn
    #define I2Cx_ER_IRQn  			I2C1_ER_IRQn
    #define I2Cx_EV_IRQHandler  I2C1_EV_IRQHandler
    #define I2Cx_ER_IRQHandler  I2C1_ER_IRQHandler
#else

    #define I2Cx I2C3
    #define I2Cx_RCC            RCC_APB1_PERIPH_I2C3

    #define I2Cx_SCL_PIN        GPIO_PIN_0
    #define I2Cx_SDA_PIN        GPIO_PIN_5
    #define I2Cx_SCL_GPIO       GPIOC
    #define I2Cx_SDA_GPIO       GPIOB
    #define I2Cx_SCL_AF         GPIO_AF8
    #define I2Cx_SDA_AF         GPIO_AF11

    #define I2Cx_clk_en         RCC_AHB_PERIPHEN_GPIOB|RCC_AHB_PERIPHEN_GPIOC

    #define I2Cx_EV_IRQn  	    I2C3_EV_IRQn
    #define I2Cx_ER_IRQn  		  I2C3_ER_IRQn
    #define I2Cx_EV_IRQHandler  I2C3_EV_IRQHandler
    #define I2Cx_ER_IRQHandler  I2C3_ER_IRQHandler
#endif

#define PMBUS_CONTRL_PIN    GPIO_PIN_7
#define PMBUS_ALERT_PIN     GPIO_PIN_8
#define PMBUS_CONTRL_GPIO   GPIOA
#define PMBUS_ALERT_GPIO    GPIOA


/* This is the PMBus command look-up table. Do not modify---------------------*/
const unsigned char PMBus_Commands[120] =
{
    0x00, // dummy byte
    0x19, 0x78, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x98,
    0x79, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
    0x93, 0x94, 0x95, 0x96, 0x97, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
    0xA7, 0xA8, 0xA9, 0x13, 0x14, 0x17, 0x18, 0x03, 0x11, 0x12, 0x15, 0x16,
    0x00, 0x01, 0x02, 0x04, 0x10, 0x20, 0x3A, 0x3D, 0x41, 0x45, 0x47, 0x49,
    0x4C, 0x50, 0x54, 0x56, 0x5A, 0x5C, 0x63, 0x69, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x31, 0x32, 0x33, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3B, 0x3C, 0x3E, 0x3F, 0x40, 0x42, 0x43, 0x44, 0x46, 0x48,
    0x4A, 0x4B, 0x4F, 0x51, 0x52, 0x53, 0x55, 0x57, 0x58, 0x59, 0x5B, 0x5D,
    0x5E, 0x5F, 0x60, 0x61, 0x62, 0x64, 0x65, 0x66, 0x68, 0x6A, 0x6B
};

static uint8_t i2c_rcv_cnt = 0;
static uint8_t i2c_transmit_cnt = 0;

uint8_t  PMBusSlave_Command = 0;
uint8_t  PMBusSlave_CommandType = 0;
uint8_t  PMBusSlave_CrcMsgSize = 0;
uint8_t  PMBusSlave_CrcMsg[5];
uint8_t  PMBusSlave_CrcMasterGenerated = 0;
uint8_t  PMBusSlave_CrcSlaveGenerated = 0;

uint8_t  PMBusSlave_ReceiveBuffer[4]  = {0, 0, 0, 0};
uint8_t  PMBusSlave_TransmitBuffer[5] = {0xDF, 0xC5, 0x18, 0x9F, 0xB7};
uint8_t  PMBusSlave_Index = 0;
uint8_t  PMBusSlave_DummyCommand = 0;

//initial values
volatile uint8_t  Temperature  = 0x12;	    //STATUS_TEMPERATURE command (R byte)
volatile uint8_t  Default_Code = 0x13;		    //STORE_DEFAULT_CODE command (W byte)
volatile uint8_t  Operation    = 0x34;		  //OPERATION command (R/W byte)
volatile uint16_t Status_Word  = 0x5678;  	//STATUS_WORD command (R word)
volatile uint8_t  Status_Byte  = 0xA1;	    //STATUS_BYTE command (R byte)
volatile uint8_t  Status_Cml   = 0xA2;	    //STATUS_CML command (R byte)
volatile uint16_t Vout_Command = 0x90AB;	  //VOUT_COMMAND command (R/W word)

/**
*\*\name    NVIC_ConfigurationSlave.
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

    NVIC_InitStructure.NVIC_IRQChannel = I2Cx_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
*\*\name    i2c_slave_init.
*\*\fun     slave gpio/rcc/i2c/nvic initializes.
*\*\param   none
*\*\return  result
**/
int i2c_slave_init(void)
{
    I2C_InitType i2cx_slave;
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
    I2C_InitStruct(&i2cx_slave);
    i2cx_slave.BusMode       = I2C_BUSMODE_I2C;
    i2cx_slave.FmDutyCycle   = I2C_FMDUTYCYCLE_2;
    i2cx_slave.OwnAddr1      = I2C_SLAVE_ADDR;
    i2cx_slave.AckEnable     = I2C_ACKEN;
    i2cx_slave.AddrMode      = I2C_ADDR_MODE_7BIT;
    i2cx_slave.ClkSpeed      = 100000; // 100000 100K

    I2C_Init(I2Cx, &i2cx_slave);

    // int enable
    I2C_ConfigInt(I2Cx, I2C_INT_EVENT | I2C_INT_BUF | I2C_INT_ERR, ENABLE);
    NVIC_Configuration();
    I2C_Enable(I2Cx, ENABLE);
    return 0;
}

/**
*\*\name    PMBusSlave_Init.
*\*\fun     Configure the N32H47x_48x device as a PMBus slave.
*\*\param   none
*\*\return  none
**/
void PMBusSlave_Init(void)
{
    GPIO_InitType pmbus_gpio;
    StatusRegs.StatusWord.all = 0;			//Clear status bits for the status registers we are using
    StatusRegs.StatusCml.all  = 0;

    // Control Line functionality GPIO0
    pmbus_gpio.Pin            = PMBUS_CONTRL_PIN ;
    pmbus_gpio.GPIO_Pull      = GPIO_PULL_UP;
    pmbus_gpio.GPIO_Alternate = GPIO_NO_AF;
    pmbus_gpio.GPIO_Mode      = GPIO_MODE_AF_PP;
    pmbus_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(PMBUS_CONTRL_GPIO, &pmbus_gpio);

    pmbus_gpio.Pin            = PMBUS_ALERT_PIN;
    pmbus_gpio.GPIO_Pull      = GPIO_PULL_UP;
    pmbus_gpio.GPIO_Alternate = GPIO_NO_AF;
    pmbus_gpio.GPIO_Mode      = GPIO_MODE_AF_PP;
    pmbus_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(PMBUS_ALERT_GPIO, &pmbus_gpio);

    i2c_slave_init();     // Initialize i2c
}

/**
*\*\name    PMBusSlave_DecodeCommand.
*\*\fun     Determine what type of PMBus command was received from the master.
*           The function also prepares data in the transmit buffer to send to
*           the master for supported READ and READ/WRITE commands. Users should modify
*           the code to implement their application's supported PMBus commands.
*\*\param   PMBusSlave_RxCommand The command byte received from the master.
*\*\return  Command group of the received command.
**/
uint8_t PMBusSlave_DecodeCommand(uint8_t PMBusSlave_RxCommand)
{
    uint8_t PMBusSlave_CommandGroup;

    for(PMBusSlave_Index = 0; PMBusSlave_Index < 120; PMBusSlave_Index++)
    {
        if(PMBus_Commands[PMBusSlave_Index] == PMBusSlave_RxCommand)
            break;
    }

    if(PMBusSlave_Index == 0)//dummy byte to check if slave is present
        PMBusSlave_DummyCommand = 1;

    if(PMBusSlave_Index > 0  && PMBusSlave_Index < 13)    //read byte from slave device
        PMBusSlave_CommandGroup = READBYTE;

    if(PMBusSlave_Index > 12 && PMBusSlave_Index < 40)    //read word from slave device
        PMBusSlave_CommandGroup = READWORD;

    if(PMBusSlave_Index > 39 && PMBusSlave_Index < 44)    //write byte to slave device
        PMBusSlave_CommandGroup = WRITEBYTE;

    if(PMBusSlave_Index > 43 && PMBusSlave_Index < 49)    //send byte to slave device
        PMBusSlave_CommandGroup = SENDBYTE;

    if(PMBusSlave_Index > 48 && PMBusSlave_Index < 69)    //Read or write one byte of data. R/W oprn. decided based on RWFlag
        PMBusSlave_CommandGroup = RWBYTE;

    if(PMBusSlave_Index > 68 && PMBusSlave_Index < 120)   //Read or write one word of data. R/W oprn. decided based on RWFlag
        PMBusSlave_CommandGroup = RWWORD;

    if(PMBusSlave_Index >= 120)                           //illegal index - invalid command trap
        while(1);

    //===============END USER CODE===================//
    //======Add code according to user needs=========//
    switch (PMBusSlave_Index)	//should include all user supported commands
    {
        case STATUS_TEMPERATURE:
            PMBusSlave_TransmitBuffer[0] = Temperature;
            break;

        case STORE_DEFAULT_CODE:
            //only write
            break;

        case OPERATION:
            PMBusSlave_TransmitBuffer[0] = Operation;
            break;

        case STATUS_WORD:
            PMBusSlave_TransmitBuffer[0] = Status_Word;	      //lower byte
            PMBusSlave_TransmitBuffer[1] = Status_Word >> 8;	//upper byte
            break;

        case VOUT_COMMAND:
            PMBusSlave_TransmitBuffer[0] = Vout_Command;	    //lower byte
            PMBusSlave_TransmitBuffer[1] = Vout_Command >> 8;	//upper byte
            break;

        default:
            PMBusSlave_DummyCommand = 1;	                    //command not supported by this slave

            if((PMBusSlave_CommandGroup == READBYTE) || (PMBusSlave_CommandGroup == RWBYTE))
                PMBusSlave_TransmitBuffer[0] = 0x00;
            else
            {
                PMBusSlave_TransmitBuffer[0] = 0x00;
                PMBusSlave_TransmitBuffer[1] = 0x00;
            }

            break;
    }

    //===============END USER CODE===================//
    return PMBusSlave_CommandGroup;
}

/**
*\*\name    PMBusSlave_ProcessWriteByte.
*\*\fun     PMBUS slave process master wtite byte command.
*
*\*\param   PMBusSlave_RxCommand
*\*\return  none
**/
void PMBusSlave_ProcessWriteByte(uint8_t PMBusSlave_RxCommand)
{
    //================Add code according to user needs=======================//
    switch (PMBusSlave_RxCommand)	//should include all user supported commands
    {
        case PMBC_STORE_DEFAULT_CODE:
            Default_Code = PMBusSlave_ReceiveBuffer[1];   //PMBusSlave_TransmitBuffer[1];
            break;

        case PMBC_OPERATION:
            Operation =   PMBusSlave_ReceiveBuffer[1];  // PMBusSlave_TransmitBuffer[1];
            break;

        default:
            PMBusSlave_DummyCommand = 1;	                    //command not supported by this slave
            break;
    }
}

/**
*\*\name    PMBusSlave_ProcessWriteWord.
*\*\fun     PMBUS slave process master wtite word command.
*
*\*\param   PMBusSlave_RxCommand
*\*\return  none
**/
void PMBusSlave_ProcessWriteWord(uint8_t PMBusSlave_RxCommand)
{
    //================Add code according to user needs=======================//
    switch (PMBusSlave_RxCommand)	//should include all user supported commands
    {
        case PMBC_VOUNT_COMMAND:
            Vout_Command = (PMBusSlave_ReceiveBuffer[2] << 8) | PMBusSlave_ReceiveBuffer[1];
            break;

        default:
            PMBusSlave_DummyCommand = 1;	                    //command not supported by this slave
            break;
    }
}

/**
*\*\name    PMBusSlave_ProcessMasterReadCommand.
*\*\fun     Receives a command from the master and performs reads accordingly.
* 			    This function should be modified by the user for their application's
* 			    supported commands to make function calls to carry out the commands
* 			    (e.g. adjusting fan speed)
*\*\param   none
*\*\return  none
**/
void PMBusSlave_ProcessMasterReadCommand(void)
{
    PMBusSlave_ReceiveBuffer[0] = I2C_RecvData(I2Cx);
    PMBusSlave_Command = PMBusSlave_ReceiveBuffer[0];	//get register value received prior to this step
    PMBusSlave_CommandType = PMBusSlave_DecodeCommand(PMBusSlave_Command);

    switch(PMBusSlave_CommandType)
    {
        //Implementation with Packet Error Checking
        case READBYTE: // master reading byte from slave
            //setup PEC byte
            i2c_transmit_cnt = 0;
            PMBusSlave_CrcMsg[1] = PMBusSlave_Command;  			    // store first rx byte
            PMBusSlave_CrcMsg[2] = I2C_SLAVE_ADDR | 0x01;		      // store slave address + R/W=1
            PMBusSlave_CrcMsg[3] = PMBusSlave_TransmitBuffer[0];	// store tx byte 1
            PMBusSlave_CrcMsgSize = 4;
            PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);
            PMBusSlave_TransmitBuffer[1] = PMBusSlave_CrcSlaveGenerated;
            pmbus_tx_num = 2;
            I2C_SendData(I2Cx, PMBusSlave_TransmitBuffer[i2c_transmit_cnt]);
            i2c_transmit_cnt++;
            break;

        case READWORD:  // master reading word from slave
            //setup PEC byte
            i2c_transmit_cnt = 0;
            PMBusSlave_CrcMsg[1] = PMBusSlave_Command;  			   // store first rx byte
            PMBusSlave_CrcMsg[2] = I2C_SLAVE_ADDR | 0x01;		     // store slave address + R/W=1
            PMBusSlave_CrcMsg[3] = PMBusSlave_TransmitBuffer[0]; // store tx byte 1
            PMBusSlave_CrcMsg[4] = PMBusSlave_TransmitBuffer[1]; // store tx byte 2
            PMBusSlave_CrcMsgSize = 5;
            PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);
            PMBusSlave_TransmitBuffer[2] = PMBusSlave_CrcSlaveGenerated;
            pmbus_tx_num = 3;
            I2C_SendData(I2Cx, PMBusSlave_TransmitBuffer[i2c_transmit_cnt]);
            i2c_transmit_cnt++;
            break;

        case RWWORD:	//read/write word
            //setup PEC byte
            i2c_transmit_cnt = 0;
            PMBusSlave_CrcMsg[1] = PMBusSlave_Command;  			   // store first rx byte
            PMBusSlave_CrcMsg[2] = I2C_SLAVE_ADDR | 0x01;		     // store slave address + R/W=1
            PMBusSlave_CrcMsg[3] = PMBusSlave_TransmitBuffer[0]; // store tx byte 1
            PMBusSlave_CrcMsg[4] = PMBusSlave_TransmitBuffer[1]; // store tx byte 2
            PMBusSlave_CrcMsgSize = 5;
            PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);
            PMBusSlave_TransmitBuffer[2] = PMBusSlave_CrcSlaveGenerated;
            pmbus_tx_num = 3;
            I2C_SendData(I2Cx, PMBusSlave_TransmitBuffer[i2c_transmit_cnt]);
            i2c_transmit_cnt++;
            break;

        case RWBYTE: // master reading byte from slave
            //setup PEC byte
            i2c_transmit_cnt = 0;
            PMBusSlave_CrcMsg[1] = PMBusSlave_Command;  			    // store first rx byte
            PMBusSlave_CrcMsg[2] = I2C_SLAVE_ADDR | 0x01;		      // store slave address + R/W=1
            PMBusSlave_CrcMsg[3] = PMBusSlave_TransmitBuffer[0];	// store tx byte 1
            PMBusSlave_CrcMsgSize = 4;
            PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);
            PMBusSlave_TransmitBuffer[1] = PMBusSlave_CrcSlaveGenerated;
            pmbus_tx_num = 2;
            I2C_SendData(I2Cx, PMBusSlave_TransmitBuffer[i2c_transmit_cnt]);
            i2c_transmit_cnt++;
            break;

        default:  //used to send error message if the command can't be recognized
            //setup PEC byte
            i2c_transmit_cnt = 0;
            PMBusSlave_TransmitBuffer[0] = 0xFF;	               // lower byte
            PMBusSlave_TransmitBuffer[1] = 0xEE;	               // upper byte
            PMBusSlave_CrcMsg[1] = PMBusSlave_Command;  			   // store first rx byte
            PMBusSlave_CrcMsg[2] = I2C_SLAVE_ADDR | 0x01;		     // store slave address + R/W=1
            PMBusSlave_CrcMsg[3] = PMBusSlave_TransmitBuffer[0]; // store tx byte 1
            PMBusSlave_CrcMsg[4] = PMBusSlave_TransmitBuffer[1]; // store tx byte 2
            PMBusSlave_CrcMsgSize = 5;
            PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);
            PMBusSlave_TransmitBuffer[2] = PMBusSlave_CrcSlaveGenerated;
            pmbus_tx_num = 3;
            I2C_SendData(I2Cx, PMBusSlave_TransmitBuffer[i2c_transmit_cnt]);
            i2c_transmit_cnt++;
            break;
    }
}

/***************************************************************************//**
 * @brief   Receives a command from the master and performs writes accordingly.
 *
 * 			    This function should be modified by the user for their application's
 * 			    supported commands to make function calls to carry out the commands
 * 			    (e.g. adjusting fan speed)
 * @param   None
 * @return  None
 ******************************************************************************/
void PMBusSlave_ProcessMasterWriteCommand(void)
{
    if(i2c_rcv_cnt > 0)
    {
        PMBusSlave_Command = PMBusSlave_ReceiveBuffer[0];	                     //get the received data
        PMBusSlave_CommandType = PMBusSlave_DecodeCommand(PMBusSlave_Command);

        switch(PMBusSlave_CommandType)
        {
            case WRITEWORD:                                                        // master writing word to slave
                PMBusSlave_CrcMasterGenerated = PMBusSlave_ReceiveBuffer[3];	     //get the received PEC byte from master
                //calculate PEC byte for comparison
                PMBusSlave_CrcMsg[1] = PMBusSlave_Command;			    // store first rx byte
                PMBusSlave_CrcMsg[2] = PMBusSlave_ReceiveBuffer[1];	// store rx byte
                PMBusSlave_CrcMsg[3] = PMBusSlave_ReceiveBuffer[2];	// store rx byte
                PMBusSlave_CrcMsgSize = 4;                 			    // bytes num
                PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);

                //if the bytes do not match, respond according to the PMBus Spec.
                if(PMBusSlave_CrcSlaveGenerated != PMBusSlave_CrcMasterGenerated)	 //compare received PEC byte with calculated PEC byte
                {
                    //Flush the received command byte and any received data
                    PMBusSlave_Command = 0;				      //flush command byte
                    PMBusSlave_Index   = 0;				      //clear index so no data is stored
                    PMBusSlave_ReceiveBuffer[0] = 0;	  //flush received data bytes
                    PMBusSlave_ReceiveBuffer[1] = 0;
                    PMBusSlave_ReceiveBuffer[2] = 0;
                    PMBusSlave_ReceiveBuffer[3] = 0;
                    //ERROR process if needed
                }
                else
                {
                    PMBusSlave_ProcessWriteWord(PMBusSlave_Command);
                }

                break;

            case RWWORD:	//read/write word
                //slave receiver
                PMBusSlave_CrcMasterGenerated = PMBusSlave_ReceiveBuffer[3];//get the received PEC byte from master

                //calculate PEC byte for comparison
                PMBusSlave_CrcMsg[1] = PMBusSlave_Command;			    // store first rx byte
                PMBusSlave_CrcMsg[2] = PMBusSlave_ReceiveBuffer[1];	// store rx byte
                PMBusSlave_CrcMsg[3] = PMBusSlave_ReceiveBuffer[2];	// store rx byte
                PMBusSlave_CrcMsgSize = 4;                 			    // of bytes
                PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);

                //if the bytes do not match, respond according to the PMBus Spec.
                if(PMBusSlave_CrcSlaveGenerated != PMBusSlave_CrcMasterGenerated)	//compare received PEC byte with calculated PEC byte
                {
                    //Flush the received command byte and any received data
                    PMBusSlave_Command = 0;				    //flush command byte
                    PMBusSlave_Index   = 0;				    //clear index so no data is stored
                    PMBusSlave_ReceiveBuffer[0] = 0;	//flush received data bytes
                    PMBusSlave_ReceiveBuffer[1] = 0;
                    PMBusSlave_ReceiveBuffer[2] = 0;
                    PMBusSlave_ReceiveBuffer[3] = 0;
                    //ERROR process if needed
                }
                else//TODO according to the command
                {
                    PMBusSlave_ProcessWriteWord(PMBusSlave_Command);
                }

                break;

            case WRITEBYTE: // master writing single byte to slave
                PMBusSlave_CrcMasterGenerated = PMBusSlave_ReceiveBuffer[2];	//get the received PEC byte from master
                //calculate PEC byte for comparison
                PMBusSlave_CrcMsg[1] = PMBusSlave_Command;			    // store first rx byte
                PMBusSlave_CrcMsg[2] = PMBusSlave_ReceiveBuffer[1];	// store rx byte
                PMBusSlave_CrcMsgSize = 3;                 			    // bytes num
                PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);

                //if the bytes do not match, respond according to the PMBus Spec.
                if(PMBusSlave_CrcSlaveGenerated != PMBusSlave_CrcMasterGenerated)	//compare received PEC byte with calculated PEC byte
                {
                    //Flush the received command byte and any received data
                    PMBusSlave_Command = 0;				    //flush command byte
                    PMBusSlave_Index = 0;				      //clear index so no data is stored
                    PMBusSlave_ReceiveBuffer[0] = 0;	//flush received data bytes
                    PMBusSlave_ReceiveBuffer[1] = 0;
                    PMBusSlave_ReceiveBuffer[2] = 0;
                    //ERROR process if needed
                }
                else
                {
                    PMBusSlave_ProcessWriteByte(PMBusSlave_Command);
                }

                break;

            case RWBYTE:	//read/write byte
                //slave receiver
                PMBusSlave_CrcMasterGenerated = PMBusSlave_ReceiveBuffer[2];//get the received PEC byte from master

                //calculate PEC byte for comparison
                PMBusSlave_CrcMsg[1] = PMBusSlave_Command;			    // store first rx byte
                PMBusSlave_CrcMsg[2] = PMBusSlave_ReceiveBuffer[1];	// store rx byte

                PMBusSlave_CrcMsgSize = 3;                 			    // of bytes
                PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);

                //if the bytes do not match, respond according to the PMBus Spec.
                if(PMBusSlave_CrcSlaveGenerated != PMBusSlave_CrcMasterGenerated)	//compare received PEC byte with calculated PEC byte
                {
                    //Flush the received command byte and any received data
                    PMBusSlave_Command = 0;				    //flush command byte
                    PMBusSlave_Index   = 0;				    //clear index so no data is stored
                    PMBusSlave_ReceiveBuffer[0] = 0;	//flush received data bytes
                    PMBusSlave_ReceiveBuffer[1] = 0;
                    PMBusSlave_ReceiveBuffer[2] = 0;
                    PMBusSlave_ReceiveBuffer[3] = 0;
                    //ERROR process if needed
                }
                else//TODO according to the command
                {
                    PMBusSlave_ProcessWriteByte(PMBusSlave_Command);
                }

                break;

            case SENDBYTE:  	//we already received the command.
                //calculate PEC byte for comparison
                PMBusSlave_CrcMsg[1] = PMBusSlave_Command;	// store first rx byte
                PMBusSlave_CrcMsgSize = 2;                 	// # of bytes
                PMBusSlave_CrcSlaveGenerated = PMBusSlave_Crc8MakeBitwise(CRC8_INIT_REM, CRC8_POLY, PMBusSlave_CrcMsg, PMBusSlave_CrcMsgSize);

                //if the bytes do not match, respond according to the PMBus Spec.
                if(PMBusSlave_CrcSlaveGenerated != PMBusSlave_CrcMasterGenerated)	//compare received PEC byte with calculated PEC byte
                {
                    //Flush the received command byte
                    PMBusSlave_Command = 0;			//flush command byte
                    PMBusSlave_Index = 0;			//clear index so no data is stored
                }
                else
                {
                    PMBusSlave_ProcessWriteByte(PMBusSlave_Command);
                }

                break;	//User could insert handling code here.

            default:
                break;
        }

        i2c_rcv_cnt = 0;
    }
}

/**
*\*\name    I2C1_EV_IRQHandler.
*\*\fun     i2c Interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2Cx_EV_IRQHandler(void)
{
    uint32_t last_event = 0;

    PMBusSlave_CrcMsg[0] = I2C_SLAVE_ADDR;

    last_event = I2C_GetLastEvent(I2Cx);

    if ((last_event & I2C_ROLE_MASTER) != I2C_ROLE_MASTER) // MSMODE = 0:I2C slave mode
    {
        switch (last_event)
        {
            case I2C_EVT_SLAVE_RECV_ADDR_MATCHED:  //Master has sent the slave address(w) and the slave prepares to receive data from the master
                // clear flag,ready to receive data
                i2c_rcv_cnt = 0;
                break;

            case I2C_EVT_SLAVE_DATA_RECVD:
                PMBusSlave_ReceiveBuffer[i2c_rcv_cnt++] = I2C_RecvData(I2Cx);
                break;

            case I2C_EVT_SLAVE_SEND_ADDR_MATCHED:  //Master has sent the slave address(R) and the slave prepares to send data to the master
                i2c_transmit_cnt = 0;
                pmbus_tx_num = 0;
                PMBusSlave_ProcessMasterReadCommand();
                break;

            case I2C_EVT_SLAVE_DATA_SENDING:
            case I2C_EVT_SLAVE_DATA_SENDED:
                if(i2c_transmit_cnt < pmbus_tx_num)
                {
                    I2C_SendData(I2Cx, PMBusSlave_TransmitBuffer[i2c_transmit_cnt++]);
                }
                else
                {
                    ;
                }

                break;

            case I2C_EVT_SLAVE_STOP_RECVD:
                I2C_Enable(I2Cx, ENABLE);

                if(i2c_rcv_cnt != 0)
                {
                    flag_slave_recv_finish = 1; // The STOPF bit is not set after a NACK reception
                }

                PMBus_ClearFlag();
                break;

            default:
                I2C_Enable(I2Cx, ENABLE);
                break;
        }
    }
}

/**
*\*\name    I2C1_ER_IRQHandler.
*\*\fun     i2c error interrupt service function.
*\*\param   none
*\*\return  none
**/
void I2Cx_ER_IRQHandler(void)
{
    uint32_t last_event;
    last_event = I2C_GetLastEvent(I2Cx);

    if(last_event & I2C_EVT_SLAVE_ACK_MISS)
    {
        I2C_ClrFlag(I2Cx, I2C_FLAG_ACKFAIL);

        if(i2c_transmit_cnt != 0)
            flag_slave_send_finish = 1;
    }
}

void PMBus_ClearFlag(void)
{
    /* ADDR Flag clear */
    while(I2C_GetIntStatus(I2Cx, I2C_INT_ADDRF))
    {
        I2C_ClrIntPendingBit(I2Cx, I2C_INT_ADDRF);
    }

    /* STOPF Flag clear */
    while(I2C_GetIntStatus(I2Cx, I2C_INT_STOPF))
    {
        I2C_ClrIntPendingBit(I2Cx, I2C_INT_STOPF);
    }
}

#if PEC
/***************************************************************************//**
 * @brief   Calculate the Packet Error Checking byte.
 * @param   PMBusSlave_CRC Initial value.
 * @param	  PMBusSlave_Poly The polynomial to use for the calculation.
 * @param	  *PMBusSlave_PMsg Pointer to the bytes from the PMBus transaction.
 * @param	  PMBusSlave_MsgSize Number of bytes in the last transaction.
 * @return  The PEC byte.
 ******************************************************************************/
uint8_t PMBusSlave_Crc8MakeBitwise(uint8_t PMBusSlave_CRC, uint8_t PMBusSlave_Poly, uint8_t *PMBusSlave_Pmsg, uint16_t PMBusSlave_MsgSize)
{
    unsigned int i, j, carry;
    unsigned char msg;

    PMBusSlave_CRC = *PMBusSlave_Pmsg++;        	  // first byte loaded in "crc"

    for(i = 0 ; i < PMBusSlave_MsgSize - 1 ; i ++)
    {
        msg = *PMBusSlave_Pmsg++;                   // next byte loaded in "msg"

        for(j = 0 ; j < 8 ; j++)
        {
            carry = PMBusSlave_CRC & 0x80;               			      // check if MSB=1
            PMBusSlave_CRC = (PMBusSlave_CRC << 1) | (msg >> 7);    // Shift 1 bit of next byte into crc

            if(carry) PMBusSlave_CRC ^= PMBusSlave_Poly;      		  // If MSB = 1, perform XOR

            msg <<= 1;                  							              // Shift left msg byte by 1
            msg &= 0x00FF;
        }
    }

    // The previous loop computes the CRC of the input bit stream. To this,
    // 8 trailing zeros are padded and the CRC of the resultant value is
    // computed. This gives the final CRC of the input bit stream.
    for(j = 0 ; j < 8 ; j++)
    {
        carry = PMBusSlave_CRC & 0x80;
        PMBusSlave_CRC <<= 1;

        if(carry) PMBusSlave_CRC ^= PMBusSlave_Poly;
    }

    PMBusSlave_CRC &= 0x00FF;	//We only want one byte (lower)

    return(PMBusSlave_CRC);
}
#endif

void PMBusSlave(void)
{
    if(flag_slave_recv_finish)
    {
        flag_slave_recv_finish = 0;
        PMBusSlave_ProcessMasterWriteCommand(); // The STOPF bit is not set after a NACK reception
    }
}
