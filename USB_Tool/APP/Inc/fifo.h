#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t* buf;
    uint16_t length;
    volatile uint16_t head;
    volatile uint16_t tail;
} usr_fifo_t;

extern usr_fifo_t usr_rx_fifo, usr_tx_fifo;
void usr_fifo_init(usr_fifo_t* fifo, uint8_t* buf, uint16_t length);
uint8_t usr_fifo_read_ch(usr_fifo_t* fifo, uint8_t* ch);
uint8_t usr_fifo_write_ch(usr_fifo_t* fifo, uint8_t ch);
uint16_t usr_fifo_free(usr_fifo_t* fifo);
uint16_t usr_fifo_used(usr_fifo_t* fifo);

// --- 全局 FIFO 操作 API ---
void USR_FIFO_INIT(void);
uint8_t USR_WRITE_RXFIFO(uint8_t* buf, uint16_t length);
uint8_t USR_READ_RXFIFO(void); // 读一个字节
uint16_t USR_RXFIFO_AVAILABLE(void);

uint8_t USR_WRITE_TXFIFO(uint8_t* buf, uint16_t length);
uint8_t USR_READ_TXFIFO(void);
uint16_t USR_TXFIFO_AVAILABLE(void);
void USR_READ_TXFIFO_BUF(uint8_t *data, uint16_t len);

uint8_t USR_FIFO_PEEK(uint16_t index); 

#endif











