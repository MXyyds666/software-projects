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
#include "n32h47x_48x.h"
#include "i2c_eeprom.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "log.h"
/** n32h47x_48x_StdPeriph_Examples **/

/** I2C_EEPROM **/

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} Status;

uint8_t tx_buf[TEST_EEPROM_SIZE] = {0};
uint8_t rx_buf[TEST_EEPROM_SIZE] = {0};
volatile Status test_status      = FAILED;

Status Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

void Delay_us(uint32_t nCount)
{
    uint32_t tcnt;
    while (nCount--)
    {
        tcnt = 144 / 5;
        while (tcnt--){;}
    }
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
    log_info("This is a I2C EEPROM polling demo\r\n");
    /* Initialize the I2C EEPROM driver ----------------------------------------*/
    I2C_EE_Init();

    /* Fill the buffer to send */
    for (i = 0; i < TEST_EEPROM_SIZE; i++)
    {
        tx_buf[i] = i;
    }
    log_info("Write to I2C EEPROM\r\n");
    /* First write in the memory followed by a read of the written data --------*/
    /* Write to I2C EEPROM from TEST_EEPROM_ADDR */
    I2C_EE_WriteBuffer(tx_buf, TEST_EEPROM_ADDR, TEST_EEPROM_SIZE);
    
    log_info("Read %d byte data from I2C EEPROM\r\n", i);
    I2C_EE_ReadBuffer(rx_buf, TEST_EEPROM_ADDR, TEST_EEPROM_SIZE);
		
    /* Check if the data written to the memory is read correctly */
    test_status = Buffercmp(tx_buf, rx_buf, TEST_EEPROM_SIZE);
    if (test_status == PASSED)
    {
        log_info("The write and read data are the same,I2C EEPROM test pass\r\n");
    }
    else
    {
        log_info("The write and read data are different,I2C EEPROM test fail\r\n");
    }
		
    while (1)
    {
    }
}

/**
*\*\brief  Compares two buffers.
*\*\param  pBuffer, pBuffer1: buffers to be compared.
*\*\param BufferLength buffer's length
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

void IIC_RestoreSlaveByClock(void)
{
    uint8_t i;
    GPIO_InitType i2cx_gpio;
    
    I2C_GPIO->POD |= (I2Cx_SCL_PIN | I2Cx_SDA_PIN);//pull up
	  RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
    I2C_GPIO->POD |= (I2Cx_SCL_PIN | I2Cx_SDA_PIN);//pull up
    
    GPIO_InitStruct(&i2cx_gpio);
    i2cx_gpio.Pin        = I2Cx_SCL_PIN;
    i2cx_gpio.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitPeripheral(I2C_GPIO, &i2cx_gpio);
    
    for (i = 0; i < 9; i++)
    {
        GPIO_SetBits(I2C_GPIO, I2Cx_SCL_PIN);
        Delay_us(5);
        GPIO_ResetBits(I2C_GPIO, I2Cx_SCL_PIN);
        Delay_us(5);
    }
}
    

void IIC_RCCReset(void)
{   
    RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1);
  
    IIC_RestoreSlaveByClock();
        
    I2C_EE_Init();
    
    log_info("***** IIC module by RCC reset! *****\r\n");
}

