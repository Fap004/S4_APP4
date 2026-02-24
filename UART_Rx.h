#ifndef _UART_RX_H
#define _UART_RX_H

#include <stdint.h>
#include <stdbool.h>
#include "UART.h"

bool uart_rx_pop(uint8_t *v);

#endif /* _UART_RX_H */