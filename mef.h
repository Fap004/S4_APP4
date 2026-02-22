#ifndef _MEF_H
#define _MEF_H
 
typedef enum {
    ETAT_ATT,
    ETAT_EN,       
    ETAT_LIRE,
    ETAT_TEST,
    ETAT_COM,
    ETAT_LIRE_TX,
    ETAT_TEST_TX,
    ETAT_INTERCOM
} Etat_t;
 
extern volatile Etat_t Etat;
 
void mef(void);
 
#endif // _MEF_H