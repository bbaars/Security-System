/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "ST7735.h"
#include "RTC.h"
#include "SecurityCheck.h"
#include <stdio.h>
#include "Variables.h"
#include "SplashScreen.h"
#include "Keypad.h"
#include "Motor.h"
#include "Flash2.h"

uint8_t stepSizeY = 10, stepSizeX = 10;


void LCD_printDayOfWeek(uint8_t x, uint8_t y, uint8_t dayOfWeek, uint8_t size);
void LCD_pin(void);

/******************************************************************************
 * This Function initializes MCLK to 48Mhz frequency for the LCD to Function
 *****************************************************************************/
void LCD_ClockInit48Mhz(void){

	/* configure pins for peripheral/crystal usage */
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
			GPIO_PIN3 + GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

	/* enables external frequencies */
	MAP_CS_setExternalClockSourceFrequency(32000, 48000000);

	/* Starting HFXT in non-bypass mode without a timeout. Before we start
	 * we have to change VCORE to 1 to support 48Mhz clock */
	MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
	MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
	MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

	/* false means there are no timeouts set, will return when stable */
	MAP_CS_startHFXT(false);

	/* initializing MCLK to HFXT (effectively to 48 MHz) */
	MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);

	/* intializing the SMLK to 12 MHz */
	MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);

	ST7735_InitR(INITR_REDTAB);

	textBackgroundColor = ST7735_WHITE;
	textColor = ST7735_BLACK;
	backgroundColor = ST7735_WHITE;
	ST7735_FillScreen(ST7735_WHITE); //white color

	ST7735_SetRotation(2);

	shiftText = 50;
}

/******************************************************************************
 * Updates the time and temperature on the LCD Display
 *****************************************************************************/
void LCD_updateData(void){

	uint8_t x = 50, y = 6, size = 1, hourInt;

	RTC_readData();

	char time[15], dateString[15], temp[15];

	if(AMPM){
		snprintf(time, 15, "  %02X:%02X:%02X PM ", hour, min, sec);
		isPM = true;
	}
	else{
		snprintf(time, 15, "  %02X:%02X:%02X AM ", hour, min, sec);
		isAM = true;
	}

	tempF = (tempL) *1.8 + 32;

	snprintf(dateString, 12, "  %02X/%02X/%02X ", month, date, year);
	snprintf(temp, 15, "  %d deg F  ", tempF);

	ST7735_DrawString(2, 135, "TOUCH BUTTON", textColor, textBackgroundColor, size);
	ST7735_DrawString(2, 150, "FOR SETTINGS", textColor, textBackgroundColor, size);

	if(!isAnimations)
		shiftText = 50;

	LCD_printDayOfWeek(shiftText, y, day, size);
	ST7735_DrawString(shiftText, y + stepSizeY, time, textColor, textBackgroundColor, size);
	ST7735_DrawString(shiftText, y + 2*stepSizeY, dateString, textColor, textBackgroundColor, size);
	ST7735_DrawString(shiftText, y + 3*stepSizeY, temp, textColor, textBackgroundColor, size);

	hourInt = hour;

	//checks for morning on a 24/12 hour clock
	if((hourInt) <= 18 && isAM)
		ST7735_DrawString(x - 20, y + 5*stepSizeY, "GOOD MORNING", textColor, textBackgroundColor, size);

	//checks for afternoon on a 24 hour clock
	else if((hourInt) >= 1 && hourInt < 5 && isPM)
		ST7735_DrawString(x - 20, y + 5*stepSizeY, "GOOD AFTERNOON", textColor, textBackgroundColor, size);

	//if it's not morning or afternoon, it's defaulted to evening
	else
		ST7735_DrawString(x - 20, y + 5*stepSizeY, "GOOD EVENING", textColor, textBackgroundColor, size);
}

/*******************************************************************************
 * Prints the day of week corresponding to the value 1-7 read from the RTC
 ******************************************************************************/
