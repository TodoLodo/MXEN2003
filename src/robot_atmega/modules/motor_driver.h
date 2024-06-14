/**
 * motor_driver.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_

#include "motor.h"

/* Timer1 fast PWM, 500 Hz. L298 direction: PL2/PL3 (left), PL4/PL5 (right) */
void motorDriverInit(void);
void motorDriverSet(Motor motor);
void motorDriverBrake(void);

#endif /* MOTOR_DRIVER_H_ */
