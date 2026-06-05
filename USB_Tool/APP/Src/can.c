#include "can.h"
#include "fifo.h"
#include <stdio.h>

/**
 * @brief  配置 FDCAN1  的过滤器为掩码模式，接收所有消息
 * @note   
 *         设置掩码为 0 表示接收所有 ID
 */
void FDCAN_ConfigFilters(void)
{
     FDCAN_FilterTypeDef sFilterConfig;

    /* 关键: 告知HAL库使用1个扩展ID过滤器 (CubeMX默认生成0) */
    hfdcan1.Init.StdFiltersNbr = 0;
    hfdcan1.Init.ExtFiltersNbr = 1;

    //------------------- FDCAN1 过滤器：掩码模式接收所有消息 -------------------
    sFilterConfig.IdType       = FDCAN_EXTENDED_ID;      // 扩展ID
    sFilterConfig.FilterIndex  = 0;                      // 过滤器索引 0
    sFilterConfig.FilterType   = FDCAN_FILTER_MASK;      // 掩码模式
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 接收到的消息放入 FIFO0
    sFilterConfig.FilterID1    = 0x00000000;             // ID 值
    sFilterConfig.FilterID2    = 0x00000000;             // 掩码值：0 表示接收所有位

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
        Error_Handler();

    // 配置全局过滤器：接收所有非匹配的消息
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, 
		FDCAN_ACCEPT_IN_RX_FIFO0,    // 非匹配标准ID的消息放入FIFO0
		FDCAN_ACCEPT_IN_RX_FIFO0,    // 非匹配扩展ID的消息放入FIFO0
		FDCAN_FILTER_REMOTE,         // 远程帧
		FDCAN_FILTER_REMOTE);        // 远程帧

    // 启动 FDCAN1
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
        Error_Handler();

    // 激活通知
    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_LINE0) != HAL_OK)
        Error_Handler();

    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, FDCAN_INTERRUPT_LINE0) != HAL_OK)
        Error_Handler();
}

// 发送标准/扩展 CAN 帧的封装函数
HAL_StatusTypeDef CAN_Send_Frame(uint32_t id, uint8_t *pData, uint8_t len) 
{
    FDCAN_TxHeaderTypeDef TxHeader;
    
    TxHeader.Identifier = id;
    TxHeader.IdType = (id > 0x7FF) ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN; // 强制经典 CAN 模式
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    // 转换长度宏
    uint32_t dlc_table[] = {FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2, 
                            FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5, 
                            FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8};
    TxHeader.DataLength = dlc_table[len > 8 ? 8 : len];

    // 等待硬件邮箱有空位，防止丢包
    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) == 0);

    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, pData);
}

//// CAN 接收回调函数
//void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
//{
//	if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
//	{
//		FDCAN_RxHeaderTypeDef RxHeader;
//		uint8_t RxData[8];

//		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
//		{
//			// --- 根据你的 DEBUG 结果，直接取低 8 位即可 ---
//			uint8_t real_len = (uint8_t)(RxHeader.DataLength & 0xFF); 
//			if(real_len > 8) real_len = 8; // 经典 CAN 最大 8 字节

//			// 构造输出字符串
//			char str[128]; // 数组开大一点防止溢出
//			int str_len = sprintf(str, "CAN_RX ID:0x%03lX Len:%d Data:", 
//													 RxHeader.Identifier, real_len);
//			
//			for(int i = 0; i < real_len; i++) {
//					str_len += sprintf(str + str_len, "%02X ", RxData[i]);
//			}
//			str_len += sprintf(str + str_len, "\r\n");

//			// 发送给电脑
//			USR_WRITE_TXFIFO((uint8_t*)str, str_len);
//		}
//	}
//}
