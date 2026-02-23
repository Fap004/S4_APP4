/*APP3 */
/* timers.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 Fait la gestion des timers et en partie la lecture en choisisant quel valeur faire jouer

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :   

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>

/* Microchip includes */
#include <xc.h>
#include <sys/attribs.h>
#include "config.h"

#include "timers.h"
#include "ADC.h"
#include "mef.h"
#include "test.h"
#include "button.h"
#include "UART_Rx.h"
#include "UART_Tx.h"
#include "led.h"


extern volatile uint8_t g_rxActive;
extern volatile uint8_t g_txActive;

// variables TX sinus 4 s définies dans test.c (extern)
extern volatile unsigned test_tx_index;
extern volatile unsigned test_tx_periods;
extern volatile bool     test_tx_odd;
extern volatile uint16_t test_buffer[BUFFER_SIZE_TEST];

 
void __ISR(_TIMER_1_VECTOR, IPL2AUTO) Timer1ISR(void) 
{  
// Code de l'ISR ici
    
  LATAbits.LATA1 ^= 1;  
  IFS0bits.T1IF = 0; // clear interrupt flag
}

void Timer1_config()
{
   // Initialisation Timer 1
    T1CONbits.TCKPS = 0b11;     //Prescaler 256
    PR1 = 37499;                //Calcul 48Mhz/(256*(37 499+1))= 5.35Hz
    TMR1 = 0;
    T1CONbits.TCS = 0;
    T1CONbits.TGATE = 0;
    
    T1CONbits.ON = 1; 
   //initialisation des interuptions
    IPC1bits.T1IP = 2;
    IPC1bits.T1IS = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
}

void Timer1_stop()
{
    T1CONbits.ON = 0;
    IEC0bits.T1IE = 0;
    IFS0bits.T1IF = 0;
}

void Timer2_config()
{
    // Initialisation du timer 3  P.14-9 guide de r?f?rence
    T2CONbits.SIDL = 0;         // On utilise pas le "Iddle mode"
    T2CONbits.TCS = 0;          // Pas de clk externe
    T2CONbits.TGATE = 0;        // pas de Gate
    T2CONbits.TCKPS = 0b000;    // Prescaler de 1
    
    PR2 = 1022;                 //Calcul 48MHz/(1*(1022+1))=46 920Hz
    TMR2 = 0;                   //initialsation
    T2CONbits.ON = 1;           // On met le timer ? ON
}


void Timer2_stop()
{
    T2CONbits.ON = 0;
    IEC0bits.T2IE = 0;
    IFS0bits.T2IF = 0;
}

void Timer3_config()
{
    // Initialisation du timer 3  P.14-9 guide de r?f?rence
    
    T3CONbits.SIDL = 0;         // On utilise pas le "Iddle mode"
    T3CONbits.TCS = 0;          // Pas de clk externe
    T3CONbits.TGATE = 0;        // pas de Gate
    T3CONbits.TCKPS = 0b100;    // Prescaler de 16
    
    PR3 = 374;                  //Calcul 48MHz/(16*(374+1))=8KHz
    TMR3 = 0;                   //initialsation
    
    IPC3bits.T3IP = 6;
    IEC0bits.T3IE = 1;
    IFS0bits.T3IF = 0;
    T3CONbits.ON = 1;           // On met le timer à ON
}


void __ISR(_TIMER_3_VECTOR, IPL6AUTO) Timer3_ISR(void)
{
                    
        if (Etat == ETAT_TEST_TX)
        {
            if (test_index<BUFFER_SIZE_TEST)
            {
                UART4_SendSample();
                test_index++;
            }
            else
            {
                test_index=0;
                test_cpt++;
            }
        }
    uint8_t byte;
    if (uart_rx_pop(&byte))
        {
            uint16_t sample10 = ((uint16_t)byte) << 2;
            OC1RS = sample10;
        }
        else
        {
            /* ========================
               3?? LOGIQUE NORMALE pour lecture ADC ou TEST
               ======================== */
            switch (Etat)
            {
                case ETAT_LIRE:
                    if (ADC_index < BUFFER_SIZE) {
                        OC1RS = (uint16_t)(audioBuffer[ADC_index++]);
                    } else {
                        OC1RS = 0;
                    }
                    break;

                case ETAT_TEST:
                    if (test_index < BUFFER_SIZE_TEST) {
                        OC1RS = (uint16_t)(test_buffer[test_index++]);
                    } 
                    else
                    {
                        test_index = 0;   // boucle sur un cycle
                        test_cpt++;       // compteur total
                        OC1RS = 0;
                    }
                    break;

                default:
                    OC1RS = 0;
                    break;
            }
        }

    tick_ms++;             // debouncing ou gestion du temps
    IFS0bits.T3IF = 0;     // clear du flag Timer3

}

void Timer3_stop(){
    T3CONbits.ON = 0;
    IEC0bits.T3IE = 0;
    IFS0bits.T3IF = 0;
}