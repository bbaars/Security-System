/*
 * Keypad.h
 *
 *  Created on: Oct 30, 2016
 *      Author: Brandon
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_


/*****************************************************************************
 * This function returns the button the user pressed on the keypad
 ****************************************************************************/
extern uint8_t KeyPad_getInput(void);

/******************************************************************************
 * Checks that the input entered matches the user PIN
 *****************************************************************************/
extern bool KeyPad_checkForCorrectPin(void);

/******************************************************************************
 * Initializes the buttons to be inputs
 *****************************************************************************/
extern void KeyPad_Init(void);

#endif /* KEYPAD_H_ */
