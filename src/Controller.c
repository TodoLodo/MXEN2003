// MXEN2003 Project
// File: Controller.c
// Controller file for MXEN2003 project

// include this .c file's header file
#include "Controller.h"
#include <math.h>

#define DEADZONE_RADIUS 10
#define MAX_DELTA_ANGLE 20

// globals
volatile uint8_t auto_state = 0;


int main(void)
{
	int16_t motor_speed[2];	 // for motor speeds
	uint8_t camera_angle[2]; // for camera angles
	char serial_str[100];	 // for serial debugging

	milliseconds_init();
	adc_init();
	serial0_init(); // for serial monitor
	serial1_init(); // for xbee comminucation with the robo

	_delay_ms(20);

	while (1) // main loop
	{
		if (milliseconds_now() >= 100)
		{
			milliseconds_reset();

			computeMotorSpeeds(motor_speed);
			computeCameraAngles(camera_angle);

			serial1_write_byte(255); // send start
			
			serial1_write_byte(254); // send end

			sprintf(serial_str, "l: %4d, R: %4d\n", motor_speed[0], motor_speed[1]);
			serial0_print_string(serial_str);
		}
	}
	return (1);
} // end main

int16_t axisValue(uint16_t adc_val)
{
	int16_t return_val;

	if (abs(adc_val - 511.5) > DEADZONE_RADIUS)
	{
		return_val = ((adc_val - 511.5 - ((adc_val - 511.5) / (abs(adc_val - 511.5)) * DEADZONE_RADIUS)) / (511.5 - DEADZONE_RADIUS)) * 512;
	}
	else
	{
		return_val = 0;
	}

	return return_val;
}

void computeMotorSpeeds(uint16_t motor_speed[2])
{

	int16_t x, y, motor_speed[2];
	uint16_t d;
	double theta;

	// X-axis
	x = axisValue(adc_read(0));

	// Y-axis
	y = axisValue(adc_read(1));

	// D
	d = sqrt(square(x) + square(y));
	if (d > 511)
	{
		d = 511;
	}

	// THETA
	theta = atan2(x, y);

	// MOTOR SPEED
	if (-M_PI <= theta && theta <= 0)
	{
		motor_speed[0] = (int16_t)((double)d * (double)(y / abs(y)) * cos(2 * theta));
		motor_speed[1] = d * (y / abs(y));
	}
	else if (0 <= theta && theta <= M_PI)
	{
		motor_speed[0] = d * (y / abs(y));
		motor_speed[1] = (int16_t)((double)d * (double)(y / abs(y)) * cos(2 * theta));
	}
}

uint8_t deltaAnglevalue(uint16_t adc_val)
{
	uint8_t half_val = adc_val - 512;
	uint8_t sign = half_val / abs(half_val);
	uint8_t d_angle = 0;

	if (abs(half_val) >= DEADZONE_RADIUS)
	{
		d_angle = sign * (abs(half_val) - DEADZONE_RADIUS) * MAX_DELTA_ANGLE / (512 - DEADZONE_RADIUS);
	}

	return d_angle;
}

void computeCameraAngles(uint8_t camera_angle[2])
{
	// x - axis
	camera_angle[0] += deltaAnglevalue(adc_read(2));
	if (camera_angle[0] > 180)
	{
		camera_angle[0] = 180;
	}
	else if (camera_angle[0] < 0)
	{
		camera_angle[0] = 0;
	}

	// y - axis
	camera_angle[1] += deltaAnglevalue(adc_read(3));
	if (camera_angle[1] > 180)
	{
		camera_angle[1] = 180;
	}
	else if (camera_angle[1] < 0)
	{
		camera_angle[1] = 0;
	}
}