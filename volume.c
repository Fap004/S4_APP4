/*APP3 */
/* volume.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 * Permet de faire la gestion du volume

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

#include "led.h"
#include "button.h"

volatile uint8_t volume=4;

void volume_init()
{
    volume=4;
    OnLed(6);
}

void gestion_volume()
{
    // Bouton UP (RB1)
    //Gestion pour monter le son
    if (bouton_appuye(PORTBbits.RB1, &btnU)) 
    {
        if (volume < 4) {
            volume++;
        }
    }

    // Bouton DOWN (RA15)
    //Gestion pour baisser le son
    if (bouton_appuye(PORTAbits.RA15, &btnD)) 
    {
        if (volume > 0) {
            volume--;
        }
    }

    // LEDs selon volume
    switch (volume) 
    {
        case 0:
            OnLed(2); OffLed(3); OffLed(4); OffLed(5); OffLed(6);   //0%
            break;
        case 1:
            OnLed(3); OffLed(2); OffLed(4); OffLed(5); OffLed(6);   //25%
            break;
        case 2:
            OnLed(4); OffLed(2); OffLed(3); OffLed(5); OffLed(6);   //50%
            break;
        case 3:
            OnLed(5); OffLed(2); OffLed(3); OffLed(4); OffLed(6);   //75%
            break;
        case 4:
            OnLed(6); OffLed(2); OffLed(3); OffLed(4); OffLed(5);   //100%
            break;
        default:
            break;
    }
}