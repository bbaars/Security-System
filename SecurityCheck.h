/*
 * SecurityCheck.h
 *
 *  Created on: Oct 28, 2016
 *      Author: Brandon
 */

#ifndef SECURITYCHECK_H_
#define SECURITYCHECK_H_

/*****************************************************************************
 * Initializes the security sensors and buttons
 *****************************************************************************/
void securityCheck_Init(void);

/*******************************************************************************
 * Sets the lock status to either unlocked or locked
 ******************************************************************************/
void setLockStatus(bool lockStatus);

/******************************************************************************
 * returns the status of the system, either locked or unlocked
 *****************************************************************************/
bool getLockStatus(void);

#endif /* SECURITYCHECK_H_ */
