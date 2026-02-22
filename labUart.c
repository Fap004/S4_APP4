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

void UART4_PutString(const char *str)
{
    while(*str != '\0')
    {
        UART4_PutChar(*str);
        str++;
    }
}