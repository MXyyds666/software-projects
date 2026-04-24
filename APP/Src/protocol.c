#include "protocol.h"
#include "usb.h"
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
 * @brief CAN 接收回调
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) 
    {
        FDCAN_RxHeaderTypeDef RxHeader;
        uint8_t rx_data[8];
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, rx_data) == HAL_OK) 
        {
            // 此时不判断 ID，实现全透明透传
            if (g_waiting_can_response)
            {
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
        }
    }
}
