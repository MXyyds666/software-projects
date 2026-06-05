/**
 * @file bsp_can.h
 * @author baishuaijie (baishuaijie17@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-18
 * 
 * 
 * 
 */
#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include "at32f45x.h"

#define USE_CAN_IT  1

error_status Bsp_Can1_Init(void);
void Can_Boardrate_Set(can_type* can_x,uint16_t baudrate);
void Bsp_Can_Filter_config(can_type* can_x, can_identifier_type id_type,uint32_t id, uint32_t mask_id,can_filter_type can_filter_num);
error_status Bsp_Can_Transmit_Classic_Standard(can_type* can_x, uint32_t id, uint8_t *data, uint8_t num);
error_status Bsp_Can_Transmit_Classic_Extended(can_type* can_x, uint32_t id, uint8_t *data, uint8_t num);

#endif
