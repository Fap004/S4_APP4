/*APP3 */
/* mef.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 MEF permettant de faire la gestion des états selon les modes 

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :   

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>
*/

#include <xc.h> // Nécessaire pour utiliser les définition de Microchip comme TRISA, ...

#include "mef.h"
#include "config.h"
#include "recording.h"
#include "test.h"
#include "play.h"
#include "button.h"

//Machine à état fini permettant la gestion des modes
void mef() 
{
    switch (Etat)
    {
        // ==========================
        case ETAT_ATT:          //Etat attente
            T1CONbits.ON = 0;   //stop clignotement
            OffLed(1);          //LED1 OFF
            OffLed(0);          //sécurité

            // lecture boutons SEULEMENT ici
             if (bouton_appuye(PORTBbits.RB8, &btnR))
                Etat = ETAT_EN;
            else if (bouton_appuye(PORTBbits.RB0, &btnL))
                Etat = ETAT_LIRE;
            else if (bouton_appuye(PORTFbits.RF0, &btnC))
                Etat = ETAT_TEST;
            break;

        // ==========================
            case ETAT_EN:               //Active l'état enregistrement
                if (enregistrement())
                {
                    Etat = ETAT_ATT;    //Etat attente
                }
                break;
                
        // ==========================
        case ETAT_LIRE:
            if (play())             //Active l'état Lire
            {
                Etat = ETAT_ATT;    //Etat attente
            }
            break;

        // ==========================
        case ETAT_TEST:             //Active l'état produisant un signal de 400Hz
            if (test())
            {
                Etat = ETAT_ATT;    //Etat attente
            }
            break;
        }
}


