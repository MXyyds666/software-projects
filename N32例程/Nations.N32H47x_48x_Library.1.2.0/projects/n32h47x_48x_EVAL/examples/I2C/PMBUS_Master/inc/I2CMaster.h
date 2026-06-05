/*******************************************************************************
    Filename:       I2CMaster.h

    Copyright 2011 Texas Instruments, Inc.
*******************************************************************************/

#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#include "stdint.h"

#define I2C_MASTER_ADDR   0x30
#define I2C_SLAVE_ADDR    0x10
#define TEST_BUFFER_SIZE  100
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x8000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


#if defined (N32H475)
#define I2Cx I2C1
#define I2Cx_RCC            RCC_APB1_PERIPH_I2C1
#define I2Cx_SCL_PIN        GPIO_PIN_11
#define I2Cx_SDA_PIN        GPIO_PIN_1
#define I2Cx_SCL_GPIO       GPIOD
#define I2Cx_SDA_GPIO       GPIOD
#define I2Cx_SCL_AF         GPIO_AF5
#define I2Cx_SDA_AF         GPIO_AF7

#define I2Cx_clk_en         RCC_AHB_PERIPHEN_GPIOD

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

#define I2Cx_EV_IRQn  			I2C3_EV_IRQn
#define I2Cx_ER_IRQn  			I2C3_ER_IRQn
#define I2Cx_EV_IRQHandler  I2C3_EV_IRQHandler
#define I2Cx_ER_IRQHandler  I2C3_ER_IRQHandler

#endif

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} Status;

typedef enum
{
    C_READY = 0,
    C_START_BIT,
    C_STOP_BIT
}CommCtrl_t;

typedef enum
{
    MASTER_OK = 0,
    MASTER_BUSY,
    MASTER_MODE,
    MASTER_TXMODE,
    MASTER_RXMODE,
    MASTER_SENDING,
    MASTER_SENDED,
    MASTER_RECVD,
    MASTER_BYTEF,
    MASTER_BUSERR,
    MASTER_UNKNOW,
    SLAVE_OK = 20,
    SLAVE_BUSY,
    SLAVE_MODE,
    SLAVE_BUSERR,
    SLAVE_UNKNOW,

}ErrCode_t;

// Prototype statements for functions
int i2c_master_init(void);
void I2CMaster_Transmit(uint16_t I2CMaster_ByteCountTx, uint8_t *I2CMaster_TxArray, uint16_t I2CMaster_ByteCountRx, uint8_t *I2CMaster_RxArray);
void I2CMaster_Wait(void);
uint8_t I2CMaster_NotReady(void);

#endif /*I2C_MASTER_H_*/
