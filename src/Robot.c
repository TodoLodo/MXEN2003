/**
 * @file Robot.c
 * @author Todo Lodo
 * @brief Robot file for MXEN2003 project
 * @version 0.1
 * @date 2024-05-23
 *
 * @copyright Copyright (c) 2024
 */

#include "Robot.h"

/**
 * globals
 *
 */
volatile Motor motor;
volatile uint8_t camera_angle[] = {90, 90};
volatile uint8_t auto_state = 0;
volatile uint8_t speed_state = 0;

int main(void)
{
	Motor output_motor;
	uint16_t v;

	serial2_init();
	motorInit();
	ledInit();

	_delay_ms(20);

	while (1) // main loop
	{
		ledUpdate();

		if (auto_state)
		{
			v = 100 * speed_state; // ! this is a set speed for testing

			// TODO: auto algorithm
		}
		else if (!auto_state)
		{
			// ? switches between 0-100 duty cycle and 0-50 duty cycle
			motor.l_val /= (2 - speed_state);
			motor.r_val /= (2 - speed_state);
		}

		// TODO: data TX2 to controller (sensor readings and battery level)

		motoUpdate(); // ? testing required
	}
	return (1);
} // end main

// TODO: docstring 
ISR(USART2_RX_vect)
{
	static uint8_t rx_index;
	static uint64_t recieved_bits;
	uint8_t recieved_byte = UDR2;

	if (recieved_byte != 255 && recieved_byte != 254)
	{
		recieved_bits |= ((uint64_t)recieved_byte << (7 * 0));
		rx_index++;
	}
	else if (recieved_byte == 255)
	{
		rx_index = 0;
	}
	else if (recieved_byte == 254)
	{

		auto_state = ((recieved_bits >> 36) & 1);
		speed_state = ((recieved_bits >> 37) & 1);

		if (!auto_state)
		{
			decompressMotorData(recieved_bits); // ? testing required
		}

		decompressCameraData(recieved_bits >> 20); // ? testing required
	}
}

// TODO: docstring 
void motorInit()
{
	// Set Fast PWM mode with ICR1 as TOP value (mode 14)
	TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Prescaler = 64

	// TODO: configure digital I/O pins for motor direction

	// Set TOP value for 500 Hz PWM
	ICR1 = 499;

	// Set initial duty cycle to 0
	OCR1A = 0; // Duty cycle for pin PB1 (OC1A)
	OCR1B = 0; // Duty cycle for pin PB2 (OC1B)
}

uint16_t _OCRValue(uint8_t _duty_cycle)
{
	return (_duty_cycle * (ICR1 + 1) / 100) - 1;
}

// TODO: docstring 
void motoUpdate()
{
	// ? could reduce code line, and add further comments once finalised ?
	uint8_t _duty_cycle_l = (motor.l_val / 511) * 100;
	uint8_t _duty_cycle_r = (motor.r_val / 511) * 100;

	OCR1A = _OCRValue(_duty_cycle_l);
	OCR1B = _OCRValue(_duty_cycle_r);
}

// TODO: docstring 
void ledInit()
{
	DDRA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);
	PORTA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);

	_delay_ms(1000);

	PORTA = 0;
}

// TODO: docstring 
void ledUpdate()
{
	//    |   auto or manual   |   spead state 0 or 1  |
	PORTA |= (1 << auto_state) | (1 << 2 + speed_state);
}

// TODO: docstring 
void decompressMotorData(uint32_t _data)
{
	motor.l_val = _data;
	motor.l_dir = _data >> 9;
	motor.r_val = _data >> 10;
	motor.r_dir - _data >> 11;
}

// TODO: docstring 
void decompressCameraData(uint16_t _data)
{
	camera_angle[0] = _data;
	camera_angle[1] = _data >> 8;
}