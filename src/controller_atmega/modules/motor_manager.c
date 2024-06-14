/**
 * motor_manager.c
 * Aaron Fredrick — MXEN2003, June 2024
 */

#include "motor_manager.h"
#include <math.h>
#include <stdlib.h>

#define DEADZONE_RADIUS  10
#define ADC_MID          511.5f
#define ADC_RANGE        (ADC_MID - DEADZONE_RADIUS)

static int16_t axisValue(uint16_t adc)
{
    float centered = adc - ADC_MID;
    if (fabsf(centered) <= DEADZONE_RADIUS)
    {
        return 0;
    }
    float sign      = (centered > 0) ? 1.0f : -1.0f;
    float deadbanded = centered - sign * DEADZONE_RADIUS;
    return (int16_t)((deadbanded / ADC_RANGE) * 512.0f);
}

void motorManagerComputeSpeeds(Motor *motor, uint16_t joy_x, uint16_t joy_y)
{
    int16_t x = axisValue(joy_x);
    int16_t y = axisValue(joy_y);

    float theta = atan2f((float)x, (float)y);
    float d     = sqrtf((float)(x * x) + (float)(y * y));
    if (d > 511.0f) d = 511.0f;

    int16_t l_speed, r_speed;
    int16_t sign = (y >= 0) ? 1 : -1;

    if (theta >= -M_PI && theta <= 0.0f)
    {
        l_speed = (int16_t)(d * sign * cosf(2.0f * theta));
        r_speed = (int16_t)(d * sign);
    }
    else
    {
        l_speed = (int16_t)(d * sign);
        r_speed = (int16_t)(d * sign * cosf(2.0f * theta));
    }

    motor->l_val = (uint16_t)abs(l_speed);
    motor->l_dir = (l_speed >= 0) ? 1 : 0;
    motor->r_val = (uint16_t)abs(r_speed);
    motor->r_dir = (r_speed >= 0) ? 1 : 0;
}

void motorManagerApplySpeedMode(Motor *motor, uint8_t speed_state)
{
    if (speed_state)
    {
        motor->l_val /= 2;
        motor->r_val /= 2;
    }
}
