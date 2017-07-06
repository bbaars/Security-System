/*
 * Flash2.h
 *
 *  Created on: Dec 3, 2016
 *      Author: Brandon
 */

#ifndef FLASH2_H_
#define FLASH2_H_

void LoadTimes();
void PushDateTimeUp(uint8_t * Date);
void ReadFromFlash(uint8_t * Date);
void LCDIntOut(int data, int x, int y);
/**
 * Save to flash allows the date information to be saved to flash so that
 * when the device is powered down and then back up the date information will
 * be saved.
 * @param Date Date information array to save.
 */
void SaveToFlash(uint8_t * Date);
void ReadAndWrite(void);
/*******************************************************************************
 * Function call from outside .c file to write the date and time to flash
 *****************************************************************************/
void FlashWrite(void);

/*****************************************************************************
 * Function call from outside .c file to read the values stored in flash
 ******************************************************************************/
void FlashRead(void);


/******************************************************************************
 * This function updates the users settings once the watchdog or power has
 * caused the system to restart. This will restore settings
 *
 * Call after you have read from flash
 *******************************************************************************/
extern void updateUserSettings(void);

void flashInitRead(void);

/******************************************************************************
 * This function updates the users settings once the watchdog or power has
 * caused the system to restart. This will restore settings
 *
 * Call after you have read from flash
 *
 * Returns true if there is a PIN saved
 *******************************************************************************/
bool checkPin(void);

#endif /* FLASH2_H_ */
