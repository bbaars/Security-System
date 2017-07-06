/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
 * Initialzes the clock source for the watch dog to be ACLK with a period
 * of 128Khz. The Timeout of the watchdog is 8192K clock cycles, or 1.04 min.
 ******************************************************************************/
void Watchdog_Init(void){
	/* Setting MCLK to REFO at 128Khz for LF mode and SMCLK to REFO */
	MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
	MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

	/* Configuring WDT to timeout after 512k iterations of ACLK, at 128k,
	 * this will roughly equal 4 seconds*/
	MAP_SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
	MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK,
			WDT_A_CLOCKITERATIONS_8192K);

	/* Starts the watchdog Timer */
	MAP_WDT_A_startTimer();
}
