#ifndef __CAN_H__
#define __CAN_H__

#include "fdcan.h"

void FDCAN_ConfigFilters(void);
HAL_StatusTypeDef CAN_Send_Frame(uint32_t id, uint8_t *pData, uint8_t len);

#endif
