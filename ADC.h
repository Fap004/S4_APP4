#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdbool.h>

#define FS        8000
#define DURATION  8
#define BUFFER_SIZE (FS * DURATION)

extern volatile uint16_t audioBuffer[BUFFER_SIZE];
extern volatile int ADC_index;
extern volatile bool threshold;

void ADC_Init(void);
void ADC_Stop(void);

#endif // ADC_H