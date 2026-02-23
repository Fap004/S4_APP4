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
#include "UART_Rx.h"
#include "UART_Tx.h"
#include "timers.h"
#include <sys/attribs.h>

//Machine à état fini permettant la gestion des modes
void mef()
{
    // ===============================
    // PRIORITÉ INTERCOM
    // ===============================
    if (PORTFbits.RF3)
    {
        Etat = ETAT_INTERCOM; // intercom prioritaire
    }

    switch (Etat)
    {
        case ETAT_INTERCOM:
            if (intercom())
            {
                Etat = ETAT_ATT;
            }
            OnLed(2);
        break;
        case ETAT_ATT:
            T1CONbits.ON = 0;
            OffLed(0);
            OffLed(1);
            OffLed(2);
            OffLed(3);
            OffLed(4);
            OffLed(5);
            //OnLed(6);

            // Lecture boutons seulement en attente
            if (bouton_appuye(PORTBbits.RB8, &btnR)) 
            {
                Etat = ETAT_EN;
            } 
            else if (bouton_appuye(PORTBbits.RB0, &btnL)) 
            {
                Etat = ETAT_LIRE;
            } 
            else if (bouton_appuye(PORTFbits.RF0, &btnC)) 
            {
                Etat = ETAT_TEST;
            } 
            else if (bouton_appuye(PORTBbits.RB1, &btnU)) 
            {
                Etat = ETAT_EN_TX;
            } 
            else if (bouton_appuye(PORTAbits.RA15, &btnD)) 
            {
                Etat = ETAT_TEST_TX;
                test_tx_index = 0;  // préparer le buffer pour le Timer ISR
                OnLed(4);
            }
            break;

        case ETAT_EN:
            if (enregistrement())
                Etat = ETAT_ATT;
            break;

        case ETAT_LIRE:
            if (play())
                Etat = ETAT_ATT;
            break;

        case ETAT_TEST:
            if (test())
                Etat = ETAT_ATT;
            break;

        case ETAT_EN_TX:
            if (enr_tx())
                Etat = ETAT_ATT;
            break;

        case ETAT_TEST_TX:
            if (test_tx())
                Etat = ETAT_ATT;
            break;
    }
}


