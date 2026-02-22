/*
#include <xc.h>
#include "uart.h"
#include "config.h"

void uartInit(){
    U4MODEbits.ON = 0;
    
    TRISFbits.TRISF12 = 0;
    TRISFbits.TRISF13 = 1;
    
    U4RXRbits.U4RXR   = 0b0100;   
    RPF12Rbits.RPF12R = 0b0010; 
    
    U4MODEbits.BRGH = 0; 
    
    U4MODEbits.PDSEL = 0b10; 
    U4MODEbits.STSEL = 1;
    
    U4BRG = 155;
    
    U4STAbits.UTXEN = 1;
    U4STAbits.URXEN = 1;
    
    U4MODEbits.ON = 1;
}


void UART4_PutChar(unsigned char c){
    while(U4STAbits.UTXBF){  
    U4TXREG = c;
    }
}

void UART4_PutChar(unsigned short v9) {
    // v9 = 0..0x1FF si PDSEL=0b11 (9 bits)
    while (U4STAbits.UTXBF) { // attendre qu'il y ait de la place  }
    U4TXREG = v9;    // en mode 9 bits, le bit 8 est transmis aussi
}
*/