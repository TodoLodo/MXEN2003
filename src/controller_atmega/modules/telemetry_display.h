/**
 * telemetry_display.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef TELEMETRY_DISPLAY_H_
#define TELEMETRY_DISPLAY_H_

#include <stdint.h>

void telemetryDisplayInit(void);
void telemetryDisplayUpdate(uint8_t battery_pct, float dist_short_cm, float dist_long_cm);

#endif /* TELEMETRY_DISPLAY_H_ */
