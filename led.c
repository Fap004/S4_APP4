/*APP3 */
/* led.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Configuration des led et fonctions permettant d'allumer et d'etteindre les LEDs

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
#include "Led.h"

//Initialisation des LEDs
void led_init(){
    TRISACLR = 0x0001;   // RA0 en sortie
    LATACLR  = 0x0001;   // LED0 éteinte au départ
    
    TRISACLR = 0x0002;   // RA1 en sortie
    LATACLR  = 0x0002;   // LED1 éteinte au départ
    
    TRISACLR = 0x0004;   // RA2 en sortie
    LATACLR  = 0x0004;   // LED2 éteinte au départ
    
    TRISACLR = 0x0008;   // RA3 en sortie
    LATACLR  = 0x0008;   // LED3 éteinte au départ
    
    TRISACLR = 0x0010;   // RA4 en sortie
    LATACLR  = 0x0010;   // LED4 éteinte au départ
    
    TRISACLR = 0x0020;   // RA5 en sortie
    LATACLR  = 0x0020;   // LED5 éteinte au départ
    
    TRISACLR = 0x0040;   // RA6 en sortie
    LATACLR  = 0x0040;   // LED6 éteinte au départ
    
    TRISACLR = 0x0080;   // RA7 en sortie
    LATACLR  = 0x0080;   // LED7 éteinte au départ
}

//Activation de la Led selon l'appel de la fonction
void OnLed(unsigned int Led)
{
    switch (Led) {
        case 0: LATAbits.LATA0 = 1; break;
        case 1: LATAbits.LATA1 = 1; break;
        case 2: LATAbits.LATA2 = 1; break;
        case 3: LATAbits.LATA3 = 1; break;
        case 4: LATAbits.LATA4 = 1; break;
        case 5: LATAbits.LATA5 = 1; break;
        case 6: LATAbits.LATA6 = 1; break;
        case 7: LATAbits.LATA7 = 1; break;
        default: break;
    }
}

//désactivation de la Led selon l'appel de la fonction
void OffLed(unsigned int Led)
{
    switch (Led) {
        case 0: LATAbits.LATA0 = 0; break;
        case 1: LATAbits.LATA1 = 0; break;
        case 2: LATAbits.LATA2 = 0; break;
        case 3: LATAbits.LATA3 = 0; break;
        case 4: LATAbits.LATA4 = 0; break;
        case 5: LATAbits.LATA5 = 0; break;
        case 6: LATAbits.LATA6 = 0; break;
        case 7: LATAbits.LATA7 = 0; break;
        default: break;
    }
}