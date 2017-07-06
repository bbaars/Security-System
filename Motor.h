/*
 * Motor.h
 *
 *  Created on: Nov 22, 2016
 *      Author: Brandon
 */

#ifndef MOTOR_H_
#define MOTOR_H_


/**********************************************************************
 *  Initializes the Pins to be Output
 *********************************************************************/
extern void Motor_Init(void);

/**********************************************************************
 *  Steps the motor 11.25 deg, 32 steps for a full 360deg rotation
 *  It sets each bit to high, steps it to low, sets the next to high and
 *  repeats this until 360 deg rotation occurs. 64:1 gear ratio,
 *
 *  64 (ratio) x 8 (teeth) = 512 steps
 *********************************************************************/
extern void Motor_Unlock(void);

/**********************************************************************
 *  Steps the motor 11.25 deg, 32 steps for a full 360deg rotation
 *  It sets each bit to high, steps it to low, sets the next to high and
 *  repeats this until 360 deg rotation occurs.
 *
 *  64 (ratio) x 8 (teeth) = 512 steps
 *********************************************************************/
extern void Motor_Lock(void);


#endif /* MOTOR_H_ */
