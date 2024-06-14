/**
 * Controller.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

#include "comms.h"
#include "calibration.h"
#include "wifi.h"
#include "motor.h"
#include "motor_manager.h"
#include "telemetry_display.h"
#include "serial.h"
#include "adc.h"
#include "milliseconds.h"

/* ADC channel assignments */
#define ADC_JOY_MOVE_X 0
#define ADC_JOY_MOVE_Y 1
#define ADC_JOY_CAM_X  2
#define ADC_JOY_CAM_Y  3

/* Max angular change applied per control cycle for camera pan/tilt */
#define CAM_MAX_DELTA_DEG 20

void buttonInit(void);
void computeCameraAngles(volatile uint8_t cam_angles[2]);
void buildControlPacket(DataPacket *packet, Motor motor, volatile uint8_t cam_angles[2]);

#endif /* CONTROLLER_H_ */