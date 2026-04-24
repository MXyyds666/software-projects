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

static uint8_t  g_bridge_buf_tx[65];    
static uint8_t  g_bridge_buf_rx[65];    
static volatile uint8_t g_rx_count = 0;          
static volatile uint32_t g_timeout_tick = 0;  

// 声明外部时间戳变量（已在 usbd_cdc_if.c 更新）
extern uint32_t g_last_usb_rx_tick;

//#define CAN_SEND_ID    0x781    
extern uint16_t SEND_ID;
#define CAN_TIMEOUT_MS 200               // 1ms频率下建议设为 200-500ms
#define USB_SYNC_TIMEOUT_MS 10          

/**
 * @brief 最终组包回传 USB (增加安全性)
 */
static void Finalize_And_Send_To_USB(void) {
    // 只有确实收到了数据才发回，防止空回传
    if (g_rx_count > 0) {
        // 这里的 USR_WRITE_TXFIFO 仅仅是存入缓冲区
        // 真正的发送由 main 循环里的 USB_CDC_TX_Bridge 处理
        USR_WRITE_TXFIFO(g_bridge_buf_rx, 65);
    }
    
    g_rx_count = 0;
    g_bridge_state = BRIDGE_IDLE; 
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
    if (avail > 0 && avail < 65) {
        if (HAL_GetTick() - g_last_usb_rx_tick > USB_SYNC_TIMEOUT_MS) {
            while(USR_RXFIFO_AVAILABLE() > 0) USR_READ_RXFIFO();
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

    if (avail < 65) return; 

    // 提取 65 字节
    for (int i = 0; i < 65; i++) {
        g_bridge_buf_tx[i] = USR_READ_RXFIFO();
    }
	
    memset(g_bridge_buf_rx, 0, 65); 
    g_rx_count = 0;

    // 清空 CAN 接收 FIFO
    FDCAN_RxHeaderTypeDef dummy_header;
    uint8_t dummy_data[8];
    while (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &dummy_header, dummy_data) == HAL_OK);

    g_bridge_state = BRIDGE_SENDING;
}

/**
 * @brief CAN 发送与超时管理
 */
void CAN_Bridge_Sequence_Process(void)
{
    if (g_bridge_state == BRIDGE_SENDING)
    {
        for (uint8_t i = 0; i < 9; i++) 
        {
            // 增加死等保护，确保 9 帧全发出去
            uint32_t wait_start = HAL_GetTick();
            while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) == 0) {
                if (HAL_GetTick() - wait_start > 10) break; // 10ms 安全跳出
            }

            FDCAN_TxHeaderTypeDef TxHeader;
            TxHeader.Identifier = SEND_ID;
            TxHeader.IdType = FDCAN_STANDARD_ID;
            TxHeader.TxFrameType = FDCAN_DATA_FRAME;
            TxHeader.DataLength = (i < 8) ? FDCAN_DLC_BYTES_8 : FDCAN_DLC_BYTES_1;
            TxHeader.FDFormat = FDCAN_CLASSIC_CAN;

            uint8_t *p_data = (i < 8) ? &g_bridge_buf_tx[i * 8] : &g_bridge_buf_tx[64];
            HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, p_data);
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
                if (g_rx_count < 8) {
                    memcpy(&g_bridge_buf_rx[g_rx_count * 8], rx_data, 8);
                } else if (g_rx_count == 8) {
                    g_bridge_buf_rx[64] = rx_data[0]; 
                }
                
                g_rx_count++;
                
                if (g_rx_count >= 9) {
                    Finalize_And_Send_To_USB();
                }
            }
        }
    }
}
