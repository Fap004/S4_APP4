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
extern  int scindillerMSB(unsigned int data);
extern  int scindillerLSB(unsigned int data);
 


/* ---------- Parité impaire logicielle (sur 8 bits) ---------- */
static inline uint8_t odd_parity8(uint8_t d) 
{
    d ^= (uint8_t)(d >> 4);
    d ^= (uint8_t)(d >> 2);
    d ^= (uint8_t)(d >> 1);
    return (uint8_t)(~d) & 1u;  // 1 => mettre MSB=1 pour rendre la parité impaire
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
    U4BRG = 14;         // 25 (115200 bps typique) 14=200 000

    U4MODEbits.PDSEL = 0b00; // 0b11 9 BITS parite
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

//volatile uint8_t idx = 0;
void UART4_SendSample(void)
{
    if (!U4STAbits.UTXBF)
    {
        if (PORTBbits.RB9 == 0) 
        {
            U4TXREG = (uint8_t)(test_buffer[test_index] >> 2);
        }
    
        else
        {
            U4TXREG=scindillerMSB(test_buffer[test_index]);
            U4TXREG=scindillerLSB(test_buffer[test_index]);
        }
    }
}

void UART4_SendRecording(void)
{
    if (!U4STAbits.UTXBF) 
    {
        if (PORTBbits.RB9 == 0)
        {
            U4TXREG = (uint8_t)(audioBuffer[ADC_index] >> 2);
        }
        else
        {
            U4TXREG=scindillerMSB(audioBuffer[ADC_index]);
            U4TXREG=scindillerLSB(audioBuffer[ADC_index]);
        }
    }
}

void UART4_SendIntercom(void)
{
    if (!U4STAbits.UTXBF) 
    {
        if (PORTBbits.RB9 == 0)
        {
            U4TXREG = (uint8_t)(ADC1BUF0 >> 2);
        }
        else
        {
            U4TXREG=scindillerMSB(ADC1BUF0);
            U4TXREG=scindillerLSB(ADC1BUF0);
        }
    }
}

