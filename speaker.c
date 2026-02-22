/*APP3 */
/* speaker.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Initialisation du speaker

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
#include "speaker.h"

//Initialisation du speaker
void speaker_init(){
    TRISBbits.TRISB14 = 0;
    ANSELBbits.ANSB14 = 0;
    RPB14R = 0x0C;
}


