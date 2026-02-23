// uart_tx.c
#include <xc.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "config.h"

#include "UART_Tx.h"
#include "ADC.h"   // audioBuffer[], ADC_index
#include "test.h"

volatile size_t test_tx_index = 0;   // indice courant dans test_buffer

/* ---------- Parité impaire logicielle (sur 8 bits) ---------- */
static inline uint8_t odd_parity8(uint8_t d) {
    d ^= (uint8_t)(d >> 4);
    d ^= (uint8_t)(d >> 2);
    d ^= (uint8_t)(d >> 1);
    return (uint8_t)(~d) & 1u;  // 1 => mettre MSB=1 pour rendre la parité impaire
}

/* ---------- Émission d?un mot 9 bits (PDSEL=0b11) ---------- */
static inline void UART4_PutChar9(uint16_t v9) {
    while (U4STAbits.UTXBF) { /* attendre de la place dans le FIFO TX */ }
    U4TXREG = v9;             // en 9-bit mode, le bit 8 (MSB) est transmis
}

/* ---------- Initialisation UART4 ---------- */
void UART_Init(void)
{
    U4MODEbits.ON = 0;

    // Basys MX3 : U4TX -> RF12 (sortie), U4RX <- RF13 (entrée)
    TRISFbits.TRISF12 = 0;
    TRISFbits.TRISF13 = 1;

    // PPS (codes à confirmer dans la datasheet device)
    U4RXRbits.U4RXR   = 0b1001;  // RF13 -> U4RX  (vérifier code exact pour PIC32MX370)0b1001
    RPF12Rbits.RPF12R = 0b0010;  // U4TX -> RF12  (vérifier code exact pour PIC32MX370)0b0010;
    // Entre 2 cartes : TX<->RX croisés + GND commun.

    // Baud ~115200 @ PBCLK 48 MHz
    U4MODEbits.BRGH = 0;        // 0
    U4BRG = 25;         // 25 (115200 bps typique)

    U4MODEbits.PDSEL = 0b00; // 0b11
    U4MODEbits.STSEL = 1;        // 1

    // Activer TX/RX AVANT ON (ordre recommandé par le FRM)
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;

    // Nettoyage flags + config IRQ RX (l?ISR est dans uart_rx.c)
    IFS2bits.U4RXIF = 0;
    IFS2bits.U4TXIF = 0;
    IEC2bits.U4RXIE = 1;   // RX interrupt ON (réception en ISR)
    IEC2bits.U4TXIE = 0;   // TX interrupt OFF (TX en polling)
    IPC9bits.U4IP   = 5;   // doit matcher IPL5SOFT de l?ISR RX
    IPC9bits.U4IS   = 0;

    U4MODEbits.ON = 1;
}

/* ---------- API émission "octet" ---------- */
void UART4_PutByte(uint8_t b) {
    UART4_PutChar9((uint16_t)b); // MSB=0 (pas de parité logicielle)
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

// Renvoie true si tout le buffer a été transmis, false sinon
bool UART4_SendTestBuffer_8MSB_NB(bool withOddParity)
{
    if (test_tx_index >= BUFFER_SIZE_TEST)
    {
        test_tx_index = 0;      // reset pour la prochaine fois
        return true;            // tout envoyé
    }

    uint8_t d8 = (uint8_t)(test_buffer[test_tx_index] >> 2);  // 8 MSB
    if (withOddParity)
    {
        UART4_PutByteOddParity(d8);
    }
    else
    {
        UART4_PutByte(d8);
    }
    test_tx_index++;           // passer au prochain octet
    return false;              // toujours en cours
}

void UART4_SendTestBufferBlocking(void)
{
    int i;
    for (i = 0; i < BUFFER_SIZE_TEST; i++) 
    {
        uint8_t d8 = (uint8_t)(test_buffer[i] >> 2);  // 8 MSB

        // attendre que le FIFO TX ait de la place
        while (U4STAbits.UTXBF);

        // envoyer le byte (sans parité)
        U4TXREG = d8;

        // pour simuler 8kHz, délai ~125 µs
        unsigned int tStart = _CP0_GET_COUNT();
        while ((_CP0_GET_COUNT() - tStart) < 300000);  // ajuster selon PBCLK
    }
}

//volatile uint8_t idx = 0;
void UART4_SendSample(void)
{
    if (!U4STAbits.UTXBF) 
    {
        U4TXREG = (uint8_t)(test_buffer[test_index] >> 2);
    }
}

void UART4_SendRecording(void)
{
    if (!U4STAbits.UTXBF) 
    {
        U4TXREG = (uint8_t)(audioBuffer[ADC_index] >> 2);
    }
}

void UART4_SendIntercom(void)
{
    if (!U4STAbits.UTXBF) 
    {
        U4TXREG = (uint8_t)(ADC1BUF0 >> 2);
    }
}

