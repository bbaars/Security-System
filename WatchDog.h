/*
 * WatchDog.h
 *
 *  Created on: Dec 2, 2016
 *      Author: Brandon
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_


/******************************************************************************
 * Initialzes the clock source for the watch dog to be ACLK with a period
 * of 128Khz. The Timeout of the watchdog is 8192K clock cycles, or 1.04 min.
 ******************************************************************************/
void Watchdog_Init(void);

#endif /* WATCHDOG_H_ */
