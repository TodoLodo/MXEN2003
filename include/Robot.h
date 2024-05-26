//Example ATmega2560 Project
//File: ATmega2560Project.h
//Author: Robert Howie

#ifndef CONTROLLER_H_ //double inclusion guard
#define CONTROLLER_H_

//include standard libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

//include header files
#include "serial.h" //minimal serial lib
#include "adc.h" //minimal adc lib
#include "milliseconds.h" //milliseconds timekeeping lib
#include "hd44780.h" //LCD lib
#include "motor.h"


//constants
#define BUILD_DATE __TIME__ " " __DATE__"\n"

// TODO: docstring 
void motorInit();

// TODO: docstring 
void ledInit();

// TODO: docstring 
void ledUpdate();

// TODO: docstring 
void decompressMotorData(uint32_t _data);

// TODO: docstring 
void decompressCameraData(uint16_t _data);

#endif /* ATMEGA2560_H_ */