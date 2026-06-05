/**
 * @file    mode_manager.c
 * @brief   设备模式管理器 — USB 字节序列检测 + 回调表驱动的模式切换
 * @note    0xAA 0xAA 0xAA → AA模式(标准ID + 65字节协议桥)
 *          0xBB 0xBB 0xBB → BB模式(扩展ID + 8字节单帧直发)
 */
#include "mode_manager.h"
#include "protocol.h"

/*===========================================================================
 * 模式处理回调表
 *===========================================================================*/
typedef struct {
    void (*enter)(void);
    void (*process)(void);
    void (*exit)(void);
} ModeHandler_t;

/* ---- AA 模式: 标准ID + 65字节协议桥 (enter/process/exit) ---- */

static void BridgeMode_Enter(void)
{
    Protocol_Bridge_Reset();
}

static void BridgeMode_Process(void)
{
    USB_Protocol_Parse_And_Bridge();
    CAN_Bridge_Sequence_Process();
}

static void BridgeMode_Exit(void)
{
    Protocol_Bridge_Reset();
}

/* ---- BB 模式: 扩展ID + 8字节单帧直发 ---- */

static void ExtendedMode_Enter(void)
{
    /* USB 接收回调中直接处理, 无需额外初始化 */
}

static void ExtendedMode_Process(void)
{
    /* USB 接收回调 Protocol_HandleSingleCanFrame 中直接发送 */
}

static void ExtendedMode_Exit(void)
{
    /* 无需清理 */
}

/*===========================================================================
 * 回调表
 *===========================================================================*/
static const ModeHandler_t g_mode_handlers[MODE_COUNT] = {
    [MODE_LEGACY]          = { BridgeMode_Enter,   BridgeMode_Process,   BridgeMode_Exit   },
    [MODE_PROTOCOL_BRIDGE] = { ExtendedMode_Enter, ExtendedMode_Process, ExtendedMode_Exit },
};

/*===========================================================================
 * 内部状态
 *===========================================================================*/
static DeviceMode_t  g_current_mode = MODE_LEGACY;
static bool          g_initialized  = false;

/* ---- 序列检测状态 ---- */
#define SEQ_TRIGGER_LEGACY          0xAAU
#define SEQ_TRIGGER_PROTOCOL_BRIDGE 0xBBU
#define SEQ_MATCH_COUNT             3U

/*===========================================================================
 * 内部函数
 *===========================================================================*/

/**
 * @brief  执行模式切换
 */
static void SwitchMode(DeviceMode_t target_mode)
{
    if (!g_initialized || target_mode == g_current_mode) {
        return;
    }

    /* 退出旧模式 */
    if (g_mode_handlers[g_current_mode].exit != NULL) {
        g_mode_handlers[g_current_mode].exit();
    }

    g_current_mode = target_mode;

    /* 进入新模式 */
    if (g_mode_handlers[g_current_mode].enter != NULL) {
        g_mode_handlers[g_current_mode].enter();
    }
}

/*===========================================================================
 * 公共接口
 *===========================================================================*/

/**
 * @brief  初始化模式管理器，默认进入 AA 模式(标准ID + 65字节协议桥)
 */
void ModeManager_Init(void)
{
    g_current_mode = MODE_LEGACY;
    g_initialized = true;

    if (g_mode_handlers[g_current_mode].enter != NULL) {
        g_mode_handlers[g_current_mode].enter();
    }
}

/**
 * @brief  获取当前工作模式
 */
DeviceMode_t ModeManager_GetCurrentMode(void)
{
    return g_current_mode;
}

/**
 * @brief  检测 USB 数据包中是否包含模式切换序列
 * @note   当前仅检测 3 字节全等序列 (0xAA 或 0xBB)
 */
bool ModeManager_DetectSequence(const uint8_t *buf, uint32_t len)
{
    if (buf == NULL || len != SEQ_MATCH_COUNT) {
        return false;
    }

    /* 检测 3 字节是否全部相同 */
    if (buf[0] != buf[1] || buf[1] != buf[2]) {
        return false;
    }

    switch (buf[0]) {
        case SEQ_TRIGGER_LEGACY:
            SwitchMode(MODE_LEGACY);
            return true;

        case SEQ_TRIGGER_PROTOCOL_BRIDGE:
            SwitchMode(MODE_PROTOCOL_BRIDGE);
            return true;

        default:
            return false;
    }
}

/**
 * @brief  运行当前模式的 process（每轮主循环调用）
 */
void ModeManager_Run(void)
{
    if (!g_initialized) {
        return;
    }

    if (g_mode_handlers[g_current_mode].process != NULL) {
        g_mode_handlers[g_current_mode].process();
    }
}

/**
 * @brief  外部请求切换到指定模式（供 CAN 接收回调等使用）
 */
void ModeManager_SwitchMode(DeviceMode_t target_mode)
{
    SwitchMode(target_mode);
}
