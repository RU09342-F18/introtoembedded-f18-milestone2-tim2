
#ifndef UART_H
#define UART_H

#define RX_BUFFER_SIZE 8

void write_serial(char* buffer);
void write_dec(int number);

unsigned int serial_available();
char serial_read();
char serial_peek();

#endif
