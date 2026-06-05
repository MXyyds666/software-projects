/*******************************************************************************
    Filename:       I2CMaster.h

    Copyright 2011 Texas Instruments, Inc.
*******************************************************************************/

#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "n32h47x_48x_i2c.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "misc.h"
#include "log.h"
#include "delay.h"
#include "stdint.h"

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} Status;

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

#define MODULE_SELF_RESET       1
#define MODULE_RCC_RESET        2
#define SYSTEM_NVIC_RESET       3
#define COMM_RECOVER_MODE       MODULE_RCC_RESET

#ifdef __cplusplus
}
#endif

int i2c_slave_init(void);
// Prototype statements for functions
//void I2CMaster_Init(uint16_t I2CMaster_SlaveAddress, uint16_t I2CMaster_Prescale);
//void I2CMaster_Transmit(uint16_t I2CMaster_ByteCountTx, uint8_t *I2CMaster_TxArray, uint16_t I2CMaster_ByteCountRx, uint8_t *I2CMaster_RxArray);
//void I2CMaster_Wait(void);
//uint8_t I2CMaster_SlavePresent(uint8_t I2CMaster_SlaveAddress);
//uint8_t I2CMaster_NotReady();

//void i2c_master_int1a_isr(void);
//void cpu_timer0_isr(void);

#endif /*I2C_MASTER_H_*/
