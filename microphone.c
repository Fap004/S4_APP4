/*APP3 */
/* microphone.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Fonction permetant l'initialisation du micro

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

/* Basys MX3 includes */
#include "config.h"
#include "microphone.h"

//Initialisation du microphone
void micro_init()
{
    TRISBbits.TRISB4 = 1; 
    ANSELBbits.ANSB4 = 1;
}

