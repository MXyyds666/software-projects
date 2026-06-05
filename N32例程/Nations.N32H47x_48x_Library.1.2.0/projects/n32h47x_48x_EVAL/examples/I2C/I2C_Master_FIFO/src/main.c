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


volatile Status test_status      = FAILED;
static __IO uint32_t I2CTimeout;


Status Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
void CommTimeOut_CallBack(ErrCode_t errcode);
static uint8_t tx_buf[TEST_BUFFER_SIZE] = {0};
static uint8_t rx_buf[TEST_BUFFER_SIZE] = {0};

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
    I2C_InitStruct(&i2cx_master); 
    i2cx_master.BusMode     = I2C_BUSMODE_I2C;
    i2cx_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2cx_master.OwnAddr1    = I2C_MASTER_ADDR;
    i2cx_master.AckEnable   = I2C_ACKEN;
    i2cx_master.AddrMode    = I2C_ADDR_MODE_7BIT;
    i2cx_master.ClkSpeed    = 100000; // 100K

    I2C_Init(I2Cx, &i2cx_master);
    I2C_Enable(I2Cx, ENABLE);
    I2C_EnableFIFO(I2Cx, ENABLE);

    I2C_SetTxFifoThreshold(I2Cx, FIFO_HALF_EMPTY_THR);
    I2C_SetRxFifoThreshold(I2Cx, FIFO_HALF_FULL_THR);

    return 0;
}

/**
*\*\name    i2c_master_fifo_send.
*\*\fun     IIC master send and receive test.
*\*\param   data-data to receive
*\*\param   len-length of data to receive
*\*\return  none 
**/
void i2c_master_fifo_send(uint8_t* data, int len)									
{	
	
	uint8_t* sendBufferPtr = data;
	uint8_t   TxIdx=0; 	
	uint16_t  feedSize = 0;		

	I2C_GenerateStart(I2Cx, ENABLE);
	/*Waiting EV5_FIFO event and clear it*/        
	while(!I2C_CheckFifoEvent(I2Cx,I2C_EVENT_MASTER_FIFO_MODE_SELECT)); 

	
	I2C_SendAddr7bit(I2Cx, I2Cx_SLAVE_ADDRESS7, I2C_DIRECTION_SEND);
	/*Waiting EV6_FIFO event and clear it*/				
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_MASTER_FIFO_TRANSMITTER_MODE_SELECTED));			

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
		while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_MASTER_FIFO_TRANSMITTING));
		
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
		
	/* --EV10_FIFO_1 */		
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_MASTER_FIFO_TRANSMITTED));
	
	/*Stop the communication.*/
	I2C_GenerateStop(I2Cx, ENABLE);
	
	/*Wait until the I2C master is not busy.*/
	while(I2C_GetFlag(I2Cx, I2C_FLAG_BUSY));
    
	I2C_ClrFIFO(I2Cx);

}

/**
*\*\name    i2c_master_fifo_recv.
*\*\fun     master receive data.
*\*\param   data-data to receive
*\*\param   len-length of data to receive
*\*\return  receive result 
**/
void i2c_master_fifo_recv(uint8_t* data, int len)
{
	uint8_t* recvBufferPtr = data;
	uint8_t   RxIdx=0;	
	uint16_t  feedSize = 0;

	I2C_EnableBYTENUM(I2Cx, ENABLE);
	I2C_SetReceivedDataBytesNum(I2Cx, len);			

	I2C_SetByteNumLastStartStop(I2Cx, STOP_PROCESS);			
	

	I2C_GenerateStart(I2Cx, ENABLE);
	/*Waiting EV5_FIFO event and clear it*/        
	while(!I2C_CheckFifoEvent(I2Cx,I2C_EVENT_MASTER_FIFO_MODE_SELECT)); 
	
	
	I2C_SendAddr7bit(I2Cx, I2Cx_SLAVE_ADDRESS7, I2C_DIRECTION_RECV);
	/*Waiting EV6_FIFO event and clear it*/ 
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_MASTER_FIFO_RECEIVER_MODE_SELECTED ));
		
	if(len >= FIFO_HALF_FULL_THR)
	{
		/*max rx fifo size*/
		feedSize = FIFO_HALF_FULL_THR;
		/* While there have datas to be read received*/
		while(feedSize <= (len - RxIdx)) 
		{				
			/*Waiting EV11_FIFO event*/ 
			while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_MASTER_FIFO_RECEIVING));	

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
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_MASTER_FIFO_STOP_DETECTED));				
	/*clear the STOPF flag*/
	I2Cx->CTRL1=I2Cx->CTRL1;		

	feedSize = len - RxIdx;	

	while(feedSize > 0)
	{
		recvBufferPtr[RxIdx++] = I2C_GetFIFODAT(I2Cx);	
		feedSize--;			
	}
	
	/*Waiting EV12_FIFO_EMPTY event*/ 
	while(!I2C_CheckFifoEvent(I2Cx, I2C_EVENT_MASTER_FIFO_RECEIVED));			
				
	
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
    log_info("\nthis is a i2c master fifo demo\r\n");
    /* Initialize the I2C EEPROM driver ----------------------------------------*/
    i2c_master_init();

    /* Fill the buffer to send */
    for (i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        tx_buf[i] = i;
    }
    /* First write in the memory followed by a read of the written data --------*/
    /* Write data*/
    i2c_master_fifo_send(tx_buf, TEST_BUFFER_SIZE);
    
    systick_delay_ms(20);

    /* Read data */
    i2c_master_fifo_recv(rx_buf, TEST_BUFFER_SIZE);

    /* Check if the data written to the memory is read correctly */
    test_status = Buffercmp(tx_buf, rx_buf, TEST_BUFFER_SIZE);
    if (test_status == PASSED)
    {
        log_info("i2c master fifo test pass!\r\n");
    }
    else
    {
        log_info("i2c master fifo test fail!\r\n");
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
    RCC_EnableAPB2PeriphClk(I2Cx_clk_en, ENABLE);
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
    RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1);
    
    IIC_RestoreSlaveByClock();
    
    i2c_master_init();
    log_info("***** IIC module by RCC reset! *****\r\n");
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



