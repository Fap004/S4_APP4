/*APP3 */
/* recording.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 mode qui permet de faire une enregistrement de 8 secondes

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :   

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>
*/
#include <xc.h>
#include "config.h"

#include "recording.h"
#include "ADC.h"
#include "led.h"

int enregistrement(void)
{
    static int started = 0; //variable permettant de connaitre si le circuit est actif ou inactif
    
    //initalise l'ADC et le timer3
    if (!started)
    {
        ADC_index = 0;
        //Timer3_config();
        //OnLed(0);
        ADC_Init();
        started = 1;
    }

    //remet à l'etat initial l'ADC, la led, la variable srated et le threshold
    if (ADC_index >= BUFFER_SIZE)
    {
        ADC_Stop();
        OffLed(0);
        started = 0;
        threshold=0;
        return 1;   // terminé
    }

    return 0;       // en cours
}