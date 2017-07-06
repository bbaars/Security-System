#include "driverlib.h"
#include "stdio.h"
#include "PWM.h"
#include "Variables.h"

/*****************************************************************************
 * Initializes the parameters for the PWM
 ***************************************************************************/
void PWM_init(void){

    /* Sets Buzzer (P2.4) to output */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,
    		GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
}

/*****************************************************************************
 * This function sets the duty cycle of the PWM for the buzzer
******************************************************************************/
void PWM_setDutyCycleBuzzer(uint8_t dcycle){

	/* object used to configure the Timer for PWM */
	Timer_A_PWMConfig pwmConfig = {TIMER_A_CLOCKSOURCE_SMCLK,
			TIMER_A_CLOCKSOURCE_DIVIDER_1,3200,
			TIMER_A_CAPTURECOMPARE_REGISTER_1,
			TIMER_A_OUTPUTMODE_RESET_SET,3200};

	/* sets the duty cycle from the object created */
		pwmConfig.dutyCycle = dcycle * 32;

		/* generates the PWM on the timer */
		MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

}
