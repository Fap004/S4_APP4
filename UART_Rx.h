#ifndef _UART_RX_H
#define _UART_RX_H

#include <stdint.h>
#include <stdbool.h>
#include "UART.h"

/* Pop 1 octet du buffer RX (retourne 1 si OK, 0 si vide). */
int uart_rx_pop(uint8_t *out);

/* (Option) Helpers de lecture ?live? contrôlée par drapeaux. */
extern volatile uint8_t g_playActive;     /* 1 = jouer via Timer 8 kHz */
void Stream_Start(void);                  /* Reset FIFO + active lecture live */
void Stream_Stop(void);                   /* Désactive lecture live         */

/* NB: L?ISR UART4 RX est DÉFINIE dans uart_rx.c (UNE seule ISR pour _UART_4_VECTOR). */
/* NB: L?ISR Timer 8 kHz qui dépile 1 échantillon/tick est dans ton module timers/play. */

#endif /* _UART_RX_H */