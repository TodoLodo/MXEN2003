#ifndef MOTOR_H_
#define MOTOR_H_

/**
 * @defgroup MotorStruct Motor Struct
 * @brief Motor Struct for both Controller and Motor.
 * @{
 */

/**
 * @struct Motor
 * @brief Contains data of each left and right motors
 *
 * @author Todo Lodo
 */
typedef struct
{
	/**
	 * @var unsigned int l_val
	 * @brief speed magnitude of left motor.
	 */
	unsigned int l_val : 9;

	/**
	 * @var unsigned int r_val
	 * @brief speed magnitude of right motor.
	 */
	unsigned int r_val : 9;

	/**
	 * @var unsigned int l_dir
	 * @brief direction indicator of left motor.
	 */
	unsigned int l_dir : 1;

	/**
	 * @var unsigned int r_dir
	 * @brief direction indicator of left motor.
	 */
	unsigned int r_dir : 1;

} Motor;

/**
 * @}
 */

#endif