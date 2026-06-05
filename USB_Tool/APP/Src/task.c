#include "task.h"
#include "canfd.h"
#include "Fifo.h"
#include "protocol.h"
// 用于接收数据的缓存
FDCAN_RxHeaderTypeDef rxHeader1;
uint8_t rxData1[64];
uint8_t len;
/**
  * @brief  将 FDCAN_DLC_BYTES_x 宏转换为实际字节数
  * @param  dlc: rxHeader.DataLength 的值
  * @retval 实际字节数 (0-64)
  */
uint8_t FDCAN_DLC_To_Bytes(uint32_t dlc)
{
    switch (dlc) {
        case FDCAN_DLC_BYTES_0:  return 0;
        case FDCAN_DLC_BYTES_1:  return 1;
        case FDCAN_DLC_BYTES_2:  return 2;
        case FDCAN_DLC_BYTES_3:  return 3;
        case FDCAN_DLC_BYTES_4:  return 4;
        case FDCAN_DLC_BYTES_5:  return 5;
        case FDCAN_DLC_BYTES_6:  return 6;
        case FDCAN_DLC_BYTES_7:  return 7;
        case FDCAN_DLC_BYTES_8:  return 8;
        case FDCAN_DLC_BYTES_12: return 12;
        case FDCAN_DLC_BYTES_16: return 16;
        case FDCAN_DLC_BYTES_20: return 20;
        case FDCAN_DLC_BYTES_24: return 24;
        case FDCAN_DLC_BYTES_32: return 32;
        case FDCAN_DLC_BYTES_48: return 48;
        case FDCAN_DLC_BYTES_64: return 64;
        default: return 0; // 异常情况
    }
}

/**
  * @brief   FIFO0接收回调，用于处理CAN1接收的数据
  * @param  无
  * @retval 无
  */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	
    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) {
						
				    MavCan_CAN_Rx_Callback(hfdcan, RxFifo0ITs);

    }
}

/**
  * @brief 定时器1中断回调函数,1ms一次
					 定时器2中断回调函数,1s一次
  * @param  无
  * @retval 无
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
    if (htim->Instance == TIM1) {
		

		}						
    else if (htim->Instance == TIM2) {
			static uint8_t flag_led = 0;
			flag_led = !flag_led;
			HAL_GPIO_WritePin(GPIOA, LED_BLUE_Pin|LED_RED_Pin, flag_led);	


			}		
		
	
}

uint8_t uart_tx_buffer[300];
void Process_UART_Tx_Task(void)
{


		if(USR_TXFIFO_AVAILABLE())
		{
			uint16_t uart_send_len =USR_TXFIFO_AVAILABLE(); 
			for(uint8_t i = 0; i < uart_send_len; i++)
			{
				uart_tx_buffer[i] = USR_READ_TXFIFO();
			}
				HAL_UART_Transmit_DMA(&huart1,uart_tx_buffer,uart_send_len);		
		}
	

}

/**
  * @brief  UART 错误回调函数 (处理波特率错误导致的死锁)
  * @note   当波特率不对时，会触发 FE (帧错误) 或 NE (噪声错误)，
  *         HAL库会停止DMA。此函数负责重启DMA。
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // 1. 读取并清除错误代码 (虽然HAL已经处理了一部分，但为了保险)
        // 常见的错误：HAL_UART_ERROR_FE (帧错误), HAL_UART_ERROR_ORE (溢出)
        uint32_t error_code = HAL_UART_GetError(huart);
        
        // 2. 清除溢出标志 (Overrun Error 是最容易锁死接收的)
        __HAL_UART_CLEAR_OREFLAG(huart);
        
        // 3. 强制重启 DMA 接收
        // 注意：先停止以复位状态机，再重新开启
        HAL_UART_DMAStop(huart);
        HAL_UART_Receive_DMA(huart, uart1_dma_rx_buf, DMA_RX_BUF_SIZE);
    }
}
