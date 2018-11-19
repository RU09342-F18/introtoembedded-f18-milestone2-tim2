#include <msp430.h>
#include "pwm.h"

void pwm_init() {
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

void pwm_manual_set(int value) {
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

