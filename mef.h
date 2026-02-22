#ifndef MEF_H
#define MEF_H

typedef enum {
    ETAT_ATT,
    ETAT_EN,       
    ETAT_LIRE,
    ETAT_TEST,
    ETAT_COM
} Etat_t;

extern volatile Etat_t Etat;

void mef(void);

#endif // MEF_H
