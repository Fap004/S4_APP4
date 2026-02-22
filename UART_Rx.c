// uart_rx.c
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

//#include "UART.h"
#include "UART_Rx.h"
#include "config.h"

/* ---------- Buffer circulaire RX 8-bit (1 octet = 1 échantillon 8 MSB) ---------- */
volatile uint8_t  uartRxBuf[UART_RX_BUF_SIZE];
volatile unsigned short uartRxWr = 0;
volatile unsigned short uartRxRd = 0;

/* ---------- Drapeaux "lecture live" (optionnels) ---------- */
volatile uint8_t g_playActive = 0;

/* ---------- Helpers FIFO ---------- */
static inline unsigned short nxt(unsigned short x){ return (unsigned short)((x+1u)%UART_RX_BUF_SIZE); }
static inline void push8(uint8_t v){
    unsigned short n = nxt(uartRxWr);
    if (n == uartRxRd) { uartRxRd = nxt(uartRxRd); } // drop oldest si plein
    uartRxBuf[uartRxWr] = v; uartRxWr = n;
}

/* ---------- API pop pour le Timer 8 kHz ---------- */
int UART_RxPop(uint8_t *out)
{
    if (uartRxWr == uartRxRd) return 0;
    *out = uartRxBuf[uartRxRd];
    uartRxRd = nxt(uartRxRd);
    return 1;
}

/* ---------- Start/Stop lecture live ---------- */
void Stream_Start(void)
{
    __builtin_disable_interrupts();
    uartRxWr = uartRxRd = 0;
    __builtin_enable_interrupts();
    g_playActive = 1;
}
void Stream_Stop(void)
{
    g_playActive = 0;
}

/* ---------- (Option) parité impaire logicielle ---------- */
static inline uint8_t odd_parity8(uint8_t d) {
    d ^= (uint8_t)(d >> 4);
    d ^= (uint8_t)(d >> 2);
    d ^= (uint8_t)(d >> 1);
    return (uint8_t)(~d) & 1u;
}

/* ---------- ISR UART4 RX (UNE seule pour _UART_4_VECTOR) ---------- */
void __ISR(_UART_4_VECTOR, IPL5SOFT) U4RX_ISR(void)
{
    // Gérer un overrun (sinon RX se bloque)
    if (U4STAbits.OERR) { (void)U4RXREG; U4STAbits.OERR = 0; }

    while (U4STAbits.URXDA) {
        uint16_t rx = U4RXREG;             // mot 9 bits si PDSEL=0b11
        uint8_t  d8 = (uint8_t)(rx & 0xFF);
        uint8_t  msb = (uint8_t)((rx >> 8) & 1u);

        // (Option APP) Vérif parité impaire logicielle, LD5 si erreur
        // if (msb != odd_parity8(d8)) { /* allumer LD5 */ }

        push8(d8);
    }

    // (Option diag) if (U4STAbits.FERR) { /* framing error */ }
    // (Option diag) if (U4STAbits.PERR) { /* parity error HW */ }

    IFS2bits.U4RXIF = 0;  // clear flag
    // NB : vider tout le FIFO (URXDA) + clear le flag = obligatoire. [1](https://usherbrooke-my.sharepoint.com/personal/paif1582_usherbrooke_ca/Documents/6-Autres/Fichiers%20Microsoft%20Copilot%20Chat/s4-ge_app4_guideetudiant_2026-hiver.pdf)
}