void LCD_printDayOfWeek(uint8_t x, uint8_t y, uint8_t dayOfWeek, uint8_t size){

	if(dayOfWeek == 1)
		ST7735_DrawString(x, y, "  MONDAY ", textColor, textBackgroundColor, size);
	else if(dayOfWeek == 2)
		ST7735_DrawString(x, y, "  TUESDAY ", textColor, textBackgroundColor, size);
	else if(dayOfWeek == 3)
		ST7735_DrawString(x, y, "  WEDNESDAY ", textColor, textBackgroundColor, size);
	else if(dayOfWeek == 4)
		ST7735_DrawString(x, y, "  THURSDAY ", textColor, textBackgroundColor, size);
	else if(dayOfWeek == 5)
		ST7735_DrawString(x, y, "  FRIDAY ", textColor, textBackgroundColor, size);
	else if(dayOfWeek == 6)
		ST7735_DrawString(x, y, "  SATURDAY  ", textColor, textBackgroundColor, size);
	else if(dayOfWeek == 7)
		ST7735_DrawString(x, y, "  SUNDAY ", textColor, textBackgroundColor, size);
}

void LCD_showMenu(void){

	uint8_t size = 1, i;
	uint16_t  stepSizeY = 20, x = 10, y = 25;

	ST7735_FillScreen(backgroundColor);
	ST7735_FillScreen(backgroundColor);

	ST7735_DrawString(15, 10, "SETTINGS", textColor, textBackgroundColor, size);

	if(isAnimations){

		for(i = 0 ; i < x ; i++){
			ST7735_DrawString(i, y, " 1. HOME", textColor, textBackgroundColor, size);

			if(isArmed)
				ST7735_DrawString(i, y + stepSizeY, " 2. DISARM", textColor, textBackgroundColor, size);
			else
				ST7735_DrawString(i, y + stepSizeY, " 2. ARM", textColor, textBackgroundColor, size);

			if(isLocked)
				ST7735_DrawString(i, y + 2*stepSizeY, " 3. UNLOCK", textColor, textBackgroundColor, size);
			else
				ST7735_DrawString(i, y + 2*stepSizeY, " 3. LOCK", textColor, textBackgroundColor, size);

			ST7735_DrawString(i, y + 3*stepSizeY, " 4. VIEW ALARMS", textColor, textBackgroundColor, size);
			ST7735_DrawString(i, y + 4*stepSizeY, " 5. CHANGE DATE/TIME", textColor, textBackgroundColor, size);
			ST7735_DrawString(i, y + 5* stepSizeY, " 6. CHANGE PIN", textColor, textBackgroundColor, size);
			ST7735_DrawString(i, y + 6* stepSizeY, " 7. CUSTOMIZE", textColor, textBackgroundColor, size);

			//Delay1ms(1);
		}
	}
	else{
		ST7735_DrawString(x, y, "1. HOME", textColor, textBackgroundColor, size);

		if(isArmed)
			ST7735_DrawString(x, y + stepSizeY, "2. DISARM", textColor, textBackgroundColor, size);
		else
			ST7735_DrawString(x, y + stepSizeY, "2. ARM", textColor, textBackgroundColor, size);

		if(isLocked)
			ST7735_DrawString(x, y + 2*stepSizeY, "3. UNLOCK", textColor, textBackgroundColor, size);
		else
			ST7735_DrawString(x, y + 2*stepSizeY, "3. LOCK", textColor, textBackgroundColor, size);

		ST7735_DrawString(x, y + 3*stepSizeY, "4. VIEW ALARMS", textColor, textBackgroundColor, size);
		ST7735_DrawString(x, y + 4*stepSizeY, "5. CHANGE DATE/TIME", textColor, textBackgroundColor, size);
		ST7735_DrawString(x, y + 5* stepSizeY, "6. CHANGE PIN", textColor, textBackgroundColor, size);
		ST7735_DrawString(x, y + 6* stepSizeY, "7. CUSTOMIZE", textColor, textBackgroundColor, size);

	}


}

