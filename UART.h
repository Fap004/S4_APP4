#ifndef _UART_H
#define _UART_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define UART_RX_BUF_SIZE 128    //buffer 8 BITS
#define UART_RX_BUF10_SIZE 128  //Buffer 10 BITS

extern volatile uint8_t uartRxBuf[UART_RX_BUF_SIZE];
extern volatile unsigned short uartRxWr;
extern volatile unsigned short uartRxRd;

#endif /* _UART_H */