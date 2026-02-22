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

//Machine à état fini permettant la gestion des modes
void mef() 
{
    // ===============================
    // PRIORITÉ INTERCOM
    // ===============================
    if (PORTFbits.RF3)
    {
        Etat = ETAT_INTERCOM;
    }
    switch (Etat)
    {
        case ETAT_ATT:
            T1CONbits.ON = 0;
            OffLed(1);
            OffLed(0);
            // Lecture boutons seulement en attente
            if (bouton_appuye(PORTBbits.RB8, &btnR))
                Etat = ETAT_EN;
            else if (bouton_appuye(PORTBbits.RB0, &btnL))
                Etat = ETAT_LIRE;
            else if (bouton_appuye(PORTFbits.RF0, &btnC))
                Etat = ETAT_TEST;
            else if (bouton_appuye(PORTBbits.RB1, &btnU))
                Etat = ETAT_LIRE_TX;
            else if (bouton_appuye(PORTAbits.RA15, &btnD))
                Etat = ETAT_TEST_TX;
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
        case ETAT_LIRE_TX:
            if (test())
                Etat = ETAT_ATT;
            break;
        case ETAT_TEST_TX:
            if (test())
                Etat = ETAT_ATT;
            break;
        case ETAT_INTERCOM:
            if (!PORTFbits.RF3)   // Sort seulement si switch OFF
                Etat = ETAT_ATT;
            else
                test();  // ou fonction intercom réelle
            break;
    }
}


