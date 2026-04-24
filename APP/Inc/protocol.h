#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "stm32g4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#define USE_CRC_TEST	0

void USB_Protocol_Parse_And_Bridge(void);
void CAN_Bridge_Sequence_Process(void);
void USB_Protocol_Parse_Loopback(void);

#endif
