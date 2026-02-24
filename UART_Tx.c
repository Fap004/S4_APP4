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

volatile uint8_t tx_subindex=0;   // 0 = MSB, 1 = LSB
volatile size_t test_tx_index = 0;   // indice courant dans test_buffer
extern  int scindillerMSB(unsigned int data);
extern  int scindillerLSB(unsigned int data);
extern  int ajout_parite_odd (unsigned int data);

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

    U4MODEbits.PDSEL = 0b11; // 0b11 9 BITS parite 0b00
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
    if (PORTBbits.RB9 == 0)
    {
        //MODE 8 BITS
        U4TXREG = ajout_parite_odd(test_buffer[test_index] >> 2);
        test_index++;
    }
    else
    {
        //MODE 10 BITS
        if (tx_subindex == 0)
        {
            U4TXREG = ajout_parite_odd(scindillerMSB(test_buffer[test_index]));
            tx_subindex = 1;
        }
        else
        {
            U4TXREG = ajout_parite_odd(scindillerLSB(test_buffer[test_index]));
            tx_subindex = 0;
            test_index++;
        }
    }
}

void UART4_SendRecording(void)
{
    if (PORTBbits.RB9 == 0) 
    {
        //MODe 8 BITS
        U4TXREG = ajout_parite_odd((audioBuffer[ADC_index] >> 2));
    }
    else 
    {
        // MODE 10 BITS
        if (tx_subindex == 0)
        {
            U4TXREG = ajout_parite_odd(scindillerMSB(audioBuffer[ADC_index]));
            tx_subindex = 1;
        }
        else
        {
            U4TXREG = ajout_parite_odd(scindillerLSB(audioBuffer[ADC_index]));
            tx_subindex = 0;
        }
    }
}

void UART4_SendIntercom_Sample(uint16_t sample10)
{
    if (PORTBbits.RB9 == 0)
    {
        // MODE 8 BITS
        if (!U4STAbits.UTXBF)
        {
            U4TXREG = ajout_parite_odd((uint8_t)(sample10 >> 2));
        }
    }
    else
    {
        // MODE 10 BITS
        if (!U4STAbits.UTXBF)
        {
            U4TXREG = ajout_parite_odd(scindillerMSB(sample10));
        }
        if (!U4STAbits.UTXBF)
        {
            U4TXREG = ajout_parite_odd(scindillerLSB(sample10));
        }
    }
}

