/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "Motor.h"
#include "ST7735.h"

uint16_t i = 0, j = 0, steps = 128, delay = 2;

/**********************************************************************
 *  Steps the motor 11.25 deg, 32 steps for a full 360deg rotation
 *  It sets each bit to high, steps it to low, sets the next to high and
 *  repeats this until 360 deg rotation occurs.
 *
 *  64 (ratio) x 8 (teeth) = 512 steps
 *********************************************************************/
void Motor_Lock(void){

	for (j = 0 ; j < steps ; j++)
	{
		for (i = 0 ; i < 4 ; i++){

			P8 -> OUT &=~ 0xF0;
			P8 -> OUT |= 0x10 << i;
			Delay1ms(delay);
		}
	}
}

/**********************************************************************
 *  Steps the motor 11.25 deg, 32 steps for a full 360deg rotation
 *  It sets each bit to high, steps it to low, sets the next to high and
 *  repeats this until 360 deg rotation occurs. 64:1 gear ratio,
 *
 *  64 (ratio) x 8 (teeth) = 512 steps
 *********************************************************************/
void Motor_Unlock(void){

	for (j = 0 ; j < steps ; j++){

		for (i = 0 ; i < 4 ; i++){

			P8 -> OUT &=~ 0xF0;
			P8 -> OUT |= 0x80 >> i;
			Delay1ms(delay);
		}
	}
}
/**********************************************************************
 *  Initializes the Pins to be Output
 *********************************************************************/
void Motor_Init(void){

	/* Sets the pins to output */
	P8 -> DIR |= BIT4 | BIT5 | BIT6 | BIT7;
}