/*******************************************************************************
 * This function locks and unlocks the door
 ********************************************************************************/
void LCD_motor(void){

	MAP_WDT_A_clearTimer();

	uint8_t size = 3;
	uint16_t  x = 20, y = 60;

	ST7735_FillScreen(backgroundColor);
	char buttons[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	ST7735_FillScreen(backgroundColor);


	if(isLocked)
		ST7735_DrawString(2, 5, "ENTER PIN TO UNLOCK", textColor, textBackgroundColor, 1);
	else
		ST7735_DrawString(2, 5, "ENTER PIN TO LOCK", textColor, textBackgroundColor, 1);

	tempPin[0]=KeyPad_getInput();
	ST7735_DrawCharS(x, y, buttons[tempPin[0]], textColor, textBackgroundColor, size);
	tempPin[1]=KeyPad_getInput();
	ST7735_DrawCharS(x + 20, y, buttons[tempPin[1]], textColor, textBackgroundColor, size);
	tempPin[2]=KeyPad_getInput();
	ST7735_DrawCharS(x + 40, y, buttons[tempPin[2]], textColor, textBackgroundColor, size);
	tempPin[3]=KeyPad_getInput();
	ST7735_DrawCharS(x + 60, y, buttons[tempPin[3]], textColor, textBackgroundColor, size);
	Delay1ms(2000);

	ST7735_FillScreen(backgroundColor);

	x = 2;
	y = 5;
	size = 1;
	if(KeyPad_checkForCorrectPin() && isLocked){
		ST7735_DrawString(x, y, "UNLOCKING...", textColor, textBackgroundColor, size);
		Motor_Unlock();
		isLocked = false;
		state = idle;
	} else if(KeyPad_checkForCorrectPin() && !isLocked){
		ST7735_DrawString(x, y, "LOCKING...", textColor, textBackgroundColor, size);
		Motor_Lock();
		isLocked = true;
		isArmed = true;
		state = armed;
		WDT_A_holdTimer();
		whatDetected = 5;
		FlashWrite();
		WDT_A_startTimer();
	}
	else{
		ST7735_DrawString(1, 60, "WRONG PIN", textColor, textBackgroundColor, 2);
		Delay1ms(1000);
		state = settings;
	}

	MAP_WDT_A_clearTimer();

	ST7735_FillScreen(backgroundColor);
	ST7735_FillScreen(backgroundColor);

}

/*******************************************************************************
 * This function allows the user to customize the system to their preferences
 ******************************************************************************/
void LCD_customize(void){

	MAP_WDT_A_clearTimer();

	uint8_t size = 1;
	uint16_t  x = 2, y= 5;

	buttonPressed = 0;

	ST7735_FillScreen(backgroundColor);
	ST7735_DrawString(x, y, "CUSTOMIZE", textColor, textBackgroundColor, size);
	y = 20;

	//prompts the user to select background color and text
	ST7735_DrawString(x, y + stepSizeY, "Background Color: ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 2*stepSizeY, "1: Blue  ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 3*stepSizeY, "2: Green ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 4*stepSizeY, "3: Black ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 5*stepSizeY, "4: White ", textColor, textBackgroundColor, size);

	//gets the background color and changes accordingly
	buttonPressed = KeyPad_getInput();

	if(buttonPressed == 1)
		backgroundColor = ST7735_BLUE;
	else if(buttonPressed == 2)
		backgroundColor = ST7735_GREEN;
	else if(buttonPressed == 3)
		backgroundColor = ST7735_BLACK;
	else if(buttonPressed == 4)
		backgroundColor = ST7735_WHITE;
	else{
		ST7735_DrawString(x, y + 6*stepSizeY, "Defaulted", textColor, textBackgroundColor, size);
		backgroundColor = ST7735_WHITE;
	}

	if(textColor == backgroundColor && backgroundColor != ST7735_BLACK)
		backgroundColor = ST7735_BLACK;
	else if(textColor == backgroundColor && backgroundColor == ST7735_BLACK)
		textColor = ST7735_WHITE;

	textBackgroundColor = backgroundColor;

	ST7735_FillScreen(backgroundColor);
	Delay1ms(750);

	ST7735_DrawString(x, y + stepSizeY, "Text Color: ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 2*stepSizeY, "1: Black  ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 3*stepSizeY, "2: White ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 4*stepSizeY, "3: Red ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 5*stepSizeY, "4: Yellow ", textColor, textBackgroundColor, size);

	buttonPressed = KeyPad_getInput();

	if(buttonPressed == 1)
		textColor = ST7735_BLACK;
	else if(buttonPressed == 2)
		textColor = ST7735_WHITE;
	else if(buttonPressed == 3)
		textColor = ST7735_RED;
	else if(buttonPressed == 4)
		textColor = ST7735_YELLOW;
	else{
		ST7735_DrawString(x, y + 6*stepSizeY, "Defaulted", textColor, textBackgroundColor, size);
		textColor = ST7735_BLACK;
		Delay1ms(500);
	}

	if(textColor == backgroundColor && backgroundColor!= ST7735_BLACK)
		textColor = ST7735_BLACK;
	else if(textColor == backgroundColor && backgroundColor == ST7735_BLACK)
		textColor = ST7735_WHITE;

	textBackgroundColor = backgroundColor;

	ST7735_FillScreen(backgroundColor);
	Delay1ms(500);

	ST7735_DrawString(x, y + stepSizeY, "Animations: ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 2*stepSizeY, "1: Yes  ", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 3*stepSizeY, "2: No ", textColor, textBackgroundColor, size);

	//gets the background color and changes accordingly
	buttonPressed = KeyPad_getInput();

	if(buttonPressed == 1)
		isAnimations = true;
	else if(buttonPressed == 2)
		isAnimations = false;
	else{
		ST7735_FillScreen(backgroundColor);
		ST7735_DrawString(x, y + 3*stepSizeY, "Defaulted: Yes ", textColor, textBackgroundColor, size);
		Delay1ms(500);
	}

	MAP_WDT_A_clearTimer();

	MAP_WDT_A_holdTimer();
	FlashWrite();
	MAP_WDT_A_startTimer();

	state = settings;
}
/*******************************************************************************
 * Goes back to showing the state of the system
 ******************************************************************************/
void LCD_displayHome(void){

	ST7735_FillScreen(backgroundColor);
	state = idle;
}

/******************************************************************************
 * Shows the last 5 alarm trips and where they were from
 *****************************************************************************/
void LCD_showAlarm(void){

	MAP_WDT_A_clearTimer();

	uint8_t size = 1;
	uint16_t  x = 2, y = 5;

	ST7735_FillScreen(backgroundColor);

	ST7735_DrawString(x, y, "Recent Alarm Trips &", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 10, "Arm & Disarm Events", textColor, textBackgroundColor, size);
	ST7735_DrawString(2, 135, "TOUCH BUTTON", textColor, textBackgroundColor, size);
	ST7735_DrawString(2, 150, "TO RETURN", textColor, textBackgroundColor, size);

	MAP_WDT_A_holdTimer();
	FlashRead();
	MAP_WDT_A_startTimer();
	state = alarms;

	//update the recent alarm trips from the alarm class to display what was trippeed and when
}

/******************************************************************************
 * Changes the date and time
 *****************************************************************************/
void LCD_changeDateTime(void){

	MAP_WDT_A_clearTimer();

	uint8_t size = 1, firstTerm, secondTerm;
	uint16_t  x = 2, y= 5;
	char buttons[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	buttonPressed = 0;

	ST7735_FillScreen(backgroundColor);
	ST7735_DrawString(x, y, "Change Date & Time", textColor, textBackgroundColor, size);
	ST7735_DrawString(x, y + 12, "Press # To Enter", textColor, textBackgroundColor, size);

	//update the wait for user input from keypad to change date and time
	uint8_t stepSizeX = 30, stepSizeY = 10;
	y = 20;

	//Gets the input for the hour
	ST7735_DrawString(x, y + stepSizeY, "Hour: ", textColor, textBackgroundColor, size);
	firstTerm = KeyPad_getInput();
	ST7735_DrawCharS(x + stepSizeX, y + stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);
	secondTerm = KeyPad_getInput();

	//if the user is entering a two digit number
	if(secondTerm != 11){
		ST7735_DrawCharS(x + stepSizeX + 6,  y + stepSizeY, buttons[secondTerm],textColor, textBackgroundColor, size);
		bcdhour = (firstTerm << 4) | secondTerm;
	}
	else
		bcdhour = firstTerm;

	//Gets the input for the min
	ST7735_DrawString(x, y + 2*stepSizeY, "Min: ", textColor, textBackgroundColor, size);
	firstTerm = KeyPad_getInput();
	ST7735_DrawCharS(x + stepSizeX, y + 2*stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);
	secondTerm = KeyPad_getInput();

	//if the user is entering a two digit number
	if(secondTerm != 11){
		ST7735_DrawCharS(x + stepSizeX + 6,  y + 2*stepSizeY, buttons[secondTerm],textColor, textBackgroundColor, size);
		bcdmin = (firstTerm << 4) | secondTerm;
	}
	else
		bcdmin = firstTerm;


	//Gets the input for the day
	ST7735_DrawString(x, y + 3*stepSizeY, "Day(1:M, 7:S): ", textColor, textBackgroundColor, size);
	firstTerm = KeyPad_getInput();
	ST7735_DrawCharS(x +3*stepSizeX, y + 3*stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);
	bcdday = firstTerm;

	//Gets the input for the day
	ST7735_DrawString(x, y + 4*stepSizeY, "Date: ", textColor, textBackgroundColor, size);
	firstTerm = KeyPad_getInput();
	ST7735_DrawCharS(x + stepSizeX, y + 4*stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);
	secondTerm = KeyPad_getInput();

	//if the user is entering a two digit number
	if(secondTerm != 11){
		ST7735_DrawCharS(x + stepSizeX + 6,  y + 4*stepSizeY, buttons[secondTerm],textColor, textBackgroundColor, size);
		bcddate = (firstTerm << 4) | secondTerm;
	}
	else
		bcddate = firstTerm;


	//Gets the input for the month
	ST7735_DrawString(x, y + 5*stepSizeY, "Month: ", textColor, textBackgroundColor, size);
	firstTerm = KeyPad_getInput();
	ST7735_DrawCharS(x + stepSizeX + 6, y + 5*stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);
	secondTerm = KeyPad_getInput();

	//if the user is entering a two digit number
	if(secondTerm != 11){
		ST7735_DrawCharS(x + stepSizeX + 12,  y + 5*stepSizeY, buttons[secondTerm],textColor, textBackgroundColor, size);
		bcdmonth = (firstTerm << 4) | secondTerm;
	}
	else
		bcdmonth = firstTerm;


	//Gets the input for the day
	ST7735_DrawString(x, y + 6*stepSizeY, "Year: ", textColor, textBackgroundColor, size);
	firstTerm = KeyPad_getInput();
	ST7735_DrawCharS(x + stepSizeX, y + 6*stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);
	secondTerm = KeyPad_getInput();

	//if the user is entering a two digit number
	if(secondTerm != 11){
		ST7735_DrawCharS(x + stepSizeX + 6,  y + 6*stepSizeY, buttons[secondTerm],textColor, textBackgroundColor, size);
		bcdyear = (firstTerm << 4) | secondTerm;
	}
	else
		bcdyear = firstTerm;

	//Gets the input for the day
	ST7735_DrawString(x, y + 7*stepSizeY, "AM/PM(0/1): ", textColor, textBackgroundColor, size);
	firstTerm = KeyPad_getInput();
	ST7735_DrawCharS(x + stepSizeX + 35, y + 7*stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);

	if(firstTerm == 0){
		isAM = true;
		isPM = false;
	}
	if(firstTerm == 1){
		isPM = true;
		isAM = false;
	}

	if(firstTerm != 1 && firstTerm != 0){
		ST7735_DrawString(x, y + 7*stepSizeY, "Wrong:       ", textColor, textBackgroundColor, size);
		Delay1ms(1000);
		ST7735_DrawString(x, y + 7*stepSizeY, "AM/PM(0/1): ", textColor, textBackgroundColor, size);
		firstTerm = KeyPad_getInput();
		ST7735_DrawCharS(x + stepSizeX, y + 7*stepSizeY, buttons[firstTerm],textColor, textBackgroundColor, size);
	}

	RTC_writeData();
	state = settings;
	MAP_WDT_A_clearTimer();

}

/******************************************************************************
 * Sets the users intital pin
 *****************************************************************************/
void LCD_setInitPin(void){

	uint8_t size = 1;
	uint16_t  x = 2, y = 5;
	char buttons[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	buttonPressed = 0;

	ST7735_FillScreen(backgroundColor);
	ST7735_DrawString(x, y, "Enter New Pin:", textColor, textBackgroundColor, size);
	y = 60;
	x = 20;
	size = 3;
	pin[0]=KeyPad_getInput();
	ST7735_DrawCharS(x, y, buttons[pin[0]], textColor, textBackgroundColor, size);
	pin[1]=KeyPad_getInput();
	ST7735_DrawCharS(x + 20, y, buttons[pin[1]], textColor, textBackgroundColor, size);
	pin[2]=KeyPad_getInput();
	ST7735_DrawCharS(x + 40, y, buttons[pin[2]], textColor, textBackgroundColor, size);
	pin[3]=KeyPad_getInput();
	ST7735_DrawCharS(x + 60, y, buttons[pin[3]], textColor, textBackgroundColor, size);

	Delay1ms(1000);

	didChangePin = true;

	WDT_A_holdTimer();
	FlashWrite();
	WDT_A_startTimer();

	ST7735_FillScreen(backgroundColor);

	state = idle;
}

/******************************************************************************
 * Changes the user pin
 *****************************************************************************/
void LCD_changePin(void){

	uint8_t size = 1;
	uint16_t  x = 2, y = 5;
	char buttons[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	state = changePin;

	buttonPressed = 0;

	ST7735_FillScreen(backgroundColor);
	ST7735_DrawString(x, y, "Enter Pin:", textColor, textBackgroundColor, size);

	y = 60;
	x = 20;
	size = 3;
	tempPin[0]=KeyPad_getInput();
	ST7735_DrawCharS(x, y, buttons[tempPin[0]], textColor, textBackgroundColor, size);
	tempPin[1]=KeyPad_getInput();
	ST7735_DrawCharS(x + 20, y, buttons[tempPin[1]], textColor, textBackgroundColor, size);
	tempPin[2]=KeyPad_getInput();
	ST7735_DrawCharS(x + 40, y, buttons[tempPin[2]], textColor, textBackgroundColor, size);
	tempPin[3]=KeyPad_getInput();
	ST7735_DrawCharS(x + 60, y, buttons[tempPin[3]], textColor, textBackgroundColor, size);
	Delay1ms(2000);

	ST7735_FillScreen(backgroundColor);

	if(KeyPad_checkForCorrectPin()){

		ST7735_FillScreen(backgroundColor);
		size = 1;
		x = 2;
		y = 5;
		ST7735_DrawString(x, y, "Enter New Pin:", textColor, textBackgroundColor, size);

		y = 60;
		x = 20;
		size = 3;
		pin[0]=KeyPad_getInput();
		ST7735_DrawCharS(x, y, buttons[pin[0]], textColor, textBackgroundColor, size);
		pin[1]=KeyPad_getInput();
		ST7735_DrawCharS(x + 20, y, buttons[pin[1]], textColor, textBackgroundColor, size);
		pin[2]=KeyPad_getInput();
		ST7735_DrawCharS(x + 40, y, buttons[pin[2]], textColor, textBackgroundColor, size);
		pin[3]=KeyPad_getInput();
		ST7735_DrawCharS(x + 60, y, buttons[pin[3]], textColor, textBackgroundColor, size);
		Delay1ms(2000);
		ST7735_FillScreen(backgroundColor);
		ST7735_DrawString(x, y, "PIN CHANGED", textColor, textBackgroundColor, 1);
		Delay1ms(1000);
		state = idle;
		WDT_A_holdTimer();
		FlashWrite();
		WDT_A_startTimer();
	}
	else{
		ST7735_DrawString(1, y, "WRONG PIN", textColor, textBackgroundColor, 2);
		Delay1ms(1000);
		state = settings;
	}

	ST7735_FillScreen(backgroundColor);
	MAP_WDT_A_clearTimer();
}


/******************************************************************************
 * Updates the screen to display the entered PIN
 *****************************************************************************/
void LCD_pin(void){

	uint8_t size = 1;
	uint16_t  x = 2, y = 5;
	char buttons[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	ST7735_FillScreen(backgroundColor);
	ST7735_FillScreen(backgroundColor);

	if(isArmed)
		ST7735_DrawString(x, y, "Enter Pin to Disarm:", textColor, textBackgroundColor, size);
	else
		ST7735_DrawString(x, y, "Enter Pin to Arm:", textColor, textBackgroundColor, size);

	y = 60;
	x = 20;
	size = 3;
	tempPin[0]=KeyPad_getInput();
	ST7735_DrawCharS(x, y, buttons[tempPin[0]], textColor, textBackgroundColor, size);
	tempPin[1]=KeyPad_getInput();
	ST7735_DrawCharS(x + 20, y, buttons[tempPin[1]], textColor, textBackgroundColor, size);
	tempPin[2]=KeyPad_getInput();
	ST7735_DrawCharS(x + 40, y, buttons[tempPin[2]], textColor, textBackgroundColor, size);
	tempPin[3]=KeyPad_getInput();
	ST7735_DrawCharS(x + 60, y, buttons[tempPin[3]], textColor, textBackgroundColor, size);
	Delay1ms(2000);

	ST7735_FillScreen(backgroundColor);

	if(KeyPad_checkForCorrectPin() && !isArmed){
		ST7735_DrawString(x, y, "System ARMED", textColor, textBackgroundColor, 1);
		Delay1ms(1000);
		state = armed;
		whatDetected = 5;
	} else if(KeyPad_checkForCorrectPin() && isArmed){
		ST7735_DrawString(x, y, "System DISARMED", textColor, textBackgroundColor, 1);
		Delay1ms(750);
		state = disarmed;
		whatDetected = 6;
	}
	else{
		ST7735_DrawString(1, y, "WRONG PIN", textColor, textBackgroundColor, 2);
		Delay1ms(1000);
		state = settings;
	}

	MAP_WDT_A_clearTimer();
	ST7735_FillScreen(backgroundColor);
	ST7735_FillScreen(backgroundColor);
}

/*******************************************************************************
 * Displays the splash screen/startup screen
 ******************************************************************************/
void LCD_startup(void){

	//displays the bitmap, starting at the bottom left corner of the screen
	ST7735_DrawBitmap(15, 130, GV_Logo, 100, 113);

	//delay loop to display bitmap
	Delay1ms(1000);

	//clears the screen
	ST7735_FillScreen(ST7735_WHITE);

	//displays the project and who it is by
	ST7735_DrawString(5, 30, "EGR 326 Project By:", textColor, textBackgroundColor, 1);
	ST7735_DrawString(5, 45, "Brandon Baars &", textColor, textBackgroundColor, 1);
	ST7735_DrawString(5, 60, "Adam Ferner", textColor, textBackgroundColor, 1);

	//delay loop to display acknowledgement for 3 seconds
	Delay1ms(3000);

	//displays the project and who it is by
	ST7735_FillRect(3, 25, 120, 45, backgroundColor);
}



