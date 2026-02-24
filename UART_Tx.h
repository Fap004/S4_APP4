#ifndef _UART_TX_H
#define _UART_TX_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "UART.h"

extern volatile uint8_t tx_index;   // 0 = MSB, 1 = LSB

void UART_Init(void);

void UART4_SendSample(void);

void UART4_SendRecording(void);

void UART4_SendIntercom(void);

#endif /* _UART_TX_H */