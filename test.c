/*APP3 */
/* test.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Mode qui fait jouer un sinus de 400Hz pendant 4 secondes

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :   

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>
*/
#include <xc.h>
#include <sys/attribs.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#include "test.h"
#include "timers.h"
#include "UART_Tx.h"
#include "UART_Rx.h"

//20 scan d'un sinus car 8kHz de freq_ech/signal 400hz =20 echantillons
volatile uint16_t test_buffer[BUFFER_SIZE_TEST]=
{
    511, 669, 812, 925, 997,
    1023, 997, 925, 812, 669,
    511, 353, 210, 97, 25,
    0, 25, 97, 210, 353
};
volatile uint16_t test_index = 0;   //permet de naviguer dans les échantillons
volatile uint16_t test_cpt=0;       //compteur pour le nombre de fois que le signal fais le tour de l'index

int test(void)
{
    static int started = 0; //variable permettant de connaitre si le circuit est actif ou inactif
    
    //configure les timers, le Oc et initi les valeurs à 0 pour recommencer a neuf
    if (!started)
    {
        //Timer2_config();
        //Timer3_config();
        //OC_config();
        
        started = 1;
        test_index = 0;
        test_cpt = 0;
    }
    
    //remet à l'etat initial les timers ainsi que le OC pour être prêt a recevoir une nouvelle commande par la suite
    if (test_cpt >= NB_PERIODE_TEST)
    {
        started = 0;
        //OC_stop();
        //Timer2_stop();
        //Timer3_stop();
        OC1RS = 0;      //sortie sonore à 0
        return 1;       // terminé
    }
    
    return 0;           // en cours
}

int test_tx(void)
{
    static int started = 0; //variable permettant de connaitre si le circuit est actif ou inactif
    
    //configure les timers, le Oc et initi les valeurs à 0 pour recommencer a neuf
    if (!started)
    {
        Timer2_config();
        Timer3_config();
        OC_config();
        
        started = 1;
        test_index = 0;
        test_cpt = 0;
    }
    
    //remet à l'etat initial les timers ainsi que le OC pour être prêt a recevoir une nouvelle commande par la suite
    if (test_cpt >= NB_PERIODE_TEST)
    {
        started = 0;
        //OC_stop();
        //Timer2_stop();
        //Timer3_stop();
        OC1RS = 0;      //sortie sonore à 0
        return 1;       // terminé
    }
    
    return 0;           // en cours
}