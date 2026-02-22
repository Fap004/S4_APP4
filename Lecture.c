/* Microchip includes */
#include <xc.h>
#include <sys/attribs.h>

/* Basys MX3 includes */
#include "config.h"
#include "Lecture.h"
#include "led.h"
#include "timers.h"

/*void __ISR(_TIMER_1_VECTOR, IPL2AUTO) Timer1ISR(void) 
{  
// Code de l'ISR ici

  LATAbits.LATA1 ^= 1;  
  IFS0bits.T1IF = 0; // clear interrupt flag
}

void stopTimer1(void) {
    T1CONbits.ON = 0;   // stop timer
    IFS0bits.T1IF = 0;  // clear flag
}


void dequoi(void){
    timer1();
    
    macro_enable_interrupts();


}/*