/*APP3 */
/* button.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Configuration des boutons et fonction empechant le boucing

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
#include "button.h"

volatile bouton_t btnR= {0, 0};      //Bouton droit
volatile bouton_t btnL= {0, 0};      //Bouton gauche
volatile bouton_t btnC= {0, 0};      //Bouton centre
volatile bouton_t btnU= {0, 0};      //Bouton haut
volatile bouton_t btnD= {0, 0};      //Bouton bas

volatile unsigned int tick_ms;

//Initialisation des boutons
void boutons_init(){
    TRISBbits.TRISB1 = 1;   // entrée Btnu
    ANSELBbits.ANSB1 = 0;   // digitalBtnu

    TRISAbits.TRISA15 = 1;  // entrée Btnd
    
    TRISBbits.TRISB8 = 1;   // entrée Btnr
    ANSELBbits.ANSB8 = 0;   // digital Btnr
    
    TRISBbits.TRISB0 = 1;   // entrée Btnl
    ANSELBbits.ANSB0 = 0;   // digital Btnl
    
    TRISFbits.TRISF0 = 1; // entrée Btnc 
}

//Fonction permettant d'éviter le bouncing
int bouton_appuye(unsigned char actuel,volatile bouton_t *b)
{
    //si le bouton est a on, le dernier etat non et que ca fait plus de 0,125 sec
    if (actuel && !b->ancien && (tick_ms - b->t_last) > 1000)
    {
        b->ancien = 1;          //l'etat ancien devient a 1
        b->t_last = tick_ms;    //sauvegarde le temps au dernier changement d'etat positif
        return 1;
    }

    //si le bouton est a off l'etat ancien devient 0
    if (!actuel)
        b->ancien = 0;

    return 0;
}