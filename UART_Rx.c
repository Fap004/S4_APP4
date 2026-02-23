// uart_rx.c
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

#include "UART_Rx.h"
#include "config.h"

/* ---------- Buffer circulaire RX 8-bit (1 octet = 1 échantillon 8 MSB) ---------- */
volatile uint8_t  uartRxBuf[UART_RX_BUF_SIZE];
volatile unsigned short uartRxWr = 0;
volatile unsigned short uartRxRd = 0;

/* ---------- Drapeaux "lecture live" (optionnels) ---------- */
volatile uint8_t g_playActive = 0;

/* ---------- Helpers FIFO ---------- */
static inline unsigned short nxt(unsigned short x){ return (unsigned short)((x + 1u) % UART_RX_BUF_SIZE); }
static inline void push8(uint8_t v){
    unsigned short n = nxt(uartRxWr);
    if (n == uartRxRd) { uartRxRd = nxt(uartRxRd); } // drop oldest si plein (politique simple)
    uartRxBuf[uartRxWr] = v; uartRxWr = n;
}

/* ---------- (Option) parité impaire logicielle ---------- */
static inline uint8_t odd_parity8(uint8_t d) {
    d ^= (uint8_t)(d >> 4);
    d ^= (uint8_t)(d >> 2);
    d ^= (uint8_t)(d >> 1);
    return (uint8_t)(~d) & 1u;
}

int uart_rx_pop(uint8_t *out)
{
    if (uartRxWr == uartRxRd) {
        return 0;   // FIFO vide
    }

    *out = uartRxBuf[uartRxRd];
    uartRxRd = nxt(uartRxRd);
    return 1;       // succès
}


void __ISR(_UART_4_VECTOR, IPL5SOFT) U4RX_ISR(void)
{
    /* 1) OERR : si overrun, la RX est bloquée tant qu'on ne met pas OERR=0 */
    if (U4STAbits.OERR) 
    {
        U4STAbits.OERR = 0;  /* relance la réception */
        /* (facultatif) compter l'overrun ici */
    }

    /* 2) Vider TOUT le FIFO matériel tant que URXDA = 1 (obligatoire) */
    while (U4STAbits.URXDA) 
    {
        uint16_t rx = U4RXREG;             /* mot 9 bits si PDSEL=0b11 */
        uint8_t  d8 = (uint8_t)(rx & 0xFF);
        uint8_t  msb = (uint8_t)((rx >> 8) & 1u);

        /* (Option APP) Vérif parité impaire logicielle : MSB doit = odd_parity8(d8) */
        // if (msb != odd_parity8(d8)) { /* allumer LD5 / incrémenter un compteur */ }

        /* (Option diag) PERR/FERR : la simple lecture de U4RXREG purge le flag pour l'octet lu */
        // if (U4STAbits.FERR) { /* compter / filtrer si besoin */ }
        // if (U4STAbits.PERR) { /* compter / filtrer si besoin */ }

        /* 3) Pousser l'octet utile dans la FIFO logicielle 8-bit */
        push8(d8);
    }

    /* 4) Clear du flag d'interruption (après avoir tout vidé) */
    IFS2bits.U4RXIF = 0;
}