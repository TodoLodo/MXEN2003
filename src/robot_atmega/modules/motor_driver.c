/**
 * motor_driver.c
 * Aaron Fredrick — MXEN2003, June 2024
 */

#include "motor_driver.h"
#include <avr/io.h>

/* L298 IN1/IN2 control left motor direction via PL2/PL3 */
#define L_IN1 PL2
#define L_IN2 PL3

/* L298 IN3/IN4 control right motor direction via PL4/PL5 */
#define R_IN1 PL4
#define R_IN2 PL5

static uint16_t dutyToOCR(uint8_t duty_pct)
{
    return (uint16_t)((uint32_t)duty_pct * (ICR1 + 1) / 100);
}

void motorDriverInit(void)
{
    /* Fast PWM mode 14 (ICR1 as TOP), non-inverting on OC1A and OC1B */
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);

    ICR1  = 499; /* 500 Hz at 16 MHz with prescaler 64 */
    OCR1A = 0;
    OCR1B = 0;

    /* OC1A = PB5, OC1B = PB6 as outputs */
    DDRB |= (1 << PB5) | (1 << PB6);

    /* L298 direction pins as outputs */
    DDRL |= (1 << L_IN1) | (1 << L_IN2) | (1 << R_IN1) | (1 << R_IN2);
    PORTL &= ~((1 << L_IN1) | (1 << L_IN2) | (1 << R_IN1) | (1 << R_IN2));
}

void motorDriverSet(Motor motor)
{
    uint8_t l_duty = (uint8_t)((uint32_t)motor.l_val * 100 / 511);
    uint8_t r_duty = (uint8_t)((uint32_t)motor.r_val * 100 / 511);

    OCR1A = dutyToOCR(l_duty);
    OCR1B = dutyToOCR(r_duty);

    if (motor.l_dir)
    {
        PORTL |=  (1 << L_IN1);
        PORTL &= ~(1 << L_IN2);
    }
    else
    {
        PORTL &= ~(1 << L_IN1);
        PORTL |=  (1 << L_IN2);
    }

    if (motor.r_dir)
    {
        PORTL |=  (1 << R_IN1);
        PORTL &= ~(1 << R_IN2);
    }
    else
    {
        PORTL &= ~(1 << R_IN1);
        PORTL |=  (1 << R_IN2);
    }
}

void motorDriverBrake(void)
{
    OCR1A  = 0;
    OCR1B  = 0;
    PORTL |= (1 << L_IN1) | (1 << L_IN2) | (1 << R_IN1) | (1 << R_IN2);
}
