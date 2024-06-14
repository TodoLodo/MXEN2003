/**
 * motor_manager.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef MOTOR_MANAGER_H_
#define MOTOR_MANAGER_H_

#include <stdint.h>
#include "motor.h"

void motorManagerComputeSpeeds(Motor *motor, uint16_t joy_x, uint16_t joy_y);
void motorManagerApplySpeedMode(Motor *motor, uint8_t speed_state);

#endif /* MOTOR_MANAGER_H_ */
