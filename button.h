#ifndef _BUTTON_H
#define _BUTTON_H

typedef struct {
    unsigned char ancien;
    unsigned int t_last;
} bouton_t;

extern volatile bouton_t btnR;
extern volatile bouton_t btnL;
extern volatile bouton_t btnC;
extern volatile bouton_t btnU;
extern volatile bouton_t btnD;

extern volatile unsigned int tick_ms;

void boutons_init(void);
int bouton_appuye(unsigned char actuel,volatile bouton_t *b);


#endif /* _BUTTON_H */