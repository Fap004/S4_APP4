// uart_tx.c
#include <xc.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "config.h"

//#include "UART.h"
#include "UART_Tx.h"
#include "ADC.h"   // audioBuffer[], ADC_index

/* ---------- Parité impaire logicielle (sur 8 bits) ---------- */
static inline uint8_t odd_parity8(uint8_t d) {
    d ^= (uint8_t)(d >> 4);
    d ^= (uint8_t)(d >> 2);
    d ^= (uint8_t)(d >> 1);
    return (uint8_t)(~d) & 1u;  // 1 => mettre MSB=1 pour rendre la parité impaire
}

/* ---------- Émission d?un mot 9 bits (PDSEL=0b11) ---------- */
static inline void UART4_PutChar9(uint16_t v9) {
    while (U4STAbits.UTXBF) { }  // attendre de la place dans le FIFO TX
    U4TXREG = v9;                // en 9-bit mode, le bit 8 (MSB) est transmis
}

/* ---------- Initialisation UART4 ---------- */
void UART_Init(void)
{
    U4MODEbits.ON = 0;

    // Basys MX3 : U4TX -> RF12 (sortie), U4RX <- RF13 (entrée)
    TRISFbits.TRISF12 = 0;
    TRISFbits.TRISF13 = 1;

    // PPS (codes à confirmer dans la datasheet PIC32MX370)
    U4RXRbits.U4RXR   = 0b0100;  // RF13 -> U4RX (vérifier le code exact)
    RPF12Rbits.RPF12R = 0b0010;  // U4TX -> RF12 (vérifier le code exact)
    // Entre deux cartes : TX<->RX croisés + GND commun.
    
    // Baud ~115200 @ PBCLK~40MHz (BRGH=0 => ÷16)
    U4MODEbits.BRGH = UART_BRGH;      // 0
    U4BRG = UART_U4BRG_DEFAULT;       // 21 (~115200 bps)

    // Trame : 9 bits (sans parité matérielle), 2 stop (exigences APP)
    U4MODEbits.PDSEL = UART_PDSEL_9BIT_NOHWPAR; // 0b11
    U4MODEbits.STSEL = UART_STSEL_2STOP;        // 1

    // Activer TX/RX avant ON (ordre recommandé
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;

    IFS2bits.U4RXIF = 0;
    IFS2bits.U4TXIF = 0;

    // Activer l'IRQ RX (l?ISR est dans uart_rx.c)
    IEC2bits.U4RXIE = 1;
    IEC2bits.U4TXIE = 0;
    IPC9bits.U4IP   = 5;
    IPC9bits.U4IS   = 0;

    U4MODEbits.ON = 1;
}

/* ---------- API émission "octet" ---------- */
void UART4_PutByte(uint8_t b) {
    UART4_PutChar9((uint16_t)b); // MSB=0
}

void UART4_PutByteOddParity(uint8_t b) {
    uint16_t v9 = (uint16_t)b;
    v9 |= ((uint16_t)odd_parity8(b) << 8);
    UART4_PutChar9(v9);
}

/* ---------- Helpers 10 bits -> 8 MSB + 2 LSB ---------- */
typedef struct { uint8_t msb8, lsb2; } packed10_t;

static inline uint8_t top8_from_10bits(uint16_t s10) { return (uint8_t)(s10 >> 2); }
static inline packed10_t pack10(uint16_t s10) {
    packed10_t p; p.msb8 = (uint8_t)(s10 >> 2); p.lsb2 = (uint8_t)(s10 & 0x03); return p;
}

/* ---------- Envoi buffer ADC en 8 MSB (1 octet/échantillon) ---------- */
void UART4_SendADC_8MSB_in_9bit(const volatile uint16_t *adcBuf, size_t count, bool withOddParity)
{
    size_t i;
    for (i = 0; i < count; i++) {
        uint8_t d8 = top8_from_10bits(adcBuf[i]);
        if (withOddParity) UART4_PutByteOddParity(d8);
        else               UART4_PutByte(d8);
    }
}

/* ---------- Envoi buffer ADC "10 bits scindés" (2 octets/échantillon) ---------- */
void UART4_SendADC_10bits_in_9bit(const volatile uint16_t *adcBuf, size_t count, bool withOddParity)
{
    size_t i;
    for (i = 0; i < count; i++) {
        packed10_t p = pack10(adcBuf[i]);
        if (withOddParity) { UART4_PutByteOddParity(p.msb8); UART4_PutByteOddParity(p.lsb2); }
        else               { UART4_PutByte(p.msb8);         UART4_PutByte(p.lsb2);         }
    }
}

/* ---------- Raccourci : envoyer le buffer ADC global ---------- */
void UART4_StartTransmitRecorded(bool full10bits, bool withOddParity)
{
    extern volatile uint16_t audioBuffer[BUFFER_SIZE];
    extern volatile int      ADC_index;

    size_t count = (size_t)ADC_index;
    if (count == 0) return;

    if (full10bits) UART4_SendADC_10bits_in_9bit(audioBuffer, count, withOddParity);
    else            UART4_SendADC_8MSB_in_9bit  (audioBuffer, count, withOddParity);
}