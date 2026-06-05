#include "protocol.h"
#include "usb.h"
#include "mode_manager.h"
#include <string.h>
#include <stdbool.h>

enum {
    BRIDGE_PACKET_LEN = 65,
    BRIDGE_CAN_FRAME_COUNT = 9,
    BRIDGE_CAN_FRAME_SIZE = 8,
    BRIDGE_LAST_BYTE_INDEX = 64,
};

static uint8_t  g_bridge_buf_tx[BRIDGE_PACKET_LEN];
static uint8_t  g_bridge_buf_rx[BRIDGE_PACKET_LEN];
static volatile uint8_t g_rx_count = 0;          
static volatile uint32_t g_timeout_tick = 0;  
static volatile bool g_waiting_can_response = false;
static volatile bool g_usb_tx_pending = false;

//#define CAN_SEND_ID    0x781    
extern uint16_t SEND_ID;
#define CAN_TIMEOUT_MS 200               // 1ms频率下建议设为 200-500ms
#define CAN_TX_WAIT_TIMEOUT_MS 10

static void Reset_Bridge_State(void)
{
    g_rx_count = 0;
    g_waiting_can_response = false;
}

static void Clear_CAN_RxFIFO(void);

void Protocol_Bridge_Reset(void)
{
    Reset_Bridge_State();
    Clear_CAN_RxFIFO();
}

static void Clear_CAN_RxFIFO(void)
{
    FDCAN_RxHeaderTypeDef dummy_header;
    uint8_t dummy_data[BRIDGE_CAN_FRAME_SIZE];

    while (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &dummy_header, dummy_data) == HAL_OK) {
    }
}

static bool Send_Bridge_Frame(uint8_t frame_index)
{
    uint32_t wait_start = HAL_GetTick();
    FDCAN_TxHeaderTypeDef tx_header;
    uint8_t *payload;

    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) == 0) {
        if (HAL_GetTick() - wait_start > CAN_TX_WAIT_TIMEOUT_MS) {
            return false;
        }
    }

    tx_header.Identifier = SEND_ID;
    tx_header.IdType = FDCAN_STANDARD_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.DataLength = (frame_index < (BRIDGE_CAN_FRAME_COUNT - 1)) ? FDCAN_DLC_BYTES_8 : FDCAN_DLC_BYTES_1;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    payload = (frame_index < (BRIDGE_CAN_FRAME_COUNT - 1)) ? &g_bridge_buf_tx[frame_index * BRIDGE_CAN_FRAME_SIZE] : &g_bridge_buf_tx[BRIDGE_LAST_BYTE_INDEX];

    return (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_header, payload) == HAL_OK);
}

static void Protocol_TryFlushUsbResponse(void)
{
    if (!g_usb_tx_pending) {
        return;
    }

    if (CDC_Transmit_FS(g_bridge_buf_rx, BRIDGE_PACKET_LEN) == USBD_OK) {
        g_usb_tx_pending = false;
    }
}

/**
 * @brief 最终组包回传 USB (增加安全性)
 */
static void Finalize_And_Send_To_USB(void)
{
    if (g_rx_count == BRIDGE_CAN_FRAME_COUNT) {
        g_usb_tx_pending = true;
        g_waiting_can_response = false;
        g_rx_count = 0;
        Protocol_TryFlushUsbResponse();
        return;
    }

    Reset_Bridge_State();
}

/**
 * @brief 判断是否为合法包头
 */
static inline bool Is_Valid_Header(uint8_t h) {
    return (h == 0x40 || h == 0x23 || h == 0xFD || h == 0xFE || h == 0x41);
}

void Protocol_HandleUsbFrame(const uint8_t *data, uint16_t len)
{
    if ((data == NULL) || (len != BRIDGE_PACKET_LEN)) {
        return;
    }

    if (g_waiting_can_response || g_usb_tx_pending) {
        return;
    }

    if (!Is_Valid_Header(data[0])) {
        return;
    }

    memcpy(g_bridge_buf_tx, data, BRIDGE_PACKET_LEN);
    memset(g_bridge_buf_rx, 0, BRIDGE_PACKET_LEN);
    g_rx_count = 0;

    Clear_CAN_RxFIFO();

    for (uint8_t i = 0; i < BRIDGE_CAN_FRAME_COUNT; i++) {
        if (!Send_Bridge_Frame(i)) {
            Reset_Bridge_State();
            return;
        }
    }

    g_timeout_tick = HAL_GetTick();
    g_waiting_can_response = true;
}

void Protocol_OnUsbTransmitComplete(void)
{
    Protocol_TryFlushUsbResponse();
}

/**
 * @brief USB 协议解析与对齐
 */
void USB_Protocol_Parse_And_Bridge(void)
{
    /* USB fixed-frame parsing has moved into CDC_Receive_FS. */
}

/**
 * @brief CAN 发送与超时管理
 */
void CAN_Bridge_Sequence_Process(void)
{
    if (g_waiting_can_response) {
        if (HAL_GetTick() - g_timeout_tick > CAN_TIMEOUT_MS) {
            Reset_Bridge_State();
        }
    }
}

/**
 * @brief BB 模式: 扩展ID + 8字节数据 → 单帧CAN发送
 * @note  USB 数据格式: [ID3 ID2 ID1 ID0] [D0 D1 ... D7], 至少12字节
 */
void Protocol_HandleSingleCanFrame(const uint8_t *data, uint32_t len)
{
    if (data == NULL || len < 12U) {
        return;
    }

    uint32_t ext_id = ((uint32_t)data[0] << 24) |
                      ((uint32_t)data[1] << 16) |
                      ((uint32_t)data[2] << 8)  |
                       (uint32_t)data[3];

    FDCAN_TxHeaderTypeDef tx_header;
    tx_header.Identifier = ext_id;
    tx_header.IdType = FDCAN_EXTENDED_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.DataLength = FDCAN_DLC_BYTES_8;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    uint32_t wait_start = HAL_GetTick();
    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) == 0) {
        if (HAL_GetTick() - wait_start > CAN_TX_WAIT_TIMEOUT_MS) {
            return;
        }
    }

    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_header, (uint8_t *)&data[4]);
}

/**
 * @brief CAN 接收回调
 */
/* CAN RX → USB TX 缓冲: 批量收集CAN帧, 一次性写入TX FIFO */
#define CAN_FRAME_SIZE      12
#define CAN_RX_BUF_FRAMES   16
#define CAN_RX_BUF_SIZE     (CAN_FRAME_SIZE * CAN_RX_BUF_FRAMES)

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        /* 批量缓冲区: 收集多帧后一次性写入TX FIFO */
        static uint8_t  can_rx_buf[CAN_RX_BUF_SIZE];
        static uint16_t can_rx_buf_len = 0;

        FDCAN_RxHeaderTypeDef RxHeader;
        uint8_t rx_data[8];

        /* 循环读取FIFO0中所有待处理帧 */
        while (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, rx_data) == HAL_OK)
        {
            if (g_waiting_can_response)
            {
                /* AA 模式协议桥：收集9帧 → 组65字节回传 */
                if (g_rx_count < (BRIDGE_CAN_FRAME_COUNT - 1)) {
                    memcpy(&g_bridge_buf_rx[g_rx_count * BRIDGE_CAN_FRAME_SIZE], rx_data, BRIDGE_CAN_FRAME_SIZE);
                } else if (g_rx_count == (BRIDGE_CAN_FRAME_COUNT - 1)) {
                    g_bridge_buf_rx[BRIDGE_LAST_BYTE_INDEX] = rx_data[0];
                }

                g_rx_count++;

                if (g_rx_count >= BRIDGE_CAN_FRAME_COUNT) {
                    Finalize_And_Send_To_USB();
                }
            }
            else
            {
                /* 根据CAN帧类型自动切换模式 */
                if (RxHeader.IdType == FDCAN_STANDARD_ID) {
                    ModeManager_SwitchMode(MODE_LEGACY);
                } else {
                    ModeManager_SwitchMode(MODE_PROTOCOL_BRIDGE);
                }

                /* 将CAN帧打包到缓冲区
                 * 格式: [4字节ID大端] + [8字节数据] = 12字节
                 */
                if (can_rx_buf_len + CAN_FRAME_SIZE <= CAN_RX_BUF_SIZE)
                {
                    uint8_t *p = &can_rx_buf[can_rx_buf_len];
                    uint32_t id = RxHeader.Identifier;

                    p[0] = (uint8_t)((id >> 24) & 0xFF);
                    p[1] = (uint8_t)((id >> 16) & 0xFF);
                    p[2] = (uint8_t)((id >> 8)  & 0xFF);
                    p[3] = (uint8_t)(id & 0xFF);
                    memcpy(&p[4], rx_data, 8);

                    can_rx_buf_len += CAN_FRAME_SIZE;
                }
            }
        }

        /* 缓冲区满或FIFO已空: 批量写入TX FIFO */
        if (can_rx_buf_len > 0)
        {
            USR_WRITE_TXFIFO(can_rx_buf, can_rx_buf_len);
            can_rx_buf_len = 0;
        }
    }
}
