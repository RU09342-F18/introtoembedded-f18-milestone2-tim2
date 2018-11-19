#include <msp430.h>
#include "temp.h"
#include "thermistor_10k.h"

void temperature_init() {
  P6SEL |= 0x01;                            // Enable A/D channel A0
  ADC12CTL0 = ADC12ON+ADC12SHT0_8+ADC12MSC; // Turn on ADC12, set sampling time
                                            // set multiple sample conversion
  ADC12CTL1 = ADC12SHP+ADC12CONSEQ_2;       // Use sampling timer, set mode
  ADC12IE = 0x01;                           // Enable ADC12IFG.0
  ADC12CTL0 |= ADC12ENC;                    // Enable conversions
  ADC12CTL0 |= ADC12SC;                     // Start conversion
}

double lookup_temp(int voltage) {

    return thermistor_table[voltage>>2]-273.0;

    //    int temp_index = voltage / THERMISTOR_STEP_DELTA;
    //
    //if (temp_index < 0) {
    //return 0.0;
    //} else if (temp_index = THERMISTOR_STEP_MAX) {
    //return thermistor_table[THERMISTOR_STEP_MAX];
    //} else if (temp_index > THERMISTOR_STEP_MAX) {
    //return 0.0;
    //} else {

    /*   |
     *   |                             *
     *   |                        *
     * T |                   *
     * e |              *
     * m |         *
     * p |    *
     *   |
     *   |
     *   +--------------------------------
     *            ADC reading
     */

    //double low_temp = thermistor_table[temp_index];
    //double high_temp = thermistor_table[temp_index+1];
    //
    //double slope = (high_temp - low_temp) / THERMISTOR_STEP_DELTA;
    //
    //double offset = slope * (voltage - temp_index * THERMISTOR_STEP_DELTA);
    //
    //double temp = low_temp + offset;
    //
    //return temp;
    //}
}

int current_adc = 0;
double temperature_read() {
    double temp = lookup_temp(current_adc);
    return temp;
}

/*
 * ADC Interrupt
 */
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void) {
    switch(__even_in_range(ADC12IV,34))
    {
        case  0: break;                           // Vector  0:  No interrupt
        case  2: break;                           // Vector  2:  ADC overflow
        case  4: break;                           // Vector  4:  ADC timing overflow
        case  6:                                  // Vector  6:  ADC12IFG0
                 current_adc = ADC12MEM0;
                 if (ADC12MEM0 >= 0x7ff)                 // ADC12MEM = A0 > 0.5AVcc?
                     P1OUT |= BIT0;                        // P1.0 = 1
                 else
                     P1OUT &= ~BIT0;                       // P1.0 = 0

        case  8: break;                           // Vector  8:  ADC12IFG1
        case 10: break;                           // Vector 10:  ADC12IFG2
        case 12: break;                           // Vector 12:  ADC12IFG3
        case 14: break;                           // Vector 14:  ADC12IFG4
        case 16: break;                           // Vector 16:  ADC12IFG5
        case 18: break;                           // Vector 18:  ADC12IFG6
        case 20: break;                           // Vector 20:  ADC12IFG7
        case 22: break;                           // Vector 22:  ADC12IFG8
        case 24: break;                           // Vector 24:  ADC12IFG9
        case 26: break;                           // Vector 26:  ADC12IFG10
        case 28: break;                           // Vector 28:  ADC12IFG11
        case 30: break;                           // Vector 30:  ADC12IFG12
        case 32: break;                           // Vector 32:  ADC12IFG13
        case 34: break;                           // Vector 34:  ADC12IFG14
        default: break;
    }
}
