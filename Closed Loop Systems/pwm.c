#include <msp430.h>
#include "pwm.h"
#include "temp.h"
#include "uart.h"

#define MAX_OUTPUT 10000
#define MIN_OUTPUT 0

#define MANUAL 0
#define AUTO 1
#define FAN_KICK_SPEED 4000
#define FAN_MIN_SPEED 1000

int mode = MANUAL;

int target = 0;

int kp = -100;
int ki = 0;

long integral_term = 0;

int output = 0;

int fan_kick_iters = 0;
int fan_kicked = 0;

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
    TA0CCR0 = MAX_OUTPUT-1;

    // Set capture/compare 1 to 499
    // 50% duty cycle
    TA0CCR1 = 0;

    // Set CCR1 to Reset/Set
    TA0CCTL1 |= OUTMOD_7;


    // Set timer 1 for pid loop

    // Use SMCLk
    TA1CTL |= TASSEL_2;

    // Continuous mode
    TA1CTL |= MC_2;

    // CCR0 interrupt enabled
    TA1CCTL0 = CCIE;

    TA1CCR0 = 50000;

}

int pwm_output() {
    return output;
}

void set_output(int value) {
    if (value <= 0) {
        TA0CCTL1 &= ~OUT;
        TA0CCTL1 &= ~OUTMOD_7;
        TA0CCTL1 |= OUTMOD_0;
        output = MIN_OUTPUT;
    } else if (value >= MAX_OUTPUT) {
        TA0CCTL1 |= OUT;
        TA0CCTL1 &= ~OUTMOD_7;
        TA0CCTL1 |= OUTMOD_0;
        output = MAX_OUTPUT;
    } else {
        TA0CCTL1 |= OUTMOD_7;
        TA0CCR1 = value;
        output = value;
    }
}

void pwm_manual_set(int value) {
    mode = MANUAL;
    set_output(value);
}

void pwm_auto_set(int value) {
    mode = AUTO;
    target = value;
}

void calculate_pid() {
    if (mode == AUTO) {

        int current_value = temperature_read();
        int error = target - current_value;

        long proportional_term = (long) kp * (long) error;
        integral_term += (long) ki * (long) error;

        // cap the integral to prevent integral windup
        if (integral_term > MAX_OUTPUT) {
            integral_term = MAX_OUTPUT;
        } else if (integral_term < MIN_OUTPUT) {
            integral_term = MIN_OUTPUT;
        }

        long new_output = proportional_term + integral_term;

        if (new_output > MAX_OUTPUT) {
            new_output = MAX_OUTPUT;
        } else if (new_output < MIN_OUTPUT) {
            new_output = MIN_OUTPUT;
        }

        if (error < 0 && new_output < FAN_MIN_SPEED) {
            new_output = FAN_MIN_SPEED;
        }

        if (fan_kick_iters > 0) {
            new_output = FAN_KICK_SPEED;
            fan_kick_iters--;
        } else if (new_output > FAN_MIN_SPEED  && new_output < FAN_KICK_SPEED && output < FAN_MIN_SPEED) {
            new_output = FAN_KICK_SPEED;
            fan_kick_iters = 10;
        }

        set_output((int) new_output);
    }
}

void pwm_p_set(int p) {
    kp = -p;
}


void pwm_i_set(int i) {
    ki = -i;
    if (i == 0) {
        integral_term = 0;
    }
}

// CCR0
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void) {
    calculate_pid();
}


