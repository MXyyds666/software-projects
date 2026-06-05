#include "usb.h"

/* CAN帧USB传输长度: 4B ID + 8B 数据 = 12字节 */
#define CAN_FRAME_USB_SIZE  12U

const uint32_t DLC_Table[] = {
    FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2, FDCAN_DLC_BYTES_3,
    FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5, FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7,
    FDCAN_DLC_BYTES_8
};

// usb.c
extern USBD_HandleTypeDef hUsbDeviceFS;

void USB_CDC_TX_Bridge(void)
{
    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;

    /* 循环发送直到FIFO空或USB忙 */
    while (hcdc->TxState == 0)
    {
        uint16_t tx_len = USR_TXFIFO_AVAILABLE();
        /* 至少要有完整一帧才发送 */
        if (tx_len < CAN_FRAME_USB_SIZE) break;

        static uint8_t usb_temp[64];
        uint16_t send_now = (tx_len > 64) ? 64 : tx_len;

        /* 按12字节对齐，确保不打碎CAN帧边界 */
        send_now = (send_now / CAN_FRAME_USB_SIZE) * CAN_FRAME_USB_SIZE;

        USR_READ_TXFIFO_BUF(usb_temp, send_now);

        if (CDC_Transmit_FS(usb_temp, send_now) != USBD_OK) {
            break;
        }
    }
}

void USB_To_CAN_Bridge_Stanard(uint32_t id)
{
    uint16_t rx_avail = USR_RXFIFO_AVAILABLE();
    
    // 如果有数据，就开始搬运
    if (rx_avail > 0) 
    {
        // 检查 CAN 发送 FIFO 是否有空间（防止发太快堵塞）
        if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) > 0) 
        {
            FDCAN_TxHeaderTypeDef TxHeader;
            uint8_t can_payload[8];
            
            // 每次最多取 8 字节（经典 CAN 的限制）
            uint8_t len_to_send = (rx_avail > 8) ? 8 : rx_avail;
            
            // 从 RX FIFO 读取数据
            for(int i = 0; i < len_to_send; i++) {
                can_payload[i] = USR_READ_RXFIFO();
            }

            // 配置发送头
            TxHeader.Identifier = id;             // 这里可以固定 ID，也可以根据协议解析
            TxHeader.IdType = FDCAN_STANDARD_ID;
            TxHeader.TxFrameType = FDCAN_DATA_FRAME;
            TxHeader.DataLength = DLC_Table[len_to_send]; // 关键：使用转换表
            TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
            TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
            TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
            TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
            TxHeader.MessageMarker = 0;

            // 发送到 CAN 总线
            HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, can_payload);
        }
    }
}

void USB_To_CAN_Bridge_Extended(uint32_t id)
{
    uint16_t rx_avail = USR_RXFIFO_AVAILABLE();
    
    // 如果有数据，就开始搬运
    if (rx_avail > 0) 
    {
        // 检查 CAN 发送 FIFO 是否有空间（防止发太快堵塞）
        if (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) > 0) 
        {
            FDCAN_TxHeaderTypeDef TxHeader;
            uint8_t can_payload[8];
            
            // 每次最多取 8 字节（经典 CAN 的限制）
            uint8_t len_to_send = (rx_avail > 8) ? 8 : rx_avail;
            
            // 从 RX FIFO 读取数据
            for(int i = 0; i < len_to_send; i++) {
                can_payload[i] = USR_READ_RXFIFO();
            }

            // 配置发送头
            TxHeader.Identifier = id;             // 这里可以固定 ID，也可以根据协议解析
            TxHeader.IdType = FDCAN_EXTENDED_ID;
            TxHeader.TxFrameType = FDCAN_DATA_FRAME;
            TxHeader.DataLength = DLC_Table[len_to_send]; // 关键：使用转换表
            TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
            TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
            TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
            TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
            TxHeader.MessageMarker = 0;

            // 发送到 CAN 总线
            HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, can_payload);
        }
    }
}

/**
 * @brief USB 端口纯软件回环测试
 * 功能：将从电脑收到的任何数据，原样丢回给电脑。
 */
void USB_Pure_Loopback_Test(void)
{
    // 1. 检查接收 FIFO (usr_rx_fifo) 里是否有电脑发来的数据
    uint16_t rx_count = USR_RXFIFO_AVAILABLE();
    
    if (rx_count > 0)
    {
        // 2. 准备一个临时小缓冲区（或者直接一个字节一个字节处理）
        static uint8_t echo_buf[64]; 
        uint16_t len_to_move = (rx_count > 64) ? 64 : rx_count;

        // 3. 从接收 FIFO 读取数据
        // 注意：这里我们循环调用单字节读取，或者你可以写一个批量读取函数
        for(int i = 0; i < len_to_move; i++)
        {
            echo_buf[i] = USR_READ_RXFIFO(); // 从 rx_fifo 弹出
        }

        // 4. 将读取到的数据直接写入发送 FIFO (usr_tx_fifo)
        USR_WRITE_TXFIFO(echo_buf, len_to_move);
    }
}
