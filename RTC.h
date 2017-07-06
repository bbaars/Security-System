/*
 * RTC.h
 *
 *  Created on: Oct 19, 2016
 *      Author: Brandon
 */

#ifndef RTC_H_
#define RTC_H_

#define RTC_ADDRESS   0x68

enum time {

	secReg			= 0x00,		//seconds
	minReg			= 0x01,		//minutes
	hourReg			= 0x02,		//hour
	dayReg			= 0x03,		//day of week
	dateReg			= 0x04,		//day of month
	monthReg		= 0x05,		//month
	yearReg			= 0x06,		//year
	A1secReg		= 0x07,		//alarm 1 seconds
	A1minReg		= 0x08,		//alarm 1 minutes
	A1hourReg		= 0x09,		//alarm 1 hour
	A1daydateReg	= 0x0A,		//alarm 1 day of week & alarm 1 day of month
	A2minReg		= 0x0B,		//alarm 2 minutes
	A2hourReg		= 0x0C,		//alarm 2 hour
	A2daydateReg	= 0x0D,		//alarm 2 day of week and alarm 2 day of month
	ControlReg		= 0x0E, 	//control
	StatusReg		= 0x0F,		//status
	AgingReg		= 0x10,		//aging offset
	TempMSBReg		= 0x11,		//MSB of Temperature
	TempLSBReg		= 0x12,		//LSB of Temperature
};

/******************************************************************************
 * Initializes the RTC for use with I2C
 *
 * args: none
 * return: none
 ******************************************************************************/
extern void RTC_Init(void);

/******************************************************************************
 * Reads the Registers to find the corresponding date and time/temperature
 ******************************************************************************/
extern void RTC_readData(void);

/******************************************************************************
 * This writes the given data to the given register
 *****************************************************************************/
extern void RTC_writeData(void);

#endif /* RTC_H_ */
