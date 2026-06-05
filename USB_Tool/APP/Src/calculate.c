#include "calculate.h"

/**
 * @brief  计算CRC8校验值
 * @param  ptr: 数据指针
 * @param  len: 数据长度
 * @retval 计算出的CRC8值
 * 
 * 多项式: X8＋X4＋X3＋X2＋1
 * 初始值: 0x00
 * 结果异或值: 0x00
 */
uint8_t Compute_CRC8(uint8_t *ptr, uint16_t len) {
	unsigned char crc;
	unsigned char i;
	crc = 0;
	while(len--)
	{
		 crc ^= *ptr++;
		 for(i = 0;i < 8;i++)
		 {
				 if(crc & 0x01)
				 {
						 crc = (crc >> 1) ^ 0xB8;
				 }
				 else crc >>= 1;
		 }
	}
	return crc;
}
