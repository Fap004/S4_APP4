#ifndef _UART_H
#define _UART_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// --- Init UART4 : 9 bits (PDSEL=0b11), 2 stop (STSEL=1), ~115200 bps (BRGH=0, BRG=21)
void UART_Init(void);

// --- Emission "octet" via trames 9 bits (bit 8 = 0 ou parité impaire logicielle)
void UART4_PutByte(uint8_t b);          // 8 bits, MSB=0
void UART4_PutByteOddParity(uint8_t b); // 8 bits + parité impaire logicielle en bit 8

// --- Envoi du tampon ADC (10 bits) vers UART (mode 9 bits)
// Option 1 : n'envoyer que les 8 MSB (débit moindre)
void UART4_SendADC_8MSB_in_9bit(const volatile uint16_t *adcBuf,
                                size_t count, bool withOddParity);

// Option 2 : scinder 10 bits -> (8 MSB) + (2 LSB)
void UART4_SendADC_10bits_in_9bit(const volatile uint16_t *adcBuf,
                                  size_t count, bool withOddParity);

// Raccourci : envoi du buffer ADC global (défini dans ADC.c)
void UART4_StartTransmitRecorded(bool full10bits, bool withOddParity);

// (Optionnel) Buffer RX si tu veux stocker ce que tu reçois
#define UART_RX_BUF_SIZE  512//2048
extern volatile uint16_t g_uartRxBuf[UART_RX_BUF_SIZE]; // mot 9 bits (bit8=MSB)
extern volatile size_t   g_uartRxWr;
extern volatile size_t   g_uartRxRd;

#endif // _UART_H