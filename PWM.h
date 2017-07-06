/*
 * PWM.h
 *
 *  Created on: Nov 27, 2016
 *      Author: Brandon
 */

#ifndef PWM_H_
#define PWM_H_


/*****************************************************************************
 * Initializes the parameters for the PWM
 ***************************************************************************/
void PWM_init(void);

/*****************************************************************************
 * This function sets the duty cycle of the PWM
******************************************************************************/
void PWM_setDutyCycleBuzzer(uint8_t dcycle);

/*****************************************************************************
 * This function sets the duty cycle of the PWM for the solenoid
******************************************************************************/
void PWM_setDutyCycleSolenoid(uint8_t dcycle);


#endif /* PWM_H_ */
