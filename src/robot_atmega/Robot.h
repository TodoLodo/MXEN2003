/**
 * Robot.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#include "comms.h"
#include "calibration.h"
#include "wifi.h"
#include "motor.h"
#include "motor_driver.h"
#include "navigation.h"
#include "serial.h"
#include "adc.h"
#include "milliseconds.h"

/* ADC channel assignments */
#define ADC_IR_SHORT    0
#define ADC_IR_LONG     1
#define ADC_BATTERY     2

/* Servo output via Timer3: pan on OC3A (PE3), tilt on OC3B (PE4) */
#define SERVO_PAN_MIN_OCR  1000
#define SERVO_PAN_MAX_OCR  4000
#define SERVO_TILT_MIN_OCR 1000
#define SERVO_TILT_MAX_OCR 4000

void ledInit(void);
void ledUpdate(uint8_t auto_state, uint8_t speed_state);
void servoInit(void);
void servoSetAngles(uint8_t pan_deg, uint8_t tilt_deg);
void buildTelemetryPacket(DataPacket *packet);

#endif /* ROBOT_H_ */