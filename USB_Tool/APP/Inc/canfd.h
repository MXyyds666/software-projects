#ifndef _CANFD_H_
#define _CANFD_H_
#include "main.h"
#include "fdcan.h"


typedef enum
{
    CANFD_BR_NO_CHANGE = 0x00,
    CANFD_BR_100K      = 0x01,
    CANFD_BR_125K      = 0x02,
    CANFD_BR_250K      = 0x03,
    CANFD_BR_500K      = 0x04,
    CANFD_BR_1M        = 0x05,
    CANFD_BR_2M        = 0x06,
    CANFD_BR_4M        = 0x07,
    CANFD_BR_8M        = 0x08,
} CANFD_Baudrate_t;
typedef struct
{
    uint16_t nominal_prescaler;
    uint16_t data_prescaler;
    uint8_t  enable_brs;
} CANFD_TimingCfg_t;

void FDCAN_ConfigFilters(void);
//CAN发送函数
HAL_StatusTypeDef FDCAN_SendMessage(FDCAN_HandleTypeDef *hfdcan,uint32_t std_id,uint8_t *data,uint8_t length);
//设置CANFD波特率
HAL_StatusTypeDef FDCAN1_SetBaudRate(uint8_t baud_index);
#endif 

