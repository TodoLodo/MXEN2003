/**
 * @file Controller.c
 * @author Todo Lodo
 * @brief Controller file for MXEN2003 project
 * @version 0.1
 * @date 2024-05-23
 *
 * @copyright Copyright (c) 2024
 */

#include "Controller.h"
#include <math.h>

#define DEADZONE_RADIUS 10
#define MAX_DELTA_ANGLE 20

/**
 * globals
 *
 */
volatile uint8_t camera_angle[] = {90, 90};
volatile uint8_t auto_state = 0;
volatile uint8_t battery_level;

int main(void)
{
	Motor motor;		  // for motor speeds
	char serial_str[100]; // for serial debugging
	uint64_t send_bits;

	milliseconds_init();
	button_init(); // for external interrupts on INT1, INT2 & INT2
	adc_init();
	serial0_init(); // for serial monitor
	serial1_init(); // for xbee comminucation with the robo

	sei();

	_delay_ms(20);

	while (1) // main loop
	{
		if (milliseconds_now() >= 100)
		{
			milliseconds_reset();

			computeMotorSpeeds(&motor);
			computeCameraAngles(camera_angle);

			serial1_write_byte(255); // send start

			send_bits = ((uint64_t)1 << 37) | ((uint64_t)auto_state << 36) | ((uint64_t)compressCameraData(camera_angle) << 20) | ((uint64_t)compressMotorData(motor) << 0);
			for (uint8_t i = 0; i < 6; i++)
			{
				serial1_write_byte((uint8_t)(send_bits >> (i * 7)));
			}

			serial1_write_byte(254); // send end

			sprintf(serial_str, "Motor<(%u, %3u)L (%u, %3u)R>, Camera<(%3u)X (%3u)Y>\n", motor.l_dir, motor.l_val, motor.r_dir, motor.r_val, camera_angle[0], camera_angle[1]);
			serial0_print_string(serial_str);
		}
	}
	return (1);
} // end main

// ISRs
/**
 * @ingroup ISRsController ISRs in Controller
 * @brief Interrupt service routine followed when INT0 pin recieves a signal.
 * @details When pin INT0 recieves a LOW to HIGH signal and external interruption is caused resulting this function routine to run.
 *
 * @details In this routine takes the responsibilty of changing the speed mode flag, which is later transmitted in the main() function where results are visually respresntable on the working robo.
 *
 * @param INT0_vect vector for INT0
 */
ISR(INT0_vect)
{
}

/**
 * @ingroup ISRsController ISRs in Controller
 * @brief Interrupt service routine followed when INT1 pin recieves a signal.
 * @details When pin INT1 recieves a LOW to HIGH signal and external interruption is caused resulting this function routine to run.
 * @details In this routine takes the responsibilty of resetting the camera angles to 90 degrees, which is later transmitted in the main() function where results are visually respresntable on the working robo.
 *
 * @param INT1_vect vector for INT1
 */
ISR(INT1_vect)
{
	camera_angle[0] = 90;
	camera_angle[1] = 90;
}

/**
 * @ingroup ISRsController ISRs in Controller
 * @brief Interrupt service routine followed when INT2 pin recieves a signal.
 * @details When pin INT2 recieves a LOW to HIGH signal and external interruption is caused resulting this function routine to run.
 * @details In this routine takes the responsibilty of toggling the robo autonomous state, which is later transmitted in the main() function where results are visually respresntable on the working robo.
 *
 * @param INT2_vect vector for INT2
 */
ISR(INT2_vect)
{
	auto_state = !auto_state;
}

// FUNCTIONS

void button_init()
{

	// Set the button pins as input
	DDRD &= ~((1 << PD0) | (1 << PD1) | (1 << PD2));

	// Configure INT0 and INT1 to trigger on rising edge (button release)
	//     |             INT0             |              INT1             |              INT2            |
	EICRA = ((1 << ISC01) | (1 << ISC00)) | ((1 << ISC11) | (1 << ISC10)) | ((1 << ISC21) | (1 << ISC20)); // Rising edge triggers interrupt

	// Enable external interrupt INT0 (D0) & INT1 (D1)
	EIMSK = (1 << INT0) | (1 << INT1) | (1 << INT2);
}

/**
 * @ingroup MotorFunctionsController Motor Functions in Controller
 * @brief Map the joystick position to a range from -511 to 511 (used in computeMotorSpeeds()).
 *
 * @param adc_val adc value recieved from the joystick
 * @return int16_t
 */
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

