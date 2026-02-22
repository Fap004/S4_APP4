/*APP3 */
/* UART.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Configuration des boutons et fonction empechant le boucing

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :   

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>
*/

#include <xc.h>
#include <sys/attribs.h>
#include "config.h"

#include "UART.h"

void UART_Init()
{
    U4MODEbits.ON = 0;    
 
    TRISFbits.TRISF12 = 0;      // RF12 = TX (output)
    TRISFbits.TRISF13 = 1;      // RF13 = RX (input)
 
    U4RXRbits.U4RXR   = 0b0100; // RF13 -> U4RX
    RPF12Rbits.RPF12R = 0b0010; // U4TX -> RF12
 
    U4MODEbits.BRGH = 0;        // Mode standard (16x)
 
    U4BRG = 21;
 
    U4MODEbits.PDSEL = 0b11;    // 9 bits, no parity
    U4MODEbits.STSEL = 1;       // 2 stop bits
 
    // Activer TX et RX
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;
    // Nettoyer flags
    IFS2bits.U4RXIF = 0;
    IFS2bits.U4TXIF = 0;
 
    // Activer interruptions
    IEC2bits.U4RXIE = 1;      // RX interrupt ON
    IEC2bits.U4TXIE = 0;      // TX interrupt OFF (on l?activera si besoin)
 
    // Priorité
    IPC9bits.U4IP = 5;
    IPC9bits.U4IS = 0;
 
    U4MODEbits.ON = 1;          // Activer UART
}
//test fap