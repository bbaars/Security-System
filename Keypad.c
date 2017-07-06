/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "Variables.h"
#include "Keypad.h"
#include "LCD.h"

uint8_t translate(uint8_t incoming);
uint8_t DebounceSwitches(void);
void detectPress(void);

uint8_t button = 0;
uint16_t status = 0;

/******************************************************************************
 * Initializes the buttons to be inputs
 *****************************************************************************/
void KeyPad_Init(void){

	/* set rows to input*/
		P4DIR &= ~0x0F;
}

/*****************************************************************************
 * This function returns the button the user pressed on the keypad
 ****************************************************************************/
uint8_t KeyPad_getInput(void){

	uint8_t pushedButton = 0;

	detectPress();
	pushedButton = translate(button);

	if(pushedButton != 12)
		return pushedButton;
//	else
//		KeyPad_getInput();

	return 0;
}

/******************************************************************************
 * Checks that the input entered matches the user PIN
 *****************************************************************************/
bool KeyPad_checkForCorrectPin(void){

	uint8_t i = 0, correct= 0;

	for(i = 0 ; i < 4 ; i++){
		if(tempPin[i] == pin[i])
			correct++;
	}

	if(correct == 4)
		return true;
	return false;
}

/*******************************************************************************
 * Waits for user input
 ******************************************************************************/
void detectPress(void)
{
	while(!DebounceSwitches());
}

/******************************************************************************
 * Scans the rows and columns to find out what button was pressed on the keypad
 *****************************************************************************/
void scanKeys(void){

	P4DIR |= 0x10;						// set column C0 to output, other colums remain high Z
	P4OUT &= ~0x10;						// set column C0 to LOW
	__delay_cycles(10);					// delay aboit 3.3 us to allow column line to settle
	status = (P4IN & 0x0F) >> 0;		// read all row input pins with C0 low

	P4DIR &= ~0x10;						// set column C0 to input, high Z state
	P4DIR |= 0x20;						// set column C1 to output, other colums remain high Z
	P4OUT &= ~0x20;						// set column C1 to LOW
	__delay_cycles(10);			    	// delay aboit 3.3 us to allow column line to settle
	status = (status << 4) | ((P4IN & 0x0F) >> 0);	//read all row input pins with C1 low

	P4DIR &= ~0x20;						// set column C1 to input, high Z state
	P4DIR |= 0x40;						// set column C2 to output, other colums remain high Z
	P4OUT &= ~0x40;						// set column C2 to LOW
	__delay_cycles(10);			    	// delay aboit 3.3 us to allow column line to settle
	status = (status << 4) | ((P4IN & 0x0F) >> 0);	//read all row input pins with C2 low

	P4DIR &= ~0x40;						// set column C2 to input
}

/********************************************************************************
 * Debounces the key pressed by waiting for steady state input
 *******************************************************************************/
uint8_t DebounceSwitches(){

	static uint16_t interval[12] = {0};

	scanKeys();

	for(button = 1; button <= 12; button++)
	{
		// get switch results, upper 5 bits of interval are don't cares
		interval[button-1] = (interval[button-1]<<1) | ((status>>(button-1)) & 0x0001) | 0xf800;

		//Assuming the contacts return zero for a closed condition,
		//the routine returns 1 till ten sequential closures are detected
		if(interval[button-1] == 0xfc00)
		{
			return button;
		}
	}


	return 0;
}

/****************************************************************************
 * Translates the button pressed to the physical keypad number
 *****************************************************************************/
uint8_t translate(uint8_t incoming)
{
	//incoming is the value of the button pressed
	switch(incoming)
	{
	case 9:
		return 1; //1
	case 5:
		return 2; //2
	case 1:
		return 3; //3
	case 10:
		return 4; //4
	case 6:
		return 5; //5
	case 2:
		return 6; //6
	case 11:
		return 7; //7
	case 7:
		return 8; //8
	case 3:
		return 9; //9
	case 12:
		return 12;//*
	case 8:
		return 0; //0
	case 4:
		return 11;//#
	//case 14:
		//return 14;
	default:
		return 13;
	}
}
