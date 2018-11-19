#include <msp430.h>

#include "uart.h"
#include "temp.h"

void set_pwm(int value) {
    if (value == 0) {
        TA0CCTL1 &= ~OUT;
        TA0CCTL1 &= ~OUTMOD_7;
        TA0CCTL1 |= OUTMOD_0;
    } else if (value >= 10000) {
        TA0CCTL1 |= OUT;
        TA0CCTL1 &= ~OUTMOD_7;
        TA0CCTL1 |= OUTMOD_0;
    } else {
        TA0CCTL1 |= OUTMOD_7;
        TA0CCR1 = value;
    }
}

void setup_watchdog() {
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
}

void setup_pwm() {
    // sets up timer 0 for pwm

    // Set P1.2 to output
    P1DIR |= BIT2;

    // Set P1.2 for peripheral function
    P1SEL |= BIT2;

    // Select clock source to SMCLK
    TA0CTL |= TASSEL_2;

    // Set to up mode
    TA0CTL |= MC_1;

    // Set capture/compare 0 to 999
    // 1kHz rate
    TA0CCR0 = 9999;

    // Set capture/compare 1 to 499
    // 50% duty cycle
    TA0CCR1 = 4999;

    // Set CCR1 to Reset/Set
    TA0CCTL1 |= OUTMOD_7;
}

int temperature_report = 0;

int main(void) {

    setup_watchdog();
    setup_pwm();

    temperature_init();
    serial_init();

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

                    set_pwm(pwm);

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

