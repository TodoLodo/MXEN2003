/**
 * navigation.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef NAVIGATION_H_
#define NAVIGATION_H_

#include "motor.h"

/* Distance thresholds in cm */
#define NAV_FRONT_CLEAR_CM  25.0f
#define NAV_WALL_PRESENT_CM 18.0f
#define NAV_TURN_SPEED      200
#define NAV_DRIVE_SPEED     350

void navigationInit(uint8_t adc_channel_short, uint8_t adc_channel_long);
void navigationStep(Motor *motor);

#endif /* NAVIGATION_H_ */
