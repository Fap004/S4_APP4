/*APP3 */
/* sys_init.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 * Permet de regrouper les fonctions d'initialisation

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

#include "sys_init.h"
#include "led.h"
#include "button.h"
#include "speaker.h"
#include "UART_Tx.h"
#include "timers.h"

//Initialisation du système
void sys_init(void)
{
    led_init();
    boutons_init();
    speaker_init();
    micro_init();
    //ADC_Init();          // ADC avant timers si Timer déclenche lecture
    UART_Init();         // RX/TX prêt avant Timer3 ISR
    switch_init();
    OC_config();         // config PWM avant timers si Timer3 pilote OC
    Timer1_config();
    Timer2_config();
    Timer3_config();     // Timer3 ISR actif à la fin
    macro_enable_interrupts();
    
    OnLed(7);
    
    //Initialisation pour l'analog discovery
    TRISBbits.TRISB6 = 0;
    ANSELBbits.ANSB6 = 0;
    RPB6Rbits.RPB6R = 0x0C;
}

void switch_init()
{
    TRISFbits.TRISF3 = 1;  // RF3 (SW0) configured as input
    
    TRISBbits.TRISB9 = 1;	 // RB9 (SW7) configured as input
    ANSELBbits.ANSB9 = 0;	 // RB9 (SW7) disabled analog
}
 