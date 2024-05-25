/**
 * @file Controller.h
 * @authors
 * - Robot Howie
 * - Todo Lodo
 * @brief Controller file for MXEN2003 project
 * @version 0.1
 * @date 2024-05-23
 *
 * @copyright Copyright (c) 2024
 */

#ifndef CONTROLLER_H_ // double inclusion guard
#define CONTROLLER_H_

// include standard libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdio.h>

// include header files
#include "serial.h"		  //minimal serial lib
#include "adc.h"		  //minimal adc lib
#include "milliseconds.h" //milliseconds timekeeping lib
#include "hd44780.h"	  //LCD lib
#include "motor.h"

// constants
#define BUILD_DATE __TIME__ " " __DATE__ "\n"

/**
 * @defgroup ISRsController ISRs in Controller
 * @brief Interrupt Service Routines in Controller.
 */

// declarations
void button_init();

/**
 * @defgroup MotorFunctionsController Motor Functions in Controller
 * @brief Functions for motor operations and data.
 * @{
 */

/**
 * @brief Compute each motor speed and direction.
 *
 * @param motor The memory address of the motor struct.
 *
 * @author Todo Lodo
 */
void computeMotorSpeeds(Motor *motor);

/**
 * @brief Compress motor struct data imto 20 bits.
 *
 * @param motor The memory address of the motor struct.
 * @return uint32_t, compressed motor data.
 *
 * @author Todo Lodo
 */
uint32_t compressMotorData(Motor motor);

/**
 * @}
 */

/**
 * @defgroup CamFunctionsController Camera Functions in Controller
 * @brief Functions for camera operations and data.
 * @{
 */

/**
 * @brief Calculate camera angle corressponding to joystick input and previous angles.
 *
 * @param camera_angle camer angles array.
 *
 * @author Todo Lodo
 */
void computeCameraAngles(volatile uint8_t camera_angle[2]);

/**
 * @brief Compress camera data imto 16 bits.
 *
 * @param camera_angle camer angles array.
 * @return uint8_t, compressed camera data.
 *
 * @author Todo Lodo
 */
uint16_t compressCameraData(volatile uint8_t camera_angle[2]);

/**
 * @}
 */

#endif /* ATMEGA2560_H_ */