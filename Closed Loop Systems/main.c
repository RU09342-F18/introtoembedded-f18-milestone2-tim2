#include <msp430.h>

#include "uart.h"
#include "temp.h"
#include "pwm.h"

void setup_watchdog() {
    // Stop WDT
    WDTCTL = WDTPW + WDTHOLD;
}

int temperature_report = 0;

int main(void) {

    setup_watchdog();

    temperature_init();
    serial_init();
    pwm_init();

    __bis_SR_register(GIE);

    while (1) {

        if (temperature_report) {
            double temp = temperature_read();
            write_dec((int)temp);
        }

        if (serial_available() > 0) {
            char cmd = serial_peek();
            switch (cmd) {

                // Set pwm directly
                case 'S':

                    // Need at least 5 characters to set pwm
                    // Sxxxx
                    if (serial_available() < 5) break;

                    // Remove the letter from the buffer, since we know it is an S
                    // and need to get to the numbers
                    serial_read();

                    // Convert the ascii numbers to a real int
                    int pwm = 0;
                    for (int i = 0; i < 4; i++) {
                        char in = serial_read();
                        switch(i) {
                            case 0:
                                pwm = (in - '0') * 1000;
                                break;
                            case 1:
                                pwm += (in - '0') * 100;
                                break;
                            case 2:
                                pwm += (in - '0') * 10;
                                break;
                            case 3:
                                pwm += (in - '0') * 1;
                                break;
                        }
                    }

                    pwm_manual_set(pwm);

                    write_serial("message:");
                    write_serial("Set pwm ");
                    write_dec(pwm);

                    break;

                // Enable/disable temp reporting
                case 'T':

                    // Need at least 2 characters
                    // Tx
                    if (serial_available() < 2) break;

                    // Remove letter from buffer
                    serial_read();

                    char in = serial_read();
                    if (in == '0') {
                        temperature_report = 0;
                        write_serial("message:");
                        write_serial("Disabled");
                        write_serial(" tempera");
                        write_serial("ture rep");
                        write_serial("orting \n");
                    } else if (in == '1') {
                        temperature_report = 1;
                        write_serial("message:");
                        write_serial("Enabled ");
                        write_serial("temperat");
                        write_serial("ure repo");
                        write_serial("rting  \n");
                    }

                    break;

                default:
                    serial_read();
                    write_serial("message:");
                    write_serial("Bad cmd ");
                    char outbuf[8] = "       \n";
                    outbuf[0] = cmd;
                    write_serial(outbuf);

                    break;
            }
        }
    }
}

