#include <xc.h>

.text
.globl ajout_parite_odd
.set noreorder
.ent ajout_parite_odd

ajout_parite_odd:

    move    t0, a0      // copie donnée a0(8bits) dans t0
    move    t1, zero    // accumulateur XOR mis a 0
    li      t2, 8       // compteur pour 8 bits

boucle_xor:
    andi    t3, t0, 1   // mask AND LSB T0
    xor     t1, t1, t3  // parité
    srl     t0, t0, 1   // on decale vers MSB
    addi    t2, t2, -1  // compteur -1
    bnez    t2, boucle_xor  // boucle sur t2 jusqua t2=0
    nop   // securité

    beq     t1, zero, set_bit  // t1 = 0 (pair) bit 9 = 1 (on va a set bit)
    nop

    move    v0, a0  // t1=1 (impair)  bit 9 = 0
    j       fin
    nop

set_bit:
    ori     v0, a0, 0x100   // pair ? bit 9 = 1

fin:
    
    jr      ra         // retour dans .c
    nop                // delay slot

.end ajout_parite_odd
    
#-------------------------------------------------------------------------
 .globl VerifierParite_S
/* Entrée : a0 = trame 9 bits reçue */

/* Action : Allume LD5 (RA5) si erreur, l'éteint au début */
 
.ent VerifierParite_S

VerifierParite_S:

    la      t4, LATACLR  // nouvelle adresse LATACLR

    li      t5, 0b00100000 // masque DEL LD5    

    sw      t5, 0(t4) // masque dans LATACLR -> DEL LD5 à 0
 
    move    t0, a0 // t0 = a0   

    li      t1, 0 // t1 = 1          

    li      t2, 9 // t2 = 9         
 
boucle_verif:

    andi    t3, t0, 1 // t3 (temporaire) = t0 et 1 -> isole bit de droite

    addu    t1, t1, t3 // t1 = t1 + t3 (unsigned)

    srl     t0, t0, 1 // t0 décalé de 1 vers la droite

    addiu   t2, t2, -1 // t2 = t2 - 1 -> décrémente le compteur (9 vers 0)

    bne     t2, zero, boucle_verif // si compteur != 0 -> recommence boucle 

    nop
 
    andi    t1, t1, 1 // si t1=0: somme paire. si t1=1: somme impaire

    bne     t1, zero, fin_verif // si t1 = 1 -> fin vérification

    nop
 
    la      t4, LATASET // nouvelle adresse LATASET

    li      t5, 0b00100000 // masque DEL LD5 

    sw      t5, 0(t4) // masque dans LATASET -> DEL LD5 à 1
 
fin_verif:

    jr      ra

    nop

.end VerifierParite_S
 