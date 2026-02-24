#include <xc.h>

.text
.globl  adc10_a_8
.set    noreorder
.ent    adc10_a_8

adc10_a_8:
    srl     v0, a0, 2       # v0 = x >> 2
    jr      ra
    nop

.end adc10_a_8
    
#------------------------------------  
.globl scindillerMSB
.ent scindillerMSB
    
scindillerMSB:
    srl     v0, a0, 2  # paquet 1
    andi    v1, a0, 0x0003 # paquet 2 
    
    jr      ra
    nop

.end scindillerMSB
#------------------------------------    
.globl scindillerLSB
.ent scindillerLSB

scindillerLSB:
    andi    v1, a0, 0x0003 # paquet 2
    jr      ra
    nop

.end scindillerLSB