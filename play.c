/*APP3 */
/* play.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 mode qui permet de jouer le son déja enregistrer

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :   

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>
*/
#include <xc.h>
#include "config.h"

#include "play.h"
#include "timers.h"
#include "ADC.h"
#include "led.h"


int play(void)
{
    static int started = 0; //variable permettant de connaitre si le circuit est actif ou inactif

    //configure les timers, le Oc et initi les valeurs à 0 pour recommencer a neuf
    if (!started)
    {
        //Timer1_config();
        //Timer2_config();
        //Timer3_config();
        //OC_config();
        
        ADC_index = 0;
        started = 1;
    }

    //remet à l'etat initial les timers ainsi que le OC pour être prêt a recevoir une nouvelle commande par la suite
    if (ADC_index >= BUFFER_SIZE)
    {
        OC1RS = 0;      //sortie sonore à 0
        started = 0;
        //OC_stop();
        //Timer2_stop();
        //Timer1_stop();
        return 1;       // terminé
    }
    
    return 0;           // en cours
}