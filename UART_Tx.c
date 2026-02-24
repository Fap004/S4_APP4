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

volatile uint8_t tx_index=0;   // 0 = MSB, 1 = LSB
extern  int scindillerMSB(unsigned int data);
extern  int scindillerLSB(unsigned int data);
extern  int ajout_parite_odd (unsigned int data);

//Initialisation UART4
void UART_Init(void)
{
    U4MODEbits.ON = 0;

    // déclaration pin
    TRISFbits.TRISF12 = 0;
    TRISFbits.TRISF13 = 1;

    U4RXRbits.U4RXR   = 0b1001;  // RF13 -> U4RX
    RPF12Rbits.RPF12R = 0b0010;  // U4TX -> RF12

    // Baud 200 000 @ PBCLK 48 MHz
    U4MODEbits.BRGH = 0; // 16
    U4BRG = 14;         // 14=200 000

    U4MODEbits.PDSEL = 0b11; // 0b11 9 BITS parite
    U4MODEbits.STSEL = 1;        // 1

    // EN TX et RX
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;

    // clean des flag et activation du UART
    IFS2bits.U4RXIF = 0;
    IFS2bits.U4TXIF = 0;
    IEC2bits.U4RXIE = 1;
    IEC2bits.U4TXIE = 0;
    IPC9bits.U4IP   = 5;
    IPC9bits.U4IS   = 0;

    U4MODEbits.ON = 1;
}

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
        if (tx_index == 0)
        {
            U4TXREG = ajout_parite_odd(scindillerMSB(test_buffer[test_index]));
            tx_index = 1;
        }
        else
        {
            U4TXREG = ajout_parite_odd(scindillerLSB(test_buffer[test_index]));
            tx_index = 0;
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
        if (tx_index == 0)
        {
            U4TXREG = ajout_parite_odd(scindillerMSB(audioBuffer[ADC_index]));
            tx_index = 1;
        }
        else
        {
            U4TXREG = ajout_parite_odd(scindillerLSB(audioBuffer[ADC_index]));
            tx_index = 0;
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

