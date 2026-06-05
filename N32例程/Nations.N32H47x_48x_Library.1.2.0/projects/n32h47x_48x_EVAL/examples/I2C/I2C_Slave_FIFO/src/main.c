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



static __IO uint32_t I2CTimeout;
void CommTimeOut_CallBack(ErrCode_t errcode);
static uint8_t data_buf[TEST_BUFFER_SIZE] = {0};
/**
*\*\name    i2c_slave_init.
*\*\fun     slave gpio/rcc/i2c initializes.
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

    i2cx_gpio.Pin        = I2Cx_SCL_PIN ;
    i2cx_gpio.GPIO_Pull = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SCL_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SCL_GPIO, &i2cx_gpio);
	
    i2cx_gpio.Pin        = I2Cx_SDA_PIN;
    i2cx_gpio.GPIO_Pull = GPIO_PULL_UP;
    i2cx_gpio.GPIO_Alternate = I2Cx_SDA_AF;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
    i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
    GPIO_InitPeripheral(I2Cx_SDA_GPIO, &i2cx_gpio);

    I2C_DeInit(I2Cx);
    I2C_InitStruct(&i2cx_slave);
    i2cx_slave.BusMode     = I2C_BUSMODE_I2C;
    i2cx_slave.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2cx_slave.OwnAddr1    = I2C_SLAVE_ADDR;
    i2cx_slave.AckEnable   = I2C_ACKEN;
    i2cx_slave.AddrMode    = I2C_ADDR_MODE_7BIT;
    i2cx_slave.ClkSpeed    = 100000; // 100000 100K

    I2C_Init(I2Cx, &i2cx_slave);
    I2C_Enable(I2Cx, ENABLE);
    I2C_EnableFIFO(I2Cx, ENABLE);

    I2C_SetTxFifoThreshold(I2Cx, FIFO_HALF_EMPTY_THR);
    I2C_SetRxFifoThreshold(I2Cx, FIFO_HALF_FULL_THR);

    return 0;
}

/**
*\*\name    i2c_slave_fifo_recv.
*\*\fun     slave receive data.
*\*\param   data-data to receive
*\*\param   rcv_len-length of data to receive
*\*\return  none 
**/
void i2c_slave_fifo_recv(uint8_t* data, uint32_t len)				
{
	uint8_t   RxIdx=0;
	uint16_t  feedSize = 0;	
	uint8_t* recvBufferPtr = data;

	I2C_EnableBYTENUM(I2Cx, ENABLE);
	I2C_SetReceivedDataBytesNum(I2Cx, len);			

	/* Waiting the EV1_FIFO and clear it */	
	while(!I2C_CheckFifoEvent(I2Cx,I2C_EVENT_SLAVE_FIFO_RECEIVER_ADDRESS_MATCHED));					
							

	if(len >= FIFO_HALF_FULL_THR)
	{
		/*max rx fifo size*/
		feedSize = FIFO_HALF_FULL_THR;
		/* While there have datas to be read received*/
		while(feedSize <= (len - RxIdx)) 
		{				
			/*Waiting EV11_FIFO event*/ 
			while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_SLAVE_FIFO_RECEIVING));	

			if(feedSize > (len - RxIdx - 1))
			{
				feedSize = len - RxIdx;
			}		
			while(feedSize > 0)
			{
				recvBufferPtr[RxIdx++] = I2C_GetFIFODAT(I2Cx);	
				feedSize--;				
			}	
			feedSize = FIFO_HALF_FULL_THR;				
		} 		
	}
	/* Waiting the EV4_FIFO and clear it(Receipt is finished) */	
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_SLAVE_FIFO_STOP_DETECTED));				
	/*clear the STOPF flag*/
	I2Cx->CTRL1=I2Cx->CTRL1;		

	feedSize = len - RxIdx;		
	while(feedSize > 0)
	{
		recvBufferPtr[RxIdx++] = I2C_GetFIFODAT(I2Cx);	
		feedSize--;				
	}
	
	/*Waiting EV12_FIFO event*/ 
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_SLAVE_FIFO_RECEIVED));		

	I2C_ClrFIFO(I2Cx);

}

/**
*\*\name    i2c_slave_fifo_send.
*\*\name    i2c_slave_send.
*\*\fun     slave send data.
*\*\param   data-data to send
*\*\param   len-length of data to send
*\*\return  none 
**/
void i2c_slave_fifo_send(uint8_t* data, uint32_t len)				
{
	uint8_t   TxIdx=0;
	uint16_t  feedSize = 0;	
	uint8_t* sendBufferPtr = data;

	/* Waiting the EV1_FIFO and clear it */	
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_SLAVE_FIFO_TRANSMITTER_ADDRESS_MATCHED));
	

	/*Send data.*/
	feedSize = 8;
	if(len < 8)
	{
		feedSize = len;
	}			
	while(feedSize > 0)
	{
		I2C_SetFIFODAT(I2Cx, sendBufferPtr[TxIdx++]);	
		feedSize--;				
	}			

	/*Send data.*/
	while(TxIdx <= (len - 1))
	{					
		/*Waiting EV10_FIFO event and clear it*/				
		while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_SLAVE_FIFO_TRANSMITTING));
		
		feedSize = FIFO_HALF_EMPTY_THR;
		
		if(feedSize > (len - 1 - TxIdx))
		{
			feedSize = len - TxIdx;
		}				
		while(feedSize > 0)
		{
			I2C_SetFIFODAT(I2Cx, sendBufferPtr[TxIdx++]);	
			feedSize--;				
		}				
	}
	
	/* Waiting the EV3_2_FIFO and clear it */	
	while(!I2C_CheckFifoEvent(I2Cx,I2C_EVENT_SLAVE_FIFO_ACK_FAILURE ));		
	/*Clear AF flag */
	I2C_ClrFlag(I2Cx, I2C_FLAG_ACKFAIL);
   
}


/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none 
**/
int main(void)
{
    log_init();
    log_info("\nThis is the i2c slave fifo mode.\r\n");

    /* Initialize the I2C driver ----------------------------------------*/
    i2c_slave_init();

    /* Read data */
    i2c_slave_fifo_recv(data_buf, TEST_BUFFER_SIZE);
    
	/* Write data */
    i2c_slave_fifo_send(data_buf, TEST_BUFFER_SIZE);
    
    while (1)
    {
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
    
    log_info("***** IIC module by RCC reset! *****\r\n");
    i2c_slave_init();
    
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
    
#if (COMM_RECOVER_MODE == MODULE_SELF_RESET)
    IIC_SWReset();
#elif (COMM_RECOVER_MODE == MODULE_RCC_RESET)
    IIC_RCCReset();
#elif (COMM_RECOVER_MODE == SYSTEM_NVIC_RESET)
    SystemNVICReset();
#endif
}



