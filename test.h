#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stdbool.h>

#define NB_PERIODE_TEST   (4 * 400)      // nombre de sinus pour 4 secondes
#define BUFFER_SIZE_TEST  (8000 / 400)   // frequence echantillonnage / frequence signal

extern volatile uint16_t test_buffer[BUFFER_SIZE_TEST];
extern volatile uint16_t test_index;
extern volatile uint16_t test_cpt;

int test(void);

#endif // TEST_H