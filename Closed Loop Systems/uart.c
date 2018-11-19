#include <msp430.h>
#include "uart.h"

void serial_init() {
    P4SEL |= BIT4+BIT5;                       // P3.3,4 = USCI_A0 TXD/RXD
    UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA1CTL1 |= UCSSEL_2;                     // SMCLK
    UCA1BR0 = 9;                              // 1MHz 115200 (see User's Guide)
    UCA1BR1 = 0;                              // 1MHz 115200
    UCA1MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
    UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA1IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void write_serial(char* buffer) {
    for (int i = 0; i < 8; i++) {
        while (!(UCA1IFG && UCTXIFG));
        UCA1TXBUF = buffer[i];
    }
}

void write_dec(int number) {
    char buffer[9] = "00000000";

    unsigned int to_convert = 0;

    if (number >= 0) {
        buffer[0] = ' ';
        to_convert = (unsigned int) number;
    } else {
        buffer[0] = '-';
        to_convert = (unsigned int) -number;
    }

    int i = 6;
    while (to_convert != 0 && i >=0) {
        buffer[i] = to_convert % 10 + '0';
        to_convert /= 10;
        i--;
        for (int i = 0; i < 10000; i++) {
            asm("");
        }
    }

    buffer[7] = '\n';
    write_serial(buffer);
}

char rx_buffer[RX_BUFFER_SIZE+1];
char rx_buffer_size = 0;

unsigned int serial_available() {
    return rx_buffer_size;
}

char serial_read() {

    if (rx_buffer_size == 0) return 255;

    char first_char = rx_buffer[0];

    for (int i = 1; i < rx_buffer_size; i++) {
        rx_buffer[i-1] = rx_buffer[i];
    }

    rx_buffer[rx_buffer_size-1] = 255;

    --rx_buffer_size;

    return first_char;
}

char serial_peek() {
    return rx_buffer[0];
}

void __attribute__((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void) {

    char in = UCA1RXBUF;

    rx_buffer[rx_buffer_size] = in;
    rx_buffer_size++;

    //while (!(UCA1IFG && UCTXIFG));
    //UCA1TXBUF = in;
}
