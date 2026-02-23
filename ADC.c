/*APP3 */
/* ADC.c */
/*
  Créateur :   paif1582 et RODL6305
  Date :      7 février 2026
  Revision :  1.0

  DESCRIPTION :
 configuration ADC et enregistrement de l'entrée audio dans la RAM 

  ENTRÉES :
       
  ENTRÉES/SORTIES :
  
  SORTIES :

  RETOUR :
    <Fournir le nom de la variable retournée par la fonction avec une brève
     description d'elle-même.>
*/

#include "ADC.h"
#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
#include "timers.h"
#include "mef.h"

volatile uint16_t audioBuffer[BUFFER_SIZE]; //stockage de la trame enregistrer
volatile int ADC_index = 0;                 //Index permettant de naviguer dans le buffer
volatile bool threshold = 0;                //Permet de connaitre si le seuil a été atteint
int compteur=0;                             //Permet de rajouter un delais pour éviter que le bruit causé par le boutton lors de l'Activation de la fonction depasse le seuil minimum

//ADC initialisation
void ADC_Init()
{   
    AD1CON1 = 0;
    AD1CON2 = 0;
    AD1CON3 = 0;
            
    AD1CON1bits.FORM = 0b000;   //FORMAT OUTPUT
    AD1CON1bits.SSRC = 0b010;   //Conversion Trigger Source Select bits
    AD1CON1bits.ASAM = 0b1;     //ADC Sample Auto-Start bit
    
    AD1CON2bits.VCFG = 0b000;
    AD1CON2bits.SMPI = 0b000;   //sample/Convert Sequences Per Interrupt Selection bits
    AD1CON2bits.BUFM = 0b000;   //ADC Result Buffer Mode Select bit
    AD1CON2bits.ALTS = 0;       //Always use MUX A input multiplexer settings
    
    AD1CON3bits.ADRC = 0b0;     //ADC Conversion Clock Select bits
    AD1CON3bits.SAMC = 15;      //Auto-sample Time bits
    AD1CON3bits.ADCS = 2;       //ADC Conversion Clock Select bits
    
    AD1CHSbits.CH0NA = 0;       //Negative Input Select bit for MUX B
    AD1CHSbits.CH0SA = 4;
    
    IFS0bits.AD1IF = 0;
    IPC5bits.AD1IP = 7;
    IPC5bits.AD1IS = 0;
    IEC0bits.AD1IE = 1;
   
    AD1CON1bits.ON = 1;       //ACTIVATION
 }

//Fonction sauvegardant le bruit du microphone apres avoir atteint le seuil et le delai
void __ISR(_ADC_VECTOR, IPL7AUTO) ADC_ISR(void)
{
    if (Etat == ETAT_INTERCOM)
    {
        UART4_SendIntercom();
        OnLed(5);
    }
    else
    {
        OffLed(5);
        if (ADC_index < BUFFER_SIZE)
        {
            audioBuffer[ADC_index++] = ADC1BUF0;
            threshold = 1;
            OnLed(0);
        }
        else
        {
            compteur++;
        }

        if (ADC_index >= BUFFER_SIZE)
        {
            ADC_Stop();
        }
    }

    IFS0bits.AD1IF = 0;
}

//Arret de l'ADC
void ADC_Stop(){
    AD1CON1bits.ON = 0;
    IEC0bits.AD1IE = 0;
    IFS0bits.AD1IF = 0;
    compteur=0;
}