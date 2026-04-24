#include "protocol.h"
#include "fifo.h"
#include "usb.h"
#include <string.h>
#include <stdbool.h>

/* --- 状态机与变量：全部加上 volatile 保证多任务可见性 --- */
typedef enum {
    BRIDGE_IDLE = 0,         
    BRIDGE_SENDING,          
    BRIDGE_WAITING_REPLY,    
} bridge_state_t;

static volatile bridge_state_t g_bridge_state = BRIDGE_IDLE;

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

// 声明外部时间戳变量（已在 usbd_cdc_if.c 更新）
extern volatile uint32_t g_last_usb_rx_tick;

//#define CAN_SEND_ID    0x781    
extern uint16_t SEND_ID;
#define CAN_TIMEOUT_MS 200               // 1ms频率下建议设为 200-500ms
#define USB_SYNC_TIMEOUT_MS 1          
#define CAN_TX_WAIT_TIMEOUT_MS 10

static void Reset_Bridge_State(void)
{
    g_rx_count = 0;
    g_bridge_state = BRIDGE_IDLE;
}

static void Flush_USB_RxFIFO(void)
{
    while (USR_RXFIFO_AVAILABLE() > 0) {
        USR_READ_RXFIFO();
    }
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

/**
 * @brief 最终组包回传 USB (增加安全性)
 */
static void Finalize_And_Send_To_USB(void) {
    // 只有确实收到了数据才发回，防止空回传
    if (g_rx_count > 0) {
        // 这里的 USR_WRITE_TXFIFO 仅仅是存入缓冲区
        // 真正的发送由 main 循环里的 USB_CDC_TX_Bridge 处理
        USR_WRITE_TXFIFO(g_bridge_buf_rx, BRIDGE_PACKET_LEN);
    }
    
    Reset_Bridge_State();
}

/**
 * @brief 判断是否为合法包头
 */
static inline bool Is_Valid_Header(uint8_t h) {
    return (h == 0x40 || h == 0x23 || h == 0xFD || h == 0xFE || h == 0x41);
}

/**
 * @brief USB 协议解析与对齐
 */
void USB_Protocol_Parse_And_Bridge(void)
{
    uint16_t avail = USR_RXFIFO_AVAILABLE();
    
    // 超时排空残留
    if (avail > 0 && avail < BRIDGE_PACKET_LEN) {
        if (HAL_GetTick() - g_last_usb_rx_tick > USB_SYNC_TIMEOUT_MS) {
            Flush_USB_RxFIFO();
            return;
        }
    }

    // 只有 IDLE 时才处理。注意：1ms 频率下，如果 CAN 还没处理完，这里会跳过
    if (avail == 0 || g_bridge_state != BRIDGE_IDLE) return;

    // 滑动对齐
    if (!Is_Valid_Header(USR_FIFO_PEEK(0))) {
        USR_READ_RXFIFO(); 
        return;
    }

    if (avail < BRIDGE_PACKET_LEN) return; 

    // 提取 65 字节
    for (uint8_t i = 0; i < BRIDGE_PACKET_LEN; i++) {
        g_bridge_buf_tx[i] = USR_READ_RXFIFO();
    }
	
    memset(g_bridge_buf_rx, 0, BRIDGE_PACKET_LEN); 
    g_rx_count = 0;

    // 清空 CAN 接收 FIFO
    Clear_CAN_RxFIFO();

    g_bridge_state = BRIDGE_SENDING;
}

/**
 * @brief CAN 发送与超时管理
 */
void CAN_Bridge_Sequence_Process(void)
{
    if (g_bridge_state == BRIDGE_SENDING)
    {
        for (uint8_t i = 0; i < BRIDGE_CAN_FRAME_COUNT; i++) 
        {
            if (!Send_Bridge_Frame(i)) {
                Reset_Bridge_State();
                return;
            }
        }
        g_timeout_tick = HAL_GetTick();
        g_bridge_state = BRIDGE_WAITING_REPLY;
    }

    if (g_bridge_state == BRIDGE_WAITING_REPLY) {
        if (HAL_GetTick() - g_timeout_tick > CAN_TIMEOUT_MS) {
            // 超时强制重置
            g_bridge_state = BRIDGE_IDLE; 
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
            if (g_bridge_state == BRIDGE_WAITING_REPLY)
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
