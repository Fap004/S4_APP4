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
    OnLed(7);
    Timer3_config();
    UART_Init();
    switch_init();
    Timer1_config;
    Timer2_config;
    Timer3_config;
    macro_enable_interrupts();
    
    //Initialisation pour l'analog discovery
    TRISBbits.TRISB6 = 0;
    ANSELBbits.ANSB6 = 0;
    RPB6Rbits.RPB6R = 0x0C;
}

void switch_init()
{
    TRISFbits.TRISF3 = 1;  // RF3 (SW0) configured as input
}
 