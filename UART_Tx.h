#ifndef _UART_TX_H
#define _UART_TX_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "UART.h"


/* Initialise UART4 (BRG/BRGH, PDSEL=9-bit, STSEL=2 stop, PPS RF12/RF13, UTXEN/URXEN). 
   NB: L?ISR RX est définie dans uart_rx.c ; cette init active l?IRQ RX. */
void UART_Init(void);

/* Envoi d?un octet ?données 8 bits? via trame 9 bits (MSB=0). */
void UART4_PutByte(uint8_t b);

/* Envoi d?un octet avec parité impaire logicielle dans le 9e bit (MSB). */
void UART4_PutByteOddParity(uint8_t b);

/* Envoi d?un buffer ADC 10 bits en ?8 MSB? (1 octet/échantillon). */
void UART4_SendADC_8MSB_in_9bit(const volatile uint16_t *adcBuf,
                                size_t count, bool withOddParity);

/* Envoi d?un buffer ADC 10 bits ?scindé? (MSB8 + LSB2) ? 2 octets/échantillon. */
void UART4_SendADC_10bits_in_9bit(const volatile uint16_t *adcBuf,
                                  size_t count, bool withOddParity);

/* Raccourci : envoie le buffer ADC global (défini dans ADC.c). */
void UART4_StartTransmitRecorded(bool full10bits, bool withOddParity);

bool UART4_SendTestBuffer_8MSB_NB(bool withOddParity);

#endif /* _UART_TX_H */