/******************************************************************************
 * Security System
 * Author: Brandon Baars
 *******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "RTC.h"
#include "LCD.h"
#include "SecurityCheck.h"
#include "Photocell.h"
#include "Keypad.h"
#include "Variables.h"
#include "Motor.h"
#include "Flash2.h"
#include "PWM.h"
#include "Delay.h"
#include "ST7735.h"
#include "WatchDog.h"


int main(void) {

	/* Stop Watchdog  */
	MAP_WDT_A_holdTimer();
	LCD_ClockInit48Mhz();
	RTC_Init();
	securityCheck_Init();
	Motor_Init();
	KeyPad_Init();
	PWM_init();
	PHOTOCELL_adc14Init();
	Timer32_Init();

	//sets a 1second delay
	Timer32_Delay(1000);
	SysTick_Delay();

	//Initializes the LED's
	P2->DIR |= BIT0;
	P1->DIR |= BIT0;
	P2->DIR |= BIT1;

	LCD_startup();

	//sets the inital state to idle
	state = idle;

	//sets default armed status to disarmed
	isArmed = false;

	//turns the LEDS low
	P2->OUT &=~ BIT0;
	P2->OUT &=~ BIT1;

	//have the user set their initial Pin if not stored in memory
	flashInitRead();
	isDoorOpened = false;

	if(!checkPin())
		LCD_setInitPin();
	else
		updateUserSettings();

	Watchdog_Init();

	//LCD_setStatus();

	while(1)
	{

		switch(state){

		//while the system is in idle state
		case idle:
			//constantly update data
			LCD_updateData();

			MAP_WDT_A_clearTimer();

			//if the system is not armed, turn on the green LED
			if(!isArmed)
				P2->OUT |= BIT1;
			if(isArmed)
				P2->OUT |= BIT0;

			//if the screen ends on inverted, set back to normal
			if(getInversion() && !hasDetected)
				ST7735_InvertDisplay(0);

			if(isDoorOpened)
					ST7735_DrawString(2, 80, "Door Opened", textColor, textBackgroundColor, 1);
			if(!isDoorOpened)
				ST7735_DrawString(2, 80, "                ", textColor, textBackgroundColor, 1);

			if(isWindowOpened)
				ST7735_DrawString(2, 95, "Window Opened", textColor, textBackgroundColor, 1);
			if(!isWindowOpened)
				ST7735_DrawString(2, 95, "               ", textColor, textBackgroundColor, 1);

			if(isMotionDetected)
				ST7735_DrawString(2, 110, "Motion Detected", textColor, textBackgroundColor, 1);
			if(!isMotionDetected)
				ST7735_DrawString(2, 110, "               ", textColor, textBackgroundColor, 1);
			break;

		case settings:

			//show the menu on the LCD
			LCD_showMenu();

			//wait for user input
			buttonPressed = KeyPad_getInput();

			//calls proper function based on user input
			if(buttonPressed == 1)
				LCD_displayHome();
			else if(buttonPressed == 2)
				LCD_pin();
			else if(buttonPressed == 3)
				LCD_motor();
			else if(buttonPressed == 4)
				LCD_showAlarm();
			else if(buttonPressed == 5)
				LCD_changeDateTime();
			else if(buttonPressed == 6)
				LCD_changePin();
			else if(buttonPressed == 7)
				LCD_customize();
			else if(buttonPressed == 14)
				state = idle;

			buttonPressed = 0;

			//if watchdog timer times out, reset system

			/************** UPDATE ****************/

			break;

		case armed:

			//sets the status of armed to true
			isArmed = true;

			//turns off the green led and turns on the red LED
			P2->OUT &=~ BIT1;
			P2->OUT |= BIT0;

			WDT_A_holdTimer();
			FlashWrite();
			WDT_A_startTimer();

			state = idle;

			break;

		case disarmed:

			//sets both armed and detcted to false
			isArmed = false;
			hasDetected = false;

			Delay1ms(10);

			//turns off the Red LED and green one on
			P2->OUT |= BIT1;
			P2->OUT &=~ BIT0;


			//sets the duty cycle of the alarm to 0 (turns off)
			PWM_setDutyCycleBuzzer(0);

			WDT_A_holdTimer();
			FlashWrite();
			WDT_A_startTimer();

			state = idle;

			break;

		case changePin:

			//sets the variable to show the user has updated pin
			didChangePin = true;

			break;

		case changeDate:

			//empty to contain user in menu
			break;

		case alarms:

			//empty to contain user in menu
			break;

		case customize:

			//empty to contain user in menu
			break;

			/********************************************************************
			 * If the system is armed and it detects something, writes the flash
			 *******************************************************************/
		case detection:

			//halt watchdog while attempting to write to flash memory
			MAP_WDT_A_holdTimer();
			FlashWrite();
			MAP_WDT_A_startTimer();

			state = idle;

			break;
		}
	}
}


//call systick delay with delay for IRQ It will be different with higher clock source
//call init function for everything
