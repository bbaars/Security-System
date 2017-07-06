/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "Variables.h"

float dCycle, voltage;
int result;

/******************************************************************************
 * This function intitializes the ADC
 ******************************************************************************/
void PHOTOCELL_adc14Init(void){

	/* Sets P5.6 to be the PWM output pin */
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5,
			GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);

	/* initializing ADC for MCLK */
	MAP_ADC14_enableModule();

	//takes 48Mhz down to 3Mhz
	MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_4,
			ADC_DIVIDER_3, 0);

	/* configure ADC Memory to MEM0 */
	MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
	MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
			ADC_INPUT_A0, false);

	/* setting GPIO pin as analog inputs (P5.5 A0) */
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5,
			GPIO_TERTIARY_MODULE_FUNCTION);

	/*Enabling interrupts*/
	MAP_Interrupt_enableInterrupt(INT_ADC14);
	MAP_ADC14_enableInterrupt(ADC_INT0);

	/* Enables the conversion of the ADC Data */
	MAP_ADC14_enableConversion();

	/* Toggles Conversion Trigger */
	MAP_ADC14_toggleConversionTrigger();
}

/******************************************************************************
 * function that when the ADC Conversion interrupt is called,
 * finds the PHOTOCELL Brightness
 ******************************************************************************/
void ADC14_IRQHandler(void){

	/* object used to configure the Timer for PWM */
	Timer_A_PWMConfig pwmConfig = {TIMER_A_CLOCKSOURCE_SMCLK,
			TIMER_A_CLOCKSOURCE_DIVIDER_1,3200,
			TIMER_A_CAPTURECOMPARE_REGISTER_1,
			TIMER_A_OUTPUTMODE_RESET_SET,3200};

	uint16_t status = MAP_ADC14_getEnabledInterruptStatus();

	MAP_ADC14_clearInterruptFlag(status);

	if(ADC_INT0 & status){

		//toggles the conversion trigger
		MAP_ADC14_toggleConversionTrigger();

		//receives the result from that specified memory location
		result = MAP_ADC14_getResult(ADC_MEM0);
		voltage = result*(3.3/16384);

		//sets the duty cycle to be out of 100%
		dCycle = 100*voltage;

		//generates PWM on TA2.1
		pwmConfig.dutyCycle = 4200-dCycle * 32;
		MAP_Timer_A_generatePWM(TIMER_A2_BASE, &pwmConfig);
	}
}
