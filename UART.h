#ifndef _UART_H
#define _UART_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ---------------- Paramètres UART partagés ----------------
   - BRG/BRGH choisis pour ~115200 bps par défaut.
   - Trame 9 bits (PDSEL=0b11), 2 stop bits (STSEL=1).
   - Sur Basys MX3 : U4TX -> RF12, U4RX <- RF13 (PPS requis).
   - Entre deux cartes : TX<->RX croisés + GND commun.
   --------------------------------------------------------- */

#define UART_PBCLK_HZ          (40000000UL)   /* ex. PBCLK ~ 40 MHz */
#define UART_BRGH               0              /* 0: ÷16 ; 1: ÷4    */
#define UART_BAUD               115200UL
#define UART_U4BRG_DEFAULT      21             /* ~115200 @ PBCLK~40 MHz, BRGH=0 */

#define UART_PDSEL_9BIT_NOHWPAR 0b11           /* 9 bits, pas de parité matérielle */
#define UART_STSEL_2STOP        1

/* ----- RX: buffer circulaire 8-bit (pour flux 8 MSB @ 8 kHz) ----- */
#ifndef UART_RX_BUF_SIZE
#  define UART_RX_BUF_SIZE 512  /* 512 tient ~64 ms @ 8kHz (8 MSB) */
#endif

/* Tampon RX visible côté application (déclaré/alloc. dans uart_rx.c) */
extern volatile uint8_t  uartRxBuf[UART_RX_BUF_SIZE];
extern volatile unsigned short uartRxWr;
extern volatile unsigned short uartRxRd;
// quelque part (ex. uart_common.h)
extern volatile uint8_t g_txActive;   // 1 => émet 1 octet/125us
extern volatile uint8_t g_rxActive;   // 1 => joue 1 octet/125us

/* Accès simple depuis le code application */
static inline int UART_RxAvailable(void) {
    return (uartRxWr != uartRxRd);
}

#endif /* _UART_H */