#ifndef _UART_RX_H
#define _UART_RX_H

#include <stdint.h>
#include <stdbool.h>
#include "UART.h"

/* (Option) Helpers de lecture ?live? contrôlée par drapeaux. */
extern volatile uint8_t g_playActive;     /* 1 = jouer via Timer 8 kHz */

bool uart_rx_pop(uint8_t *v);

#endif /* _UART_RX_H */