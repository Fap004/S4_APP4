/*APP3 */
/* OC.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Permet le controle du PWM

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :   

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>
*/

/* Microchip includes */
#include <xc.h>
#include <sys/attribs.h>
#include "config.h"

#include "OC.h"

//Gestion sortie DAC pour sortie audio
void OC_config(){
    //P.429
    OC1CONbits.OC32 = 0;    
    OC1CONbits.OCM = 0b110; // PWM mode
    OC1CONbits.OCTSEL = 0;  // Timer2
    OC1RS = 0;
    OC1R  = 0;
    OC1CONbits.ON = 1;
}

void OC_stop(){
    OC1CONbits.ON = 0;
}