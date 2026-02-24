#ifndef _UART_H
#define _UART_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define UART_RX_BUF_SIZE 128  // <-- taille du buffer 8-bit
#define UART_RX_BUF10_SIZE 128

extern volatile uint8_t uartRxBuf[UART_RX_BUF_SIZE];
extern volatile unsigned short uartRxWr;
extern volatile unsigned short uartRxRd;

extern volatile uint8_t g_txActive;
extern volatile uint8_t g_rxActive;

static inline int UART_RxAvailable(void) {
    return (uartRxWr != uartRxRd);
}

#endif /* _UART_H */