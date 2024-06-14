/**
 * calibration.c
 * Aaron Fredrick — MXEN2003, June 2024
 */

#include "calibration.h"

static float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float calibrateShortRange1(uint16_t adc)
{
    if (adc <= 20) return 80.0f;
    return clamp(4800.0f / (adc - 20.0f), 10.0f, 80.0f);
}

float calibrateShortRange2(uint16_t adc)
{
    if (adc <= 20) return 80.0f;
    return clamp(4500.0f / (adc - 20.0f), 10.0f, 80.0f);
}

float calibrateLongRange(uint16_t adc)
{
    if (adc <= 17) return 150.0f;
    return clamp(9462.0f / (adc - 16.92f), 20.0f, 150.0f);
}
