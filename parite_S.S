#include <xc.h>

.text
.globl ajout_parite_odd
.set noreorder
.ent ajout_parite_odd

ajout_parite_odd:

    addi    sp, sp, -4  # réserver 4 octets sur la pile
    sw      ra, 0(sp)   # sauvegarde de l'adresse(ra)

    move    t0, a0      # copie donnée a0(8bits) dans t0
    move    t1, zero    # accumulateur XOR mis a 0
    li      t2, 8       # compteur pour 8 bits

boucle_xor:
    andi    t3, t0, 1   # mask AND LSB T0
    xor     t1, t1, t3  # parité
    srl     t0, t0, 1   # on decale vers MSB
    addi    t2, t2, -1  # compteur -1
    bnez    t2, boucle_xor  # boucle sur t2 jusqua t2=0
    nop   # securité

    beq     t1, zero, set_bit  # t1 = 0 (pair) ou 1 (impair)
    nop

    move    v0, a0  # impair ? bit 9 = 0
    j       fin
    nop

set_bit:
    ori     v0, a0, 0x100   # pair ? bit 9 = 1

fin:
    lw      ra, 0(sp)  
    addi    sp, sp, 4  # on libere les 4 octets
    jr      ra         # retour dans .c
    nop                # delay slot

.end ajout_parite_odd
    
#-------------------------------------------------------------------------
    
.globl valider_parite_odd
.ent valider_parite_odd

valider_parite_odd:

    addi    sp, sp, -4
    sw      ra, 0(sp)

    move    t0, a0          # copie donnée 9 bits
    andi    t4, t0, 0x100   # extrait bit de parité
    srl     t4, t4, 8       # place parité en bit 0

    andi    t0, t0, 0xFF    # garde seulement 8 bits données

    move    t1, zero        # accumulateur XOR
    li      t2, 8           # compteur

boucle_val:
    andi    t3, t0, 1
    xor     t1, t1, t3
    srl     t0, t0, 1
    addi    t2, t2, -1
    bnez    t2, boucle_val
    nop

    # t1 = parité des 8 bits
    # t4 = bit de parité reçu

    xor     t5, t1, t4      # compare les deux

    move    v0, t5          # 0 = OK, 1 = erreur

    lw      ra, 0(sp)
    addi    sp, sp, 4
    jr      ra
    nop

.end valider_parite_odd
