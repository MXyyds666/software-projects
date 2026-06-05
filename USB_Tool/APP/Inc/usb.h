#ifndef __USB_H__
#define __USB_H__

#include "usbd_cdc_if.h"
#include "fifo.h"
#include "fdcan.h"

extern USBD_HandleTypeDef hUsbDeviceFS; // 引用全局 USB 句柄

void USB_CDC_TX_Bridge(void);
void USB_To_CAN_Bridge_Stanard(uint32_t id);
void USB_To_CAN_Bridge_Extended(uint32_t id);
void USB_Pure_Loopback_Test(void);

#endif
