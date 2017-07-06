/*
 * Variables.h
 *
 *  Created on: Nov 21, 2016
 *      Author: Brandon
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

/* BOOLEANS */
bool isHomeButtonPushed, isLocked, isArmed, isHomeButtonPushed, is24, is12, didChangePin, isAM, isPM, isAnimations, hasDetected, isDoorOpened, isWindowOpened, isMotionDetected;

/* UINT16_T */
uint16_t textBackgroundColor, textColor, backgroundColor, tempF;

/* UINT8_T */
uint8_t buttonPressed, sizeCount, bcdhour, bcdmin, bcdday, bcddate, bcdmonth, bcdyear, shiftText, whatDetected;

char RTC_registers[19], sec, min, hour, day, date,
		month, year, tempL, tempH, AMPM;

uint8_t pin[4], tempPin[4];

/* STATES */
enum states {
	idle,
	disarmed,
	armed,
	settings,
	changePin,
	changeDate,
	alarms,
	customize,
	detection
} state;

#endif /* VARIABLES_H_ */
