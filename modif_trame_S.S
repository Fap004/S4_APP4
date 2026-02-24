#include <xc.h>

.text
.set    noreorder    
#------------------------------------  
.globl scindillerMSB
.ent scindillerMSB
    
scindillerMSB:
    srl     v0, a0, 2  # paquet 1
    
    jr      ra
    nop

.end scindillerMSB
#------------------------------------    
.globl scindillerLSB
.ent scindillerLSB

scindillerLSB:
    andi    v0, a0, 0x0003 # paquet 2
    
    jr      ra
    nop

.end scindillerLSB