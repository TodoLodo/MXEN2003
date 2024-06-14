/**
 * calibration.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#include <stdint.h>

/* Sharp GP2Y0A21 — 4800/(adc-20), clamped 10–80 cm */
float calibrateShortRange1(uint16_t adc);

/* Alternate short-range model — 4500/(adc-20), clamped 10–80 cm */
float calibrateShortRange2(uint16_t adc);

/* Sharp GP2Y0A02 — 9462/(adc-16.92), clamped 20–150 cm */
float calibrateLongRange(uint16_t adc);

#endif /* CALIBRATION_H_ */
