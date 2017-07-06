/*
 * Systick.h
 *
 *  Created on: Oct 19, 2016
 *      Author: Brandon
 */

#ifndef DELAY_H_
#define DELAY_H_

/*******************************************************************************
 * Initializes the Timer32 module to run off MCLK
 ********************************************************************************/
void Timer32_Init(void);

/*******************************************************************************
 * Sets the delay for the 32 bit timer
 ******************************************************************************/
void Timer32_Delay(uint32_t delay);

/******************************************************************************
 * This function uses the Systick timer to create a delay
 *****************************************************************************/
void SysTick_Delay(void);

#endif /* DELAY_H_ */
