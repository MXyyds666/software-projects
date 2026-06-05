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
 * @file       PMBusMaster.c
 * @addtogroup PMBusMaster
 * @{
 ******************************************************************************/
#include "PMBusMaster.h"
#include "PMBus.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"

#define PMBUS_CONTRL_PIN    GPIO_PIN_7
#define PMBUS_ALERT_PIN     GPIO_PIN_8
#define PMBUS_CONTRL_GPIO   GPIOA
#define PMBUS_ALERT_GPIO    GPIOA


/* This is the PMBus command look-up table. Do not modify---------------------*/
const unsigned char PMBus_Commands[120] =
{
    0x00, // dummy byte
    0x19,0x78,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x80,0x81,0x82,0x98,
    0x79,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,
    0x93,0x94,0x95,0x96,0x97,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,
    0xA7,0xA8,0xA9,0x13,0x14,0x17,0x18,0x03,0x11,0x12,0x15,0x16,
    0x00,0x01,0x02,0x04,0x10,0x20,0x3A,0x3D,0x41,0x45,0x47,0x49,
    0x4C,0x50,0x54,0x56,0x5A,0x5C,0x63,0x69,0x21,0x22,0x23,0x24,
    0x25,0x26,0x27,0x28,0x29,0x2A,0x31,0x32,0x33,0x35,0x36,0x37,
    0x38,0x39,0x3B,0x3C,0x3E,0x3F,0x40,0x42,0x43,0x44,0x46,0x48,
    0x4A,0x4B,0x4F,0x51,0x52,0x53,0x55,0x57,0x58,0x59,0x5B,0x5D,
    0x5E,0x5F,0x60,0x61,0x62,0x64,0x65,0x66,0x68,0x6A,0x6B
};

uint8_t slave_address = 0x10;  //I2C_SLAVE_ADDR
uint8_t alert = 0;
uint8_t PMBusMaster_CommandGroup  = 0;

/**
*\*\name    PMBusMaster_Init.
*\*\fun     PMBus master init
*\*\param   none
*\*\return  None
**/
void PMBusMaster_Init(void)
{
    // Control Line functionality GPIO0
    GPIO_InitType pmbus_gpio;

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

    i2c_master_init();
}

/**
*\*\name    PMBusMaster.
*\*\fun     Perform a PMBus transaction as a master.
*\*\param   PMBusMaster_CommandByte PMBus command index.
*\*\param   PMBusMaster_RWFlag The Read/Write flag (R = 1, W = 0)
*\*\param   PMBusMaster_Message The message to write, if any.
*\*\param   *PMBusMaster_ReceivedValue Pointer to structure to store received data.
*\*\return  Success/Failure (Success = 1, Failure = 0)
**/
uint8_t PMBusMaster(uint8_t PMBusMaster_CommandByte, uint8_t PMBusMaster_RWFlag, uint16_t PMBusMaster_Message,uint8_t *PMBusMaster_ReceivedValue)
{
    uint8_t  PMBusMaster_TransmitBuffer[4];
    uint8_t  PMBusMaster_ReceiveBuffer[5];
    uint8_t  PMBusMaster_RxCount = 0;
    uint16_t PMBusMaster_Temp = 0;
    uint8_t  PMBusMaster_Index;

#if PEC  //PEC variables
    uint8_t PMBusMaster_CrcMsgSize = 0;
    uint8_t PMBusMaster_CrcMsg[5];
    uint8_t PMBusMaster_CrcMasterGenerated = 0;
    uint8_t PMBusMaster_CrcSlaveGenerated = 0;
    uint8_t PMBusMaster_Result = 0;

    PMBusMaster_CrcMsg[0] = slave_address;		// 1st CRC byte = slave address...
#endif

    PMBusMaster_Index = PMBusMaster_CommandByte;                     	// Store PMBus command byte as ...
    PMBusMaster_TransmitBuffer[0] = PMBus_Commands[PMBusMaster_Index];	//...1st byte in Tx buffer
    PMBusMaster_Temp = PMBusMaster_Message;

    PMBusMaster_TransmitBuffer[1] = PMBusMaster_Message & 0x00FF;	//store lower byte of message
    PMBusMaster_Temp = (PMBusMaster_Message) >> 8;
    PMBusMaster_TransmitBuffer[2] = PMBusMaster_Temp;				// store higher byte of message

    if(PMBusMaster_Index > 0 && PMBusMaster_Index < 13)			//read byte from slave device
        PMBusMaster_CommandGroup = READBYTE;
    if(PMBusMaster_Index > 12 && PMBusMaster_Index < 40)		// read word from slave device
        PMBusMaster_CommandGroup = READWORD;
    if(PMBusMaster_Index > 39 && PMBusMaster_Index < 44)		// write byte to slave device
        PMBusMaster_CommandGroup = WRITEBYTE;
    if(PMBusMaster_Index > 43 && PMBusMaster_Index < 49)		// send byte to slave device
        PMBusMaster_CommandGroup = SENDBYTE;
    /* Read or write one byte of data. R/W oprn. decided based on RWFlag *******/
    if(PMBusMaster_Index > 48 && PMBusMaster_Index < 69)
    {
        if (PMBusMaster_RWFlag == 0)				// write byte
            PMBusMaster_CommandGroup = WRITEBYTE;
        else										            // read byte
            PMBusMaster_CommandGroup = READBYTE;
    }
    /* Read or write one word of data. R/W oprn. decided based on RWFlag *******/
    if(PMBusMaster_Index > 68 && PMBusMaster_Index < 120)		// R/W Word
    {
        if (PMBusMaster_RWFlag == 0)             	// write word (new command group)
            PMBusMaster_CommandGroup = WRITEWORD;
        else                                    	// read word
            PMBusMaster_CommandGroup = READWORD;
    }
    if(PMBusMaster_Index >= 120)
        while(1);						//illegal index - invalid command trap

    switch(PMBusMaster_CommandGroup)
    {
#if !PEC
    case READBYTE: // read byte
        PMBusMaster_RxCount = 1;
        while (I2CMaster_NotReady());
        I2CMaster_Transmit(1,PMBusMaster_TransmitBuffer,1,PMBusMaster_ReceiveBuffer);
        break;

    case READWORD:  // read word
        PMBusMaster_RxCount = 2;
        while (I2CMaster_NotReady());
        I2CMaster_Transmit(1,PMBusMaster_TransmitBuffer,2,PMBusMaster_ReceiveBuffer);
        break;

    case WRITEBYTE: // write byte
        while (I2CMaster_NotReady() );
        I2CMaster_Transmit(2,PMBusMaster_TransmitBuffer,0,0);
        break;
    case SENDBYTE:  // send byte
        while (I2CMaster_NotReady());
        I2CMaster_Transmit(1,PMBusMaster_TransmitBuffer,0,0);
        break;
    case WRITEWORD:  // write word
        while (I2CMaster_NotReady());
        I2CMaster_Transmit(3,PMBusMaster_TransmitBuffer,0,0);
        break;
#else
    case READBYTE:					// read byte
        PMBusMaster_RxCount = 1;
        while (I2CMaster_NotReady());
        I2CMaster_Transmit(1,PMBusMaster_TransmitBuffer,2,PMBusMaster_ReceiveBuffer);

        /* Assembling bit stream for CRC check*/
        PMBusMaster_CrcMsg[1] = PMBusMaster_TransmitBuffer[0];  		    // store first tx byte
        PMBusMaster_CrcMsg[2] = slave_address  | 0x01;				          // store slave addres + R/W=1
        PMBusMaster_CrcMsg[3] = PMBusMaster_ReceiveBuffer[0];   		    // store rx byte 1
        PMBusMaster_CrcSlaveGenerated = PMBusMaster_ReceiveBuffer[1]; 	// store PEC byte from slave
        PMBusMaster_CrcMsgSize = 4;                 					          // bytes num

        /* CRC function call, generate CRC byte to compare with slave CRC*/
        PMBusMaster_CrcMasterGenerated = PMBusMaster_Crc8MakeBitwise(CRC8_INIT_REM,CRC8_POLY,PMBusMaster_CrcMsg,PMBusMaster_CrcMsgSize);
        if (PMBusMaster_CrcMasterGenerated == PMBusMaster_CrcSlaveGenerated)	//compare the PEC bytes
            PMBusMaster_Result = PEC_PASS;            // PEC byte was validated
        else
            PMBusMaster_Result = PEC_FAIL;            // failed PEC test
        break;

    case READWORD:						// read word
        PMBusMaster_RxCount = 2;
        while (I2CMaster_NotReady());
        I2CMaster_Transmit(1,PMBusMaster_TransmitBuffer,3,PMBusMaster_ReceiveBuffer);

        /* Assembling bit stream for CRC check*/
        PMBusMaster_CrcMsg[1] = PMBusMaster_TransmitBuffer[0];
        PMBusMaster_CrcMsg[2] = slave_address  | 0x01;				        // store slave address + R/W=1
        PMBusMaster_CrcMsg[3] = PMBusMaster_ReceiveBuffer[0];
        PMBusMaster_CrcMsg[4] = PMBusMaster_ReceiveBuffer[1];
        PMBusMaster_CrcSlaveGenerated = PMBusMaster_ReceiveBuffer[2];	//store PEC byte from slave
        PMBusMaster_CrcMsgSize = 5;

        /* CRC function call, generate CRC byte to compare with slave CRC*/
        PMBusMaster_CrcMasterGenerated = PMBusMaster_Crc8MakeBitwise(CRC8_INIT_REM,CRC8_POLY,PMBusMaster_CrcMsg,PMBusMaster_CrcMsgSize);
        if (PMBusMaster_CrcMasterGenerated == PMBusMaster_CrcSlaveGenerated)
            PMBusMaster_Result = PEC_PASS;
        else
            PMBusMaster_Result = PEC_FAIL;
        break;

    case WRITEBYTE:						// write byte
        /* CRC function call, generate CRC byte to transmit to slave device*/
        PMBusMaster_CrcMsg[1] = PMBusMaster_TransmitBuffer[0];
        PMBusMaster_CrcMsg[2] = PMBusMaster_TransmitBuffer[1];
        PMBusMaster_CrcMsgSize = 3;
        PMBusMaster_CrcMasterGenerated = PMBusMaster_Crc8MakeBitwise(CRC8_INIT_REM,CRC8_POLY,PMBusMaster_CrcMsg,PMBusMaster_CrcMsgSize);
        PMBusMaster_TransmitBuffer[2] = PMBusMaster_CrcMasterGenerated;

        while (I2CMaster_NotReady() );
        I2CMaster_Transmit(3,PMBusMaster_TransmitBuffer,0,0);

        //check if the slave verified the PEC byte
        if(alert == 0)
            PMBusMaster_Result = PEC_PASS;
        else
            PMBusMaster_Result = PEC_FAIL;
        break;

    case SENDBYTE:						// send byte
        /* CRC function call, generate CRC byte to transmit to slave device*/
        PMBusMaster_CrcMsg[1] = PMBusMaster_TransmitBuffer[0];
        PMBusMaster_CrcMsgSize = 2;
        PMBusMaster_CrcMasterGenerated = PMBusMaster_Crc8MakeBitwise(CRC8_INIT_REM,CRC8_POLY,PMBusMaster_CrcMsg,PMBusMaster_CrcMsgSize);
        PMBusMaster_TransmitBuffer[1] = PMBusMaster_CrcMasterGenerated;

        while (I2CMaster_NotReady());
        I2CMaster_Transmit(2,PMBusMaster_TransmitBuffer,0,0);

        //check if the slave verified the PEC byte
        if(alert == 0)
            PMBusMaster_Result = PEC_PASS;
        else
            PMBusMaster_Result = PEC_FAIL;
        break;

    case WRITEWORD:						// write word
        /* CRC function call, generate CRC byte to transmit to slave device*/
        PMBusMaster_CrcMsg[1] = PMBusMaster_TransmitBuffer[0];
        PMBusMaster_CrcMsg[2] = PMBusMaster_TransmitBuffer[1];
        PMBusMaster_CrcMsg[3] = PMBusMaster_TransmitBuffer[2];
        PMBusMaster_CrcMsgSize = 4;
        PMBusMaster_CrcMasterGenerated = PMBusMaster_Crc8MakeBitwise(CRC8_INIT_REM,CRC8_POLY,PMBusMaster_CrcMsg,PMBusMaster_CrcMsgSize);
        PMBusMaster_TransmitBuffer[3] = PMBusMaster_CrcMasterGenerated;

        while (I2CMaster_NotReady());
        I2CMaster_Transmit(4,PMBusMaster_TransmitBuffer,0,0);

        //check if the slave verified the PEC byte
        if(alert == 0)
            PMBusMaster_Result = PEC_PASS;
        else
            PMBusMaster_Result = PEC_FAIL;
        break;
#endif

    default:
        break;
    }

    if (PMBusMaster_ReceivedValue !=0)
    {
        *PMBusMaster_ReceivedValue++ = PMBusMaster_ReceiveBuffer[0];
        if (PMBusMaster_RxCount > 1)
            *PMBusMaster_ReceivedValue = PMBusMaster_ReceiveBuffer[1];
    }

#if PEC
    return PMBusMaster_Result;	//When PEC is implemented, return the result of the packet error checking (0 = failure, 1 = success)
#else
    return 1;	//When PEC is not implemented, return a 1 for completed communication
#endif
}

#if PEC

/**
*\*\name    PMBusMaster_Crc8MakeBitwise.
*\*\fun     alculate the Packet Error Checking byte.
*\*\param   PMBusMaster_CRC Initial value.
*\*\param   PMBusMaster_Poly The polynomial to use for the calculation.
*\*\param   *PMBusMaster_PMsg Pointer to the bytes from the PMBus transaction.
*\*\param   PMBusMaster_MsgSize Number of bytes in the last transaction.
*\*\return  The PEC byte.
**/
uint8_t PMBusMaster_Crc8MakeBitwise(uint8_t PMBusMaster_CRC, uint8_t PMBusMaster_Poly, uint8_t *PMBusMaster_Pmsg, uint16_t PMBusMaster_MsgSize)
{
    uint16_t i, j, carry;
    uint8_t msg;

    PMBusMaster_CRC = *PMBusMaster_Pmsg++;			   // first byte loaded in "crc"
    for(i = 0 ; i < PMBusMaster_MsgSize-1 ; i ++)
    {
        msg = *PMBusMaster_Pmsg++;					       // next byte loaded in "msg"

        for(j = 0 ; j < 8 ; j++)
        {
            carry = PMBusMaster_CRC & 0x80;				// check if MSB=1
            PMBusMaster_CRC = (PMBusMaster_CRC << 1) | (msg >> 7);		// Shift 1 bit of next byte into crc
            if(carry) PMBusMaster_CRC ^= PMBusMaster_Poly;      		  // If MSB = 1, perform XOR
            msg <<= 1;                  					// Shift left msg byte by 1
            msg &= 0x00FF;
        }
    }
    // The previous loop computes the CRC of the input bit stream. To this,
    // 8 trailing zeros are padded and the CRC of the resultant value is
    // computed. This gives the final CRC of the input bit stream.
    for(j = 0 ; j < 8 ; j++)
    {
        carry = PMBusMaster_CRC & 0x80;
        PMBusMaster_CRC <<= 1;
        if(carry) PMBusMaster_CRC ^= PMBusMaster_Poly;
    }

    PMBusMaster_CRC &= 0x00FF;		//We only want one byte (lower)

    return(PMBusMaster_CRC);
}
#endif

//
/**
*\*\name    EXTI15_10_IRQHandler
*\*\fun     ISR for the exti8,10 interrupt triggered from PA8 PA7 (Alert line) 
*\*\param   none.
*\*\return  none.
**/
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_LINE7) == SET)
    {
        
    }
		if(EXTI_GetITStatus(EXTI_LINE8) == SET)
    {
        
    }
    
    EXTI_ClrITPendBit(EXTI_LINE0);
}


