/* APP3
 * UART.c
 * Créateur :   paif1582 et RODL6305
 * Date :       7 février 2026
 * Revision :   1.0
 *
 * DESCRIPTION :
 *   UART4 en 9 bits, 2 stop. Envoi de tampons ADC (8 MSB ou 10 bits scindés).
 */

#include <xc.h>
#include <sys/attribs.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "config.h"

#include "UART.h"
#include "ADC.h"

// === Buffers RX (optionnel, pour démo stockage réception) ===
volatile uint16_t g_uartRxBuf[UART_RX_BUF_SIZE];
volatile size_t   g_uartRxWr = 0;
volatile size_t   g_uartRxRd = 0;

// --- Utils internes ---
static inline void uart_rxbuf_push(uint16_t v9)
{
    size_t nxt = (g_uartRxWr + 1u) % UART_RX_BUF_SIZE;
    if (nxt != g_uartRxRd) {  // simple anti-overrun
        g_uartRxBuf[g_uartRxWr] = v9;
        g_uartRxWr = nxt;
    }
}

// --- Parité impaire logicielle (sur 8 bits) ---
static inline uint8_t odd_parity_flag8(uint8_t d) {
    d ^= (uint8_t)(d >> 4);
    d ^= (uint8_t)(d >> 2);
    d ^= (uint8_t)(d >> 1);
    return (uint8_t)(~d) & 1u; // 1 => mettre MSB=1 pour rendre la parité impaire
}

// --- Emission d'un mot "9 bits" sur U4 (PDSEL=0b11) ---
static inline void UART4_PutChar9(uint16_t v9) {
    while (U4STAbits.UTXBF) { /* attendre de la place dans le FIFO TX */ }
    U4TXREG = v9; // en mode 9 bits, le bit 8 (MSB) est transmis aussi
}

void UART_Init(void)
{
    U4MODEbits.ON = 0;

    // Broches Basys MX3 : U4TX -> RF12 (sortie), U4RX <- RF13 (entrée)
    TRISFbits.TRISF12 = 0;
    TRISFbits.TRISF13 = 1;

    // PPS (codes à confirmer dans la datasheet du PIC32MX370)
    U4RXRbits.U4RXR   = 0b0100; // RF13 -> U4RX (vérifier code exact)
    RPF12Rbits.RPF12R = 0b0010; // U4TX -> RF12 (vérifier code exact)
    // Câblage inter-cartes : TX<->RX croisés + GND commun (guide APP). [1](https://usherbrooke-my.sharepoint.com/personal/paif1582_usherbrooke_ca/Documents/6-Autres/Fichiers%20Microsoft%20Copilot%20Chat/s4-ge_app4_guideetudiant_2026-hiver.pdf)

    // Baud ~115200 @ PBCLK~40MHz (BRGH=0 => ÷16, U4BRG=21, err ? -1.36 %)
    U4MODEbits.BRGH = 0;
    U4BRG = 21;

    // Trame : 9 bits (sans parité matérielle), 2 stop (exigences APP)
    U4MODEbits.PDSEL = 0b11;
    U4MODEbits.STSEL = 1;

    // Activer TX/RX avant ON (ordre recommandé par le guide). [1](https://usherbrooke-my.sharepoint.com/personal/paif1582_usherbrooke_ca/Documents/6-Autres/Fichiers%20Microsoft%20Copilot%20Chat/s4-ge_app4_guideetudiant_2026-hiver.pdf)
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;

    IFS2bits.U4RXIF = 0;
    IFS2bits.U4TXIF = 0;

    // IRQ RX (penser à vider tout le FIFO dans l'ISR)
    IEC2bits.U4RXIE = 1;
    IEC2bits.U4TXIE = 0;
    IPC9bits.U4IP   = 5;
    IPC9bits.U4IS   = 0;

    U4MODEbits.ON = 1;
}

// --- API "octet" via trame 9 bits ---
void UART4_PutByte(uint8_t b) {
    uint16_t v9 = (uint16_t)b;      // MSB=0
    UART4_PutChar9(v9);
}

void UART4_PutByteOddParity(uint8_t b) {
    uint16_t v9 = (uint16_t)b;
    v9 |= ((uint16_t)odd_parity_flag8(b) << 8);
    UART4_PutChar9(v9);
}

// --- Helpers 10 bits -> 8 MSB + 2 LSB ---
typedef struct { uint8_t msb8, lsb2; } packed10_t;

static inline packed10_t pack10(uint16_t sample10) {
    packed10_t p;
    p.msb8 = (uint8_t)(sample10 >> 2); // bits 9..2
    p.lsb2 = (uint8_t)(sample10 & 0x03); // bits 1..0
    return p;
}

static inline uint8_t top8_from_10bits(uint16_t sample10) {
    return (uint8_t)(sample10 >> 2);     // 8 MSB
}

// === Option 1 : n'envoyer que les 8 MSB ===
void UART4_SendADC_8MSB_in_9bit(const volatile uint16_t *adcBuf,
                                size_t count, bool withOddParity)
{
    size_t i;
    for (i = 0; i < count; i++) {
        uint8_t d8 = top8_from_10bits(adcBuf[i]);
        if (withOddParity) UART4_PutByteOddParity(d8);
        else               UART4_PutByte(d8);
    }
}

// === Option 2 : 10 bits -> (8 MSB) + (2 LSB) ===
void UART4_SendADC_10bits_in_9bit(const volatile uint16_t *adcBuf,
                                  size_t count, bool withOddParity)
{
    size_t i;
    for (i = 0; i < count; i++) {
        packed10_t p = pack10(adcBuf[i]);

        // Paquet 1 : 8 MSB
        if (withOddParity) UART4_PutByteOddParity(p.msb8);
        else               UART4_PutByte(p.msb8);

        // Paquet 2 : 2 LSB (dans bits[1:0], le reste = 0)
        if (withOddParity) UART4_PutByteOddParity(p.lsb2);
        else               UART4_PutByte(p.lsb2);
    }
}

// === Raccourci : envoi du buffer ADC global ===
void UART4_StartTransmitRecorded(bool full10bits, bool withOddParity)
{
    extern volatile uint16_t audioBuffer[BUFFER_SIZE];
    extern volatile int      ADC_index;

    size_t count = (size_t)ADC_index;
    if (count == 0) return;

    if (full10bits) UART4_SendADC_10bits_in_9bit(audioBuffer, count, withOddParity);
    else            UART4_SendADC_8MSB_in_9bit  (audioBuffer, count, withOddParity);
}

// === RX ISR : vide complètement le FIFO + (option) vérif parité logicielle ===
void __ISR(_UART_4_VECTOR, IPL5SOFT) U4RX_ISR(void)
{
    while (U4STAbits.URXDA) {
        uint16_t rx = U4RXREG;           // lit un mot 9 bits (bit8 = MSB/?parité soft?)

        // (Option) Vérif parité impaire logicielle si tu l'utilises :
        // uint8_t d8  = (uint8_t)(rx & 0xFF);
        // uint8_t msb = (uint8_t)((rx >> 8) & 1u);
        // if (msb != odd_parity_flag8(d8)) {
        //     // TODO: allumer LD5, log, etc.
        // }

        uart_rxbuf_push(rx);             // stocker pour traitement ultérieur si besoin
    }
    IFS2bits.U4RXIF = 0;                 // clear flag
}