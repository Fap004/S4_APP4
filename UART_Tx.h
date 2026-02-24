#ifndef _UART_TX_H
#define _UART_TX_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "UART.h"

void UART_Init(void);

static inline uint8_t odd_parity8(uint8_t d);

void UART4_SendSample(void);

void UART4_SendRecording(void);

void UART4_SendIntercom(void);

#endif /* _UART_TX_H */