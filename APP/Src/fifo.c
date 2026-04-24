#include "fifo.h" // 包含FIFO相关的头文件定义
#include "stm32g4xx_hal.h"

// 定义发送和接收缓冲区的最大容量
#define USR_TX_BUFFER_SIZE 2048 // 发送缓冲区大小
#define USR_RX_BUFFER_SIZE 4096 // 接收缓冲区大小

// 实例化两个FIFO结构体对象：一个用于接收，一个用于发送
usr_fifo_t usr_rx_fifo, usr_tx_fifo;
// 开辟实际的内存空间（数组），用于存储FIFO的数据
uint8_t usr_tx_buf[USR_TX_BUFFER_SIZE];
uint8_t usr_rx_buf[USR_RX_BUFFER_SIZE];

/**
 * 初始化FIFO结构体
 * @param fifo: 指向FIFO结构体的指针
 * @param buf: 绑定的数组空间
 * @param length: 数组的长度
 */
void usr_fifo_init(usr_fifo_t *fifo, uint8_t *buf, uint16_t length)
{
  fifo->buf = buf;       // 绑定缓冲区地址
  fifo->length = length; // 设置缓冲区总长度
  fifo->head = 0;        // 初始化写指针（头）
  fifo->tail = 0;        // 初始化读指针（尾）
}

uint16_t usr_fifo_free(usr_fifo_t *fifo)
{
  return (uint16_t)(fifo->length - usr_fifo_used(fifo) - 1U);
}

/**
 * 从FIFO中读取一个字节
 * @param fifo: 指向FIFO结构体的指针
 * @param ch: 用于存储读取到的数据的指针
 * @return: 成功返回true(1)，FIFO为空返回false(0)
 */
uint8_t usr_fifo_read_ch(usr_fifo_t *fifo, uint8_t *ch)
{
  if (fifo->tail == fifo->head)
    return false;              // 如果读写指针相等，表示缓冲区为空
  *ch = fifo->buf[fifo->tail]; // 从尾指针指向的位置取出数据
  if (++fifo->tail >= fifo->length)
    fifo->tail = 0; // 尾指针后移，若超过数组长度则回绕到0
  return true;      // 读取成功
}

/**
 * 向FIFO中写入一个字节
 * @param fifo: 指向FIFO结构体的指针
 * @param ch: 要写入的数据
 * @return: 成功返回true(1)，缓冲区已满返回false(0)
 */
uint8_t usr_fifo_write_ch(usr_fifo_t *fifo, uint8_t ch)
{
  uint16_t h = fifo->head; // 暂存当前的头指针
  if (++h >= fifo->length)
    h = 0; // 计算下一个预期的头指针位置（处理回绕）
  if (h == fifo->tail)
    return false;             // 如果下一个位置等于尾指针，表示缓冲区已满
  fifo->buf[fifo->head] = ch; // 在当前头指针位置存入数据
  fifo->head = h;             // 更新头指针到新位置
  return true;                // 写入成功
}

/**
 * 获取FIFO中当前已存储的数据字节数
 */
uint16_t usr_fifo_used(usr_fifo_t *fifo)
{
  if (fifo->head >= fifo->tail)
    return fifo->head - fifo->tail; // 头在尾后，直接相减
  else
    return fifo->head + (fifo->length - fifo->tail); // 头在尾前（回绕了），计算两段长度之和
}

// --- 全局实例封装（供外部模块直接调用） ---

// 初始化全局的RX和TX FIFO
void USR_FIFO_INIT(void)
{
  usr_fifo_init(&usr_rx_fifo, usr_rx_buf, USR_RX_BUFFER_SIZE);
  usr_fifo_init(&usr_tx_fifo, usr_tx_buf, USR_TX_BUFFER_SIZE);
}

// 向接收FIFO批量写入数据（通常在串口中断中调用）
uint8_t USR_WRITE_RXFIFO(uint8_t *buf, uint16_t length)
{
	uint32_t primask = __get_PRIMASK(); // 备份当前中断状态
  uint8_t ok = true;
	__disable_irq(); // 关中断
	
  for (uint16_t i = 0; i < length; i++) {
    if (!usr_fifo_write_ch(&usr_rx_fifo, buf[i])) {
      ok = false;
      break;
    }
  }
	
	__set_PRIMASK(primask); // 恢复中断状态（比直接enable更安全，支持嵌套）
  return ok;
}

// 获取接收FIFO中当前有多少字节可用
uint16_t USR_RXFIFO_AVAILABLE(void) { return usr_fifo_used(&usr_rx_fifo); }

// 从接收FIFO中读取一个字节
uint8_t USR_READ_RXFIFO(void)
{
  uint8_t ch = 0;
  usr_fifo_read_ch(&usr_rx_fifo, &ch);
  return ch;
}

// 向发送FIFO批量写入数据（应用层准备发送数据时调用）
uint8_t USR_WRITE_TXFIFO(uint8_t *buf, uint16_t length)
{
  uint32_t primask = __get_PRIMASK();
  uint8_t ok = true;
  __disable_irq();

  for (uint16_t i = 0; i < length; i++) {
    if (!usr_fifo_write_ch(&usr_tx_fifo, buf[i])) {
      ok = false;
      break;
    }
  }

  __set_PRIMASK(primask);
  return ok;
}

// 获取发送FIFO中当前有多少字节待发送
uint16_t USR_TXFIFO_AVAILABLE(void)
{
  return usr_fifo_used(&usr_tx_fifo);
}

// 从发送FIFO中读取一个字节（通常由底层发送驱动/中断调用）
uint8_t USR_READ_TXFIFO(void)
{
  uint8_t ch = 0;
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  usr_fifo_read_ch(&usr_tx_fifo, &ch);
  __set_PRIMASK(primask);
  return ch;
}

// 批量从发送FIFO中提取数据到目标缓冲区
void USR_READ_TXFIFO_BUF(uint8_t *data, uint16_t len)
{
  uint32_t primask = __get_PRIMASK();
  __disable_irq();
  for (uint16_t i = 0; i < len; i++) {
    if (!usr_fifo_read_ch(&usr_tx_fifo, &data[i])) {
      break;
    }
  }
  __set_PRIMASK(primask);
}

/**
 * [核心新增] PEEK 实现
 * 功能：查看FIFO中的数据而不弹出（即不移动 tail 指针）
 * @param index: 距离当前 tail 指针的偏移量（0表示下一个要读的字节）
 * @return: 查找到的数据，若索引越界返回0
 */
uint8_t USR_FIFO_PEEK(uint16_t index)
{
  // 如果请求的偏移量超过了缓冲区现有数据量，则无效
  if (index >= USR_RXFIFO_AVAILABLE())
    return 0;
  // 计算在环形数组中的真实逻辑索引（当前尾部 + 偏移量，并取模防止越界）
  uint16_t real_idx = (usr_rx_fifo.tail + index) % usr_rx_fifo.length;
  return usr_rx_fifo.buf[real_idx]; // 返回该位置的数据
}

/**
 * 快速丢弃/跳过FIFO中的数据
 * @param len: 要跳过的字节数
 */
void USR_RXFIFO_FLUSH(uint16_t len) {
    uint16_t used = USR_RXFIFO_AVAILABLE();
    if(len > used) len = used;

    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    usr_rx_fifo.tail = (usr_rx_fifo.tail + len) % usr_rx_fifo.length;
    __set_PRIMASK(primask);
}
