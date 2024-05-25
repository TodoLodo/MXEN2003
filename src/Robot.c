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
	serial2_init();
	led_init();

	_delay_ms(20);

	while (1) // main loop
	{
		led_control();
		
		if (auto_state & speed_state) {
			// TODO: auto algorithm
		}
		else if (!auto_state)
		{
			// TODO: manual control operations
		}
		
	}
	return (1);
} // end main

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
		// TODO:decompress motor data

		// TODO:decompress camera data

		auto_state = ((recieved_bits >> 36) & 1);
		speed_state = ((recieved_bits >> 37) & 1);
	}
}

void led_init()
{
	DDRA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);
	PORTA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3);

	_delay_ms(1000);

	PORTA = 0;
}

void led_control()
{
	//     |       auto ON       |       auto OFF       |       speed 0        |       speed 1        |
	PORTA |= (auto_state << PA0) | (!auto_state << PA1) | (speed_state << PA2) | (!speed_state << PA3);
}