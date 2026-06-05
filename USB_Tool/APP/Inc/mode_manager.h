/**
 * @file    mode_manager.h
 * @brief   设备模式管理器 - 通过 USB 字节序列切换工作模式
 * @note    0xAA 0xAA 0xAA → 原始模式, 0xBB 0xBB 0xBB → 协议转化模式
 */
#ifndef __MODE_MANAGER_H__
#define __MODE_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>

/*===========================================================================
 * 模式枚举
 *===========================================================================*/
typedef enum {
    MODE_LEGACY           = 0x00U,   /**< 原始模式: 0xAA 0xAA 0xAA 触发 */
    MODE_PROTOCOL_BRIDGE  = 0x01U,   /**< 协议转化模式: 0xBB 0xBB 0xBB 触发 */
    MODE_COUNT                       /**< 模式总数 */
} DeviceMode_t;

/*===========================================================================
 * API 函数
 *===========================================================================*/

/**
 * @brief  初始化模式管理器，默认进入协议转化模式
 * @note   应在 main() 初始化阶段调用一次
 */
void ModeManager_Init(void);

/**
 * @brief  获取当前工作模式
 */
DeviceMode_t ModeManager_GetCurrentMode(void);

/**
 * @brief  检测 USB 数据包中是否包含模式切换序列
 * @param  buf   接收到的数据缓冲区
 * @param  len   数据长度（字节）
 * @retval true  检测到切换序列，已执行模式切换
 * @retval false 未检测到切换序列，数据应正常处理
 * @note   应在 CDC_Receive_FS() 中数据处理之前调用
 *         当前仅当 len==3 且 3 字节全部相同(0xAA 或 0xBB)时触发切换
 */
bool ModeManager_DetectSequence(const uint8_t *buf, uint32_t len);

/**
 * @brief  运行当前模式的 process 函数 (每轮主循环调用)
 */
void ModeManager_Run(void);

/**
 * @brief  外部请求切换到指定模式
 * @param  target_mode  目标模式
 */
void ModeManager_SwitchMode(DeviceMode_t target_mode);

#endif /* __MODE_MANAGER_H__ */
