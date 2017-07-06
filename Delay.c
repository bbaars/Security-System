/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "PWM.h"
#include "Variables.h"
#include "ST7735.h"
#include "Delay.h"

uint16_t delayTime, tempBackGroundColor;

uint8_t counter = 0, dx = 1, c = 0;

/*******************************************************************************
 * Initializes the Timer32 module to run off MCLK
 ********************************************************************************/
void Timer32_Init(void){


	MAP_Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
	            TIMER32_FREE_RUN_MODE);

	//inits the INT-T32_INT1 interrupt
	MAP_Interrupt_enableInterrupt(INT_T32_INT1);
}

/*******************************************************************************
 * Sets the delay for the 32 bit timer
 ******************************************************************************/
void Timer32_Delay(uint32_t delay){

	//initializes the timer and the count
	MAP_Timer32_setCount(TIMER32_BASE, 48000000);
	MAP_Timer32_enableInterrupt(TIMER32_BASE);
	MAP_Timer32_startTimer(TIMER32_BASE, true);
	delayTime = delay;
}



/******************************************************************************
 * Interrupt handler for the Timer32
 *****************************************************************************/
void T32_INT1_IRQHandler(void){

	//recalls the set count and start timer (seemed to be a one-shot timer)
	MAP_Timer32_clearInterruptFlag(TIMER32_BASE);
	MAP_Timer32_setCount(TIMER32_BASE, 48000000);
	MAP_Timer32_startTimer(TIMER32_BASE, true);

	P1->OUT ^= BIT0;

	counter++;

	if(counter == 10){
		counter = 0;
	}

	if(counter % 2 == 0){
		if(isArmed && hasDetected){
			P2->OUT ^= BIT0;
			PWM_setDutyCycleBuzzer(15);
			ST7735_InvertDisplay(0);
		}
	}
	else{
		if(isArmed && hasDetected){
			P2->OUT ^= BIT0;
			PWM_setDutyCycleBuzzer(85);
			ST7735_InvertDisplay(1);
		}
	}

	if(isDoorOpened && counter == 4)
		isDoorOpened = false;

	if(isWindowOpened && counter == 4)
		isWindowOpened = false;

	if(isMotionDetected && counter == 4)
		isMotionDetected = false;
}


/******************************************************************************
 * This function uses the Systick timer to create a delay 2^16/48Mhz (1ms)
 *****************************************************************************/
void SysTick_Delay(void){

	//delayTime = delay;

	/* Initializes SysTick Timer with Interrupts */
	SysTick -> CTRL = 0;					//disable systick timer during setup
	SysTick -> LOAD =((100*32768)-1);		//set the value to the requested delay
	SysTick -> VAL = 0;						//any write to the val clears it
	SysTick -> CTRL = 0x00000007;			//enable SysTick with Interrupts
}

/********************************************************************************
 * Interrupt handler for the Systick Timer
 ********************************************************************************/
void SysTick_Handler(void){

	MAP_ADC14_toggleConversionTrigger();

	//fire alarm turns on buzzer and flashes LED
	if(tempF >= 110){
		isArmed = true;
		hasDetected = true;
		whatDetected = 4;
		state = detection;
		//LCD_updateSecurityStatus(4);
	}

	if(isAnimations){
		shiftText = shiftText + dx;

		if((shiftText + 14) >= 70 || shiftText <= 0)
			dx = -1*dx;
	}

}
