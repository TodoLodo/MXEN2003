/**
 * motor.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef MOTOR_H_
#define MOTOR_H_

typedef struct
{
    unsigned int l_val : 9;
    unsigned int l_dir : 1;
    unsigned int r_val : 9;
    unsigned int r_dir : 1;
} Motor;

#endif /* MOTOR_H_ */