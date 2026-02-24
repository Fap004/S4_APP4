// uart_rx.c
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/attribs.h>

#include "UART_Rx.h"
#include "config.h"

extern  void VerifierParite_S(unsigned int data);

//FIFO 8 bits
volatile uint8_t uartRxBuf[UART_RX_BUF_SIZE];
volatile unsigned short uartRxWr = 0;//Rx write
volatile unsigned short uartRxRd = 0;//RX read

//FIFO 10 bits
volatile uint16_t uartRxBuf10[UART_RX_BUF10_SIZE];
volatile unsigned short uartRxWr10 = 0;//RX write
volatile unsigned short uartRxRd10 = 0;//Rx read

//Helpers FIFO (facilite lutilisation)
static inline unsigned short nxt8(unsigned short x)
{
    return (x + 1) % UART_RX_BUF_SIZE; 
}

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

 //FIFO 10 bits
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

//ISR UART
void __ISR(_UART_4_VECTOR, IPL5AUTO) U4RX_ISR(void)
{
    static uint8_t rx_msb8    = 0;//variable MSB
    static uint8_t rx_wait_lsb = 0;//variable LSB

    //Lire tout le FIFO matériel
    while (U4STAbits.URXDA)
    {
        uint16_t rx = U4RXREG;//stock le message 8 bits messages et 1 parité

        VerifierParite_S(rx);//allume une del si un erreur detecter

        uint8_t data8 = (uint8_t)(rx & 0xFF);//enlever le bit de parité pour le reste

        if (!PORTBbits.RB9)
        {
            //MODE 8 BITS
            push8(data8);
        }
        else
        {
            //MODE 10 BITS
            if (!rx_wait_lsb)
            {
                rx_msb8     = data8;
                rx_wait_lsb = 1;
            } else
            {
                // 2messages 2 LSB
                uint8_t  lsb2     = (uint8_t)(data8 & 0x03);
                uint16_t sample10 = ((uint16_t)rx_msb8 << 2) | lsb2;
                push10(sample10);
                rx_wait_lsb = 0;
            }
        }
    }
    //Clear flag
    IFS2bits.U4RXIF = 0;
}