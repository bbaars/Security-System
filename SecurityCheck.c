/***********************************************************************
 * Author: Brandon Baars
 * Date:
 ***********************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ST7735.h"
#include "LCD.h"
#include "SecurityCheck.h"
#include "Variables.h"
#include "PWM.h"

uint8_t DebounceSwitch();

/**********************************************************************
 * This function initializes the linear hall sensors to inputs
 * and sets the proper interrupts for each pin
 *********************************************************************/
void securityCheck_Init(void){

	/* sets the door sensor to input */
	//P2 -> DIR &=~ BIT3;

	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN7);
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN5);
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN3);
	//MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);

	/* sets the window, touch button, and motion detector to input */
	P5 -> DIR &=~ BIT7 | BIT1 | BIT2;

	/* clears the interrupt flag */
	//GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN7);
	GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);
	GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN2);
	GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN3);
	GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN7);
	GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN5);

	/* selects the pins to trigger on a low to high state transition */
	//GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN2, GPIO_LOW_TO_HIGH_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P2, GPIO_PIN3, GPIO_LOW_TO_HIGH_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P2, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
	GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);

	/* enables the interrupts */
	//GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN7);
	GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);
	GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN2);
	GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN3);
	GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7);
	GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN5);

	/* enables the interrupts */
	MAP_Interrupt_enableInterrupt(INT_PORT5);
	MAP_Interrupt_enableInterrupt(INT_PORT2);
	MAP_Interrupt_enableInterrupt(INT_PORT1);

	/* master enable */
	MAP_Interrupt_enableMaster();
}

/*********************************************************************
 * Window IRQ Handler, clears the flag and updates the LCD
 ********************************************************************/
void PORT5_IRQHandler(void){

	uint16_t motionDectectorStatus, buttonStatus;

	buttonStatus = GPIO_getInterruptStatus(GPIO_PORT_P5, GPIO_PIN2);

	//clears the flag from the given status
	motionDectectorStatus = GPIO_getInterruptStatus(GPIO_PORT_P5, GPIO_PIN1);

	GPIO_clearInterruptFlag(GPIO_PORT_P5, motionDectectorStatus);
	GPIO_clearInterruptFlag(GPIO_PORT_P5, buttonStatus);


	if(buttonStatus & GPIO_PIN2)
		state = settings;

	if(motionDectectorStatus & GPIO_PIN1){
		isMotionDetected = true;
		if(isArmed){
			hasDetected = true;
			whatDetected = 3;
			state = detection;
		}
	}
}


void PORT1_IRQHandler(void){

	uint16_t windowStatus = 0;

	windowStatus = GPIO_getInterruptStatus(GPIO_PORT_P1, GPIO_PIN5);
	GPIO_clearInterruptFlag(GPIO_PORT_P1, windowStatus);

	if(windowStatus & GPIO_PIN5){
		isWindowOpened = true;
			if(isArmed){
				hasDetected = true;
				whatDetected = 1;
				state = detection;
			}
	}
}

/*********************************************************************
 * Door IRQ Handler, clears the flag and updates the LCD
 ********************************************************************/
void PORT2_IRQHandler(void){

	uint16_t doorStatus, doorbellStatus;

	//clears the flag from the given status
	doorStatus = GPIO_getInterruptStatus(GPIO_PORT_P2, GPIO_PIN3);
	doorbellStatus = GPIO_getInterruptStatus(GPIO_PORT_P2, GPIO_PIN7);
	GPIO_clearInterruptFlag(GPIO_PORT_P2, doorStatus);
	GPIO_clearInterruptFlag(GPIO_PORT_P2, doorbellStatus);

	if(doorStatus & GPIO_PIN3){
		isDoorOpened = true;
		if(isArmed){
			hasDetected = true;
			whatDetected = 2;
			state = detection;
		}
	}
}

/*******************************************************************************
 * Debounces the door bell button
 ******************************************************************************/
uint8_t DebounceSwitch(void){

	static uint16_t buttonState = 0;

	//read switch
	buttonState = (buttonState << 1) | (P2IN & BIT7) >> 7 | 0xf800;

	if(buttonState == 0xfc00) 	// indicates 0 level is stable
		return 1;			//for 10 consecutive calls
	return 0;
}
