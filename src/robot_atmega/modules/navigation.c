/**
 * navigation.c
 * Aaron Fredrick — MXEN2003, June 2024
 *
 * Left-wall-following algorithm. The robot tracks the left wall and turns
 * right when the front is blocked, or turns left when contact with the left
 * wall is lost.
 */

#include "navigation.h"
#include "calibration.h"
#include "adc.h"

typedef enum
{
    NAV_FORWARD,
    NAV_TURN_LEFT,
    NAV_TURN_RIGHT,
    NAV_REVERSE
} NavState;

static uint8_t  channel_short;
static uint8_t  channel_long;
static NavState state = NAV_FORWARD;

void navigationInit(uint8_t adc_channel_short, uint8_t adc_channel_long)
{
    channel_short = adc_channel_short;
    channel_long  = adc_channel_long;
    state         = NAV_FORWARD;
}

void navigationStep(Motor *motor)
{
    float dist_front = calibrateShortRange1(adc_read(channel_short));
    float dist_left  = calibrateLongRange(adc_read(channel_long));

    switch (state)
    {
        case NAV_FORWARD:
            if (dist_front < NAV_FRONT_CLEAR_CM)
            {
                state = NAV_TURN_RIGHT;
            }
            else if (dist_left > NAV_WALL_PRESENT_CM)
            {
                state = NAV_TURN_LEFT;
            }
            else
            {
                motor->l_val = NAV_DRIVE_SPEED;
                motor->r_val = NAV_DRIVE_SPEED;
                motor->l_dir = 1;
                motor->r_dir = 1;
            }
            break;

        case NAV_TURN_RIGHT:
            motor->l_val = NAV_TURN_SPEED;
            motor->r_val = NAV_TURN_SPEED;
            motor->l_dir = 1;
            motor->r_dir = 0;
            if (dist_front >= NAV_FRONT_CLEAR_CM)
            {
                state = NAV_FORWARD;
            }
            break;

        case NAV_TURN_LEFT:
            motor->l_val = NAV_TURN_SPEED;
            motor->r_val = NAV_TURN_SPEED;
            motor->l_dir = 0;
            motor->r_dir = 1;
            if (dist_left <= NAV_WALL_PRESENT_CM)
            {
                state = NAV_FORWARD;
            }
            break;

        case NAV_REVERSE:
            motor->l_val = NAV_DRIVE_SPEED;
            motor->r_val = NAV_DRIVE_SPEED;
            motor->l_dir = 0;
            motor->r_dir = 0;
            state = NAV_TURN_RIGHT;
            break;
    }
}
