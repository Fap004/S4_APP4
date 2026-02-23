/*APP3 */
/* led.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Main faisant l'initiation et apres l'appel de la MEF et de la gestion du son

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
#include "config_bits.h"
#include "mef.h"
#include "sys_init.h"
#include "led.h"

volatile Etat_t Etat = ETAT_ATT;    //Etat Attente soit celle initial

int main(void)
{
    sys_init();             // Initiation du système
    Etat = ETAT_ATT;        // Assignation de l'état de base

    while (1)
    {
        mef();              //MEF controlant les modes
//    if (!PORTFbits.RF3)
//    {
//        OnLed(3); // intercom prioritaire
//        OffLed(5);
//    }
//    if (PORTFbits.RF3)
//    {
//        OnLed(5); // intercom prioritaire
//        OffLed(3);
//    }
    //        gestion_volume();   //Gestion du volume
    }
} 
//test luis