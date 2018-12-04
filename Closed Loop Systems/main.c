#include <msp430.h>

#include "uart.h"
#include "temp.h"
#include "pwm.h"

void setup_watchdog() {
    // Stop WDT
    WDTCTL = WDTPW + WDTHOLD;
}

int report = 0;

int main(void) {

    setup_watchdog();

    temperature_init();
    serial_init();
    pwm_init();

    pwm_p_set(1000);
    pwm_i_set(1);

    __bis_SR_register(GIE);

    while (1) {

        if (report) {
            double temp = temperature_read();
            int output = pwm_output();
            write_serial("report: ");
            write_dec((int)temp);
            write_serial("        ");
            write_dec(output);
            write_serial("       \n");
        }

        if (serial_available() > 0) {
            char cmd = serial_peek();
            switch (cmd) {
                // Set temperature for auto pwm
                case 'A':

                    // Need 3 characters to set temp
                    // Axx

                    // Remover letter
                    serial_read();

                    int temp = 0;
                    for(int i = 0; i < 2; i++) {
                        char in = serial_read();
                        switch(i) {
                            case 0:
                                temp = (in - '0') * 10;
                                break;
                            case 1:
                                temp += (in - '0') * 1;
                                break;
                        }
                    }

                    pwm_auto_set(temp);
                    write_serial("message:");
                    write_serial("Set temp");
                    write_dec(temp);
                    write_serial("       \n");

                    break;

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
                    write_serial("       \n");

                    break;

                // Enable/disable reporting
                case 'R':

                    // Need at least 2 characters
                    // Rx
                    if (serial_available() < 2) break;

                    // Remove letter from buffer
                    serial_read();

                    char in = serial_read();
                    if (in == '0') {
                        report = 0;
                        write_serial("message:");
                        write_serial("Disabled");
                        write_serial(" report\n");
                    } else if (in == '1') {
                        report = 1;
                        write_serial("message:");
                        write_serial("Enabled ");
                        write_serial("report  \n");
                    }

                    break;

                case 'P':

                    // Need at least 5 characters to set pwm
                    // Pxxxx
                    if (serial_available() < 5) break;

                    // Remove the letter from the buffer, since we know it is an S
                    // and need to get to the numbers
                    serial_read();

                    // Convert the ascii numbers to a real int
                    int p = 0;
                    for (int i = 0; i < 4; i++) {
                        char in = serial_read();
                        switch(i) {
                            case 0:
                                p = (in - '0') * 1000;
                                break;
                            case 1:
                                p += (in - '0') * 100;
                                break;
                            case 2:
                                p += (in - '0') * 10;
                                break;
                            case 3:
                                p += (in - '0') * 1;
                                break;
                        }
                    }

                    pwm_p_set(p);

                    write_serial("message:");
                    write_serial("Set p   ");
                    write_dec(p);
                    write_serial("       \n");

                    break;

                case 'I':

                    // Need at least 5 characters to set pwm
                    // Pxxxx
                    if (serial_available() < 5) break;

                    // Remove the letter from the buffer, since we know it is an S
                    // and need to get to the numbers
                    serial_read();

                    // Convert the ascii numbers to a real int
                    int ki = 0;
                    for (int i = 0; i < 4; i++) {
                        char in = serial_read();
                        switch(i) {
                            case 0:
                                ki = (in - '0') * 1000;
                                break;
                            case 1:
                                ki += (in - '0') * 100;
                                break;
                            case 2:
                                ki += (in - '0') * 10;
                                break;
                            case 3:
                                ki += (in - '0') * 1;
                                break;
                        }
                    }

                    pwm_i_set(ki);

                    write_serial("message:");
                    write_serial("Set i   ");
                    write_dec(ki);
                    write_serial("       \n");

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

