# CLAUDE.md — 软件程序 嵌入式开发工作区

## 项目概览

基于 CAN 总线的 IAP Bootloader 解决方案，支持多个 MCU 平台。

## 子项目

### 1. Bootloader — STM32F103RC (主项目)
| 项目 | 详情 |
|------|------|
| MCU | STM32F103RC (Cortex-M3, 72MHz, 256KB Flash) |
| 工程 | `Bootloader/MDK-ARM/Project.uvprojx` |
| 编译器 | Armcc V5.06 |
| 输出 | `Bootloader/Bin/Bootloader_Driver_V1.0.0.1.bin` (6.3KB) |
| 库 | STM32F10x 标准外设库 + CMSIS Cortex-M3 |

**源文件 (`Bootloader/src/`):**
`main.c`, `IAP.c`, `CAN.c`, `Flash.c`, `Parse_ComData.c`, `System_Config.c`, `Universal_Init.c`, `variable.c`

### 2. Bootloader_test — AT32F456CEU7 (移植版)
| 项目 | 详情 |
|------|------|
| MCU | AT32F456CEU7 (Cortex-M4F, 192MHz) |
| 工程 | `Bootloader_test/Mdk/Bootloader.uvprojx` |
| 输出 | `Bootloader_test/Mdk/bootloader.bin` |

**源文件:** 与 Bootloader 架构相同 — `IAP.c`, `Flash.c`, `bl_can.c`, `Parse_ComData.c`

### 3. N32 SDK — 官方例程库
- **位置:** `N32/`
- **版本:** Nations.N32H47x_48x_Library.1.2.0
- **内容:** 外设示例 (ADC/DAC/ETH/USB/TIM/UART...), FreeRTOS 中间件
- **用途:** 参考实现，向 N32H473 移植的官方代码参考

### 4. CAN_OTA V1.1 — Windows 上位机
- **位置:** `CAN_OTA V1.1/`
- **程序:** `CAN_OTA.exe` — 通过 CAN 卡向设备推送固件
- **依赖:** `ControlCANFD.dll`
- **CAN 波特率:** 250 Kbps

## CAN 通信参数

- 250 Kbps, 29-bit J1939 扩展 ID
- STM32: bxCAN (CAN1)
- AT32: bxCAN (CAN1)
- N32: FDCAN1

## 项目关系

```
Bootloader (STM32F103) ──移植──→ Bootloader_test (AT32F456)
                                        │
                                  继续移植 ↓
                              N32 SDK ← 参考
                                        │
                                   N32_WorkPlace/test (N32H473)
```
