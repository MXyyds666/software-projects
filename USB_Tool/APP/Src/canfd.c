#include "canfd.h"
#include "stdio.h"
static uint32_t GET_FDCAN_STD_LENGTH(uint8_t data_length);


/**
 * @brief  配置 FDCAN1  的过滤器为掩码模式，接收所有消息
 * @note   
 *         设置掩码为 0 表示接收所有 ID
 */
void FDCAN_ConfigFilters(void)
{
     FDCAN_FilterTypeDef sFilterConfig;

    //------------------- FDCAN1 过滤器：掩码模式接收所有消息 -------------------

    sFilterConfig.IdType       = FDCAN_EXTENDED_ID;      // 扩展ID
    sFilterConfig.FilterIndex  = 0;                      // 过滤器索引 1
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



/**
 * @brief  发送一帧标准ID的 CAN FD 数据帧（支持自动DLC匹配）
 * @note   可发送任意长度（0~64字节）数据，函数内部自动选择合法DLC编码
 *         支持 BitRateSwitch（速率切换），采用 FDCAN_FD_CAN 格式
 *         使用发送FIFO队列（非事件队列）
 * 
 * @param  hfdcan   指向要发送的 FDCAN 句柄（如 &hfdcan1 / &hfdcan2）
 * @param  std_id   发送帧的标准ID（11位，范围 0~0x7FF）
 * @param  data     指向要发送的数据缓冲区
 * @param  length   实际要发送的数据长度（单位：字节，范围 0~64）
 * 
 * @retval HAL_StatusTypeDef
 *         - HAL_OK         成功发送
 *         - HAL_ERROR      参数非法或发送失败（如长度超限、硬件故障等）
 */
HAL_StatusTypeDef FDCAN_SendMessage(FDCAN_HandleTypeDef *hfdcan,
                                    uint32_t std_id,
                                    uint8_t *data,
                                    uint8_t length)
{
    FDCAN_TxHeaderTypeDef txHeader;

    // 限制数据长度：FDCAN 最大 64 字节
    if (length > 64) return HAL_ERROR;

    // 填充发送帧头
    txHeader.Identifier        = std_id;
    txHeader.IdType            = FDCAN_EXTENDED_ID;	//扩展ID
    txHeader.TxFrameType       = FDCAN_DATA_FRAME;	//数据帧
    txHeader.DataLength        = GET_FDCAN_STD_LENGTH(length);//GET_FDCAN_STD_LENGTH(length);
    txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    txHeader.BitRateSwitch     = FDCAN_BRS_OFF;		//可变波特率 FDCAN_BRS_OFF
    txHeader.FDFormat          = FDCAN_FD_CAN;		//FDCAN格式发送 FDCAN_FD_CAN
    txHeader.TxEventFifoControl= FDCAN_NO_TX_EVENTS;
    txHeader.MessageMarker     = 0;

    return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &txHeader, data);

}

//用于处理数据位数不匹配情况
static uint32_t GET_FDCAN_STD_LENGTH(uint8_t data_length)
{
    if (data_length <= 0) {
        return FDCAN_DLC_BYTES_0;
    } else if (data_length <= 1) {
        return FDCAN_DLC_BYTES_1;
    } else if (data_length <= 2) {
        return FDCAN_DLC_BYTES_2;
    } else if (data_length <= 3) {
        return FDCAN_DLC_BYTES_3;
    } else if (data_length <= 4) {
        return FDCAN_DLC_BYTES_4;
    } else if (data_length <= 5) {
        return FDCAN_DLC_BYTES_5;
    } else if (data_length <= 6) {
        return FDCAN_DLC_BYTES_6;
    } else if (data_length <= 7) {
        return FDCAN_DLC_BYTES_7;
    } else if (data_length <= 8) {
        return FDCAN_DLC_BYTES_8;
    } else if (data_length <= 12) {
        return FDCAN_DLC_BYTES_12;
    } else if (data_length <= 16) {
        return FDCAN_DLC_BYTES_16;
    } else if (data_length <= 20) {
        return FDCAN_DLC_BYTES_20;
    } else if (data_length <= 24) {
        return FDCAN_DLC_BYTES_24;
    } else if (data_length <= 32) {
        return FDCAN_DLC_BYTES_32;
    } else if (data_length <= 48) {
        return FDCAN_DLC_BYTES_48;
    } else {
        return FDCAN_DLC_BYTES_64;
    }
}

/**
  * @brief  根据图表Index配置FDCAN1波特率
  * @param  baud_index: 0x00 - 0x05
  * @note   时钟源: 160MHz, Total TQ: 10 (7+2+1)
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef FDCAN1_SetBaudRate(uint8_t baud_index)
{
    /* 0x00: 不更改 */
    if (baud_index == 0x00) {
        return HAL_OK;
    }

		
    // 1. 等待 FIFO 变空 (假设 FIFO 深度为 3，FreeLevel==3 表示全空)
    // 注意：具体深度取决于 CubeMX 配置，通常为 3
    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) != 3) {
        // 忙等待，直到 FIFO 空
    }

    HAL_Delay(2); 		
		
		
    /* 1. 先取消初始化，进入配置模式 */
    if (HAL_FDCAN_DeInit(&hfdcan1) != HAL_OK) {
        return HAL_ERROR;
    }

    /* 2. 重载基础配置 */
    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = DISABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = DISABLE;
    
    /* 时间参数保持不变 (Sample Point 80%) */
    hfdcan1.Init.NominalSyncJumpWidth = 1;
    hfdcan1.Init.NominalTimeSeg1 = 7;
    hfdcan1.Init.NominalTimeSeg2 = 2;
    hfdcan1.Init.DataSyncJumpWidth = 1;
    hfdcan1.Init.DataTimeSeg1 = 7;
    hfdcan1.Init.DataTimeSeg2 = 2;
    hfdcan1.Init.StdFiltersNbr = 0;
    hfdcan1.Init.ExtFiltersNbr = 0;
    hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

    /* 3. 根据 Index 设置 Prescaler 和 FrameFormat */
    /* 公式: Prescaler = 160M / (Baud * 10) */
    
    switch (baud_index)
    {
        /* --- 低于 1M：不做加速 (No BRS)，仲裁域与数据域速度相同 --- */
        
        case 0x01: // 100K -> Pre = 160
            hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS; 
            hfdcan1.Init.NominalPrescaler = 160;
            hfdcan1.Init.DataPrescaler = 160; 
            break;

        case 0x02: // 125K -> Pre = 128
            hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
            hfdcan1.Init.NominalPrescaler = 128;
            hfdcan1.Init.DataPrescaler = 128;
            break;

        case 0x03: // 250K -> Pre = 64
            hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
            hfdcan1.Init.NominalPrescaler = 64;
            hfdcan1.Init.DataPrescaler = 64;
            break;

        case 0x04: // 500K -> Pre = 32
            hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
            hfdcan1.Init.NominalPrescaler = 32;
            hfdcan1.Init.DataPrescaler = 32;
            break;

        /* --- 大于等于 1M：做加速 (BRS)，仲裁域固定1M，数据域变动 --- */

        case 0x05: // 1M (1M+1M) -> NomPre=16, DataPre=16

            hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
            hfdcan1.Init.NominalPrescaler = 16; // 1M
            hfdcan1.Init.DataPrescaler = 16;    // 1M
            break;


        default:
            // 非法参数，不执行Init，直接返回错误
            return HAL_ERROR;
    }

    /* 4. 重新初始化硬件 */
    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* 5. 关键：初始化后必须重新启动 FDCAN */
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}























