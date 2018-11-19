
#ifndef PWM_H
#define PWM_H

void pwm_init();
void pwm_manual_set(int value);
void pwm_auto_set(int value);
void pwm_p_set(double p);
void pwm_i_set(double i);

#endif