/**
 * @ingroup MotorFunctionsController Motor Functions in Controller
 * @brief Compute each motor speed and direction.
 *
 * @param motor The memory address of the motor struct.
 */
void computeMotorSpeeds(Motor *motor)
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

	motor->l_val = abs(motor_speed[0]);
	motor->l_dir = (motor_speed[0] >= 0) ? 1 : 0;
	motor->r_val = abs(motor_speed[1]);
	motor->r_dir = (motor_speed[1] >= 0) ? 1 : 0;
}

/**
 * @ingroup MotorFunctionsController Motor Functions in Controller
 * @brief Compress motor struct data imto 20 bits.
 * @details This function write all motor data into 20 bits.
 * @details The bit structure is as follows.
 *
 * 			* sdbl (9 bits) - speed data bit for left motor.
 * 			* sdbr (9 bits) - speed data bit for right motor.
 * 			* ddbl (1 bit)  - direction data bit for left motor.
 * 			* ddbl (1 bit)  - direction data bit for right motor.
 * 			* nu            - not used.
 *
 *
 * 			.  31  .  30  .  29  .  28  .  27  .  26  .  25  .  24  .  23  .  22  .  21  .  20  .  19  .  18  .  17  .  16  .  15  .  14  .  13  .  12  .  11  .  10  .  09  .  08  .  07  .  06  .  05  .  04  .  03  .  02  .  01  .  00  .
 *
 * 			|  nu  |  nu  |  nu  |  nu  |  nu  |  nu  |  nu  |  nu  |  nu  |  nu  |  nu  |  nu  | ddbr | sdbr | sdbr | sdbr | sdbr | sdbr | sdbr | sdbr | sdbr | sdbr | ddbl | sdbl | sdbl | sdbl | sdbl | sdbl | sdbl | sdbl | sdbl | sdbl |
 *
 * @param motor The memory address of the motor struct.
 * @return uint32_t, compressed motor data.
 */
uint32_t compressMotorData(Motor motor)
{
	return (uint32_t)(((uint32_t)motor.r_dir << 19) | ((uint32_t)motor.r_val << 10) | ((uint32_t)motor.l_dir << 9) | ((uint32_t)motor.l_val << 0));
}

int8_t deltaAnglevalue(uint16_t adc_val)
{
	int8_t d_angle = 0;

	if (abs(adc_val - 511.5) >= DEADZONE_RADIUS)
	{
		d_angle = ((adc_val - 511.5 - ((adc_val - 511.5) / (abs(adc_val - 511.5)) * DEADZONE_RADIUS)) / (511.5 - DEADZONE_RADIUS)) * MAX_DELTA_ANGLE;
	}

	return d_angle;
}

void computeCameraAngles(volatile uint8_t camera_angle[2])
{
	int8_t _tmp;
	char str[20];

	// x - axis
	_tmp = deltaAnglevalue(adc_read(2));
	if ((int16_t)camera_angle[0] +_tmp > 180)
	{
		camera_angle[0] = 180;
	}
	else if ((int16_t)camera_angle[0] + _tmp < 0)
	{
		camera_angle[0] = 0;
	}
	else
	{
		camera_angle[0] += _tmp;
	}
	

	// y - axis
	_tmp = deltaAnglevalue(adc_read(3));
	if ((int16_t)camera_angle[1] +_tmp > 180)
	{
		camera_angle[1] = 180;
	}
	else if ((int16_t)camera_angle[1] + _tmp < 0)
	{
		camera_angle[1] = 0;
	}
	else
	{
		camera_angle[1] += _tmp;
	}
}

/**
 * @ingroup CamFunctionsController Camera Functions in Controller
 * @brief Compress camera data imto 16 bits.
 * @details This function write all camera data into 16 bits.
 * @details The bit structure is as follows.
 *
 * 			* adbx (9 bits) - angle data bit for x-axis.
 * 			* adby (9 bits) - angle data bit for y-axis.
 *
 *
 * 			.  15  .  14  .  13  .  12  .  11  .  10  .  09  .  08  .  07  .  06  .  05  .  04  .  03  .  02  .  01  .  00  .
 *
 * 			| adby | adby | adby | adby | adby | adby | adby | adby | adbx | adbx | adbx | adbx | adbx | adbx | adbx | adbx |
 *
 * @param camera_angle camer angles array.
 * @return uint8_t, compressed camera data.
 * 
 */
uint16_t compressCameraData(volatile uint8_t camera_angle[2])
{
	return (uint16_t)(((uint16_t)camera_angle[0] << 8) | ((uint16_t)camera_angle[0] << 0));
}
