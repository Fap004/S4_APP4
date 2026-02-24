// uart_rx.c
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

#include "UART_Rx.h"
#include "config.h"

extern  void VerifierParite_S(unsigned int data);
/* ---------- FIFO 8 bits ---------- */
//#define UART_RX_BUF_SIZE 128
volatile uint8_t uartRxBuf[UART_RX_BUF_SIZE];
volatile unsigned short uartRxWr = 0;
volatile unsigned short uartRxRd = 0;

/* ---------- FIFO 10 bits ---------- */
//#define UART_RX_BUF10_SIZE 128
volatile uint16_t uartRxBuf10[UART_RX_BUF10_SIZE];
volatile unsigned short uartRxWr10 = 0;
volatile unsigned short uartRxRd10 = 0;

/* ---------- Flags et variables internes ---------- */
static uint8_t rx_msb8    = 0;   // stocke le MSB du mot 10 bits
static uint8_t rx_wait_lsb = 0;  // 0 = attend MSB, 1 = attend LSB

/* ---------- Helpers FIFO ---------- */
static inline unsigned short nxt8(unsigned short x){ return (x + 1) % UART_RX_BUF_SIZE; }
static inline void push8(uint8_t v)
{
    unsigned short n = nxt8(uartRxWr);
    if (n == uartRxRd) uartRxRd = nxt8(uartRxRd); // overwrite oldest
    uartRxBuf[uartRxWr] = v;
    uartRxWr = n;
}

bool uart_rx_pop(uint8_t *v)
{
    if (uartRxRd == uartRxWr) return false; // buffer vide
    *v = uartRxBuf[uartRxRd];
    uartRxRd = nxt8(uartRxRd);
    return true;
}

/* ---------- FIFO 10 bits ---------- */
static inline unsigned short nxt10(unsigned short x){ return (x + 1) % UART_RX_BUF10_SIZE; }
static inline void push10(uint16_t v)
{
    unsigned short n = nxt10(uartRxWr10);
    if (n == uartRxRd10) uartRxRd10 = nxt10(uartRxRd10); // overwrite oldest
    uartRxBuf10[uartRxWr10] = v;
    uartRxWr10 = n;
}

bool uart_rx_pop10(uint16_t *v)
{
    if (uartRxRd10 == uartRxWr10) return false; // buffer vide
    *v = uartRxBuf10[uartRxRd10];
    uartRxRd10 = nxt10(uartRxRd10);
    return true;
}

/* ---------- ISR UART4 ---------- */
void __ISR(_UART_4_VECTOR, IPL5AUTO) U4RX_ISR(void)
{
    // 1) Débloquer si overrun
    if (U4STAbits.OERR) {
        U4STAbits.OERR = 0;
    }

    // 2) Variables statiques pour reconstituer les 10 bits
    static uint8_t rx_msb8    = 0;
    static uint8_t rx_wait_lsb = 0;

    // 3) Lire tout le FIFO matériel
    while (U4STAbits.URXDA)
    {
        // *** LIRE UNE SEULE FOIS ***
        uint16_t rx = U4RXREG;            // PDSEL=0b11 -> 9 bits utiles : [parité_logicielle | data8]

        // Vérification de parité logicielle via TA fonction (void, allume LED si erreur)
        // IMPORTANT : ne PAS relire U4RXREG ici !
        VerifierParite_S(rx);

        // Extraire l?octet utile
        uint8_t data8 = (uint8_t)(rx & 0xFF);

        if (!PORTBbits.RB9)
        {
            // ===== MODE 8 BITS =====
            push8(data8);
        }
        else
        {
            // ===== MODE 10 BITS =====
            if (!rx_wait_lsb) {
                // 1er mot = 8 MSB
                rx_msb8     = data8;
                rx_wait_lsb = 1;
            } else {
                // 2e mot = 2 LSB dans data8[1:0]
                uint8_t  lsb2     = (uint8_t)(data8 & 0x03);
                uint16_t sample10 = ((uint16_t)rx_msb8 << 2) | lsb2;
                push10(sample10);
                rx_wait_lsb = 0;
            }
        }
    }

    // 4) Clear flag d?interruption
    IFS2bits.U4RXIF = 0;
}
/*void __ISR(_UART_4_VECTOR, IPL5AUTO) U4RX_ISR(void)
{
    // Clear overrun si nécessaire
    if (U4STAbits.OERR)
    {
        U4STAbits.OERR = 0;
    }
    
    while (U4STAbits.URXDA)
    {
        VerifierParite_S(U4RXREG);
        uint8_t d8 = (uint8_t)(U4RXREG & 0xFF);

        if (!PORTBbits.RB9)
        {
            // ===== MODE 8 BITS SIMPLE =====
            push8(d8);
        }
        else
        {
            // ===== MODE 10 BITS =====
            if (!rx_wait_lsb)
            {
                rx_msb8 = d8;       // stocke les 8 MSB
                rx_wait_lsb = 1;    // attend le prochain octet (2 LSB)
            }
            else
            {
                uint8_t lsb2 = d8 & 0x03;
                uint16_t sample10 = ((uint16_t)rx_msb8 << 2) | lsb2;
                push10(sample10);   // ajoute dans le FIFO 10 bits
                rx_wait_lsb = 0;    // prêt pour le prochain mot
            }
        }
    }
    // Clear flag d'interruption
    IFS2bits.U4RXIF = 0;
}
*/
/*void __ISR(_UART_4_VECTOR, IPL5AUTO) U4RX_ISR(void)
{
    // 1) OERR : si overrun, la RX est bloquée tant qu'on ne met pas OERR=0
    if (U4STAbits.OERR) 
    {
        U4STAbits.OERR = 0;  // relance la réception
        // (facultatif) compter l'overrun ici 
    }

    // 2) Vider TOUT le FIFO matériel tant que URXDA = 1 (obligatoire) 
    while (U4STAbits.URXDA) 
    {
        if(!PORTBbits.RB9)
        {
            uint16_t rx = U4RXREG;             // mot 9 bits si PDSEL=0b11 
            uint8_t  d8 = (uint8_t)(rx & 0xFF);
            uint8_t  msb = (uint8_t)((rx >> 8) & 1u);

            push8(d8);
        }
        else
        {
            
        }
    }

    // 4) Clear du flag d'interruption (après avoir tout vidé)
    IFS2bits.U4RXIF = 0;
}*/