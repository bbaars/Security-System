/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "RTC.h"
#include "Variables.h"

void RTC_Init(void);

/******************************************************************************
 * Initializes the RTC for use with I2C
 ******************************************************************************/
void RTC_Init(void){

	const eUSCI_I2C_MasterConfig i2cConfig = {

			EUSCI_B_I2C_CLOCKSOURCE_SMCLK,		//SMCLK as CLock Source 12Mhz (set from LCD Init)
			CS_CLOCK_DIVIDER_4,					//SMCLK down to 3MHz
			EUSCI_B_I2C_SET_DATA_RATE_400KBPS,	// Desired I2C Clock = 400kbs
			0,									//no byte counter threshold
			EUSCI_B_I2C_NO_AUTO_STOP			//no autostop
	};

	/* setting P6.5/P6.4 to use for I2C */
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
			GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

	P6REN |= BIT4 | BIT5;
	P6OUT |= BIT4 | BIT5;

	/* Initializes the master */
	MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);

	/* sets the slave address */
	MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, RTC_ADDRESS);

	/* sets the mode to transmit */
	MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

	/* enables I2C to start operations */
	MAP_I2C_enableModule(EUSCI_B1_BASE);

}

void RTC_readData(void){

	RTC_Init();

	/* set master to transmit mode */
	MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

	/* wait for bus to release */
	while(MAP_I2C_isBusBusy(EUSCI_B1_BASE));

	/* set pointer to beginning of registers */
	MAP_I2C_masterSendSingleByte(EUSCI_B1_BASE, 0);

	/* wait for bus to release */
	while(MAP_I2C_isBusBusy(EUSCI_B1_BASE));

	MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);

	/* wait for bus to release */
	while(MAP_I2C_isBusBusy(EUSCI_B1_BASE));

	RTC_registers[0] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[1] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[2] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[3] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[4] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[5] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[6] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[7] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[8] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[9] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[10] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[11] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[12] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[13] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[14] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[15] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[16] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[17] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
	RTC_registers[18] = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);

	sec = (int)RTC_registers[0];
	min = (int)RTC_registers[1];
	hour = (int)RTC_registers[2] & 0x1F; //since 12 hour, only want the first 5 bits
	AMPM = (int)RTC_registers[2] & 0x20; //want to mask wether or not it's AM or PM
	day = (int)RTC_registers[3];
	date = (int)RTC_registers[4] & 0x3F; //want to mask just the data
	month = (int)RTC_registers[5];
	year = (int)RTC_registers[6];
	tempL = (int)RTC_registers[17]; //want to mask the data MSB is Sign of Temperature
	tempH = (int)(RTC_registers[18] >> 6);
}

/******************************************************************************
 * This writes the given data to the given register
 *****************************************************************************/
void RTC_writeData(void){

	/* set master to transmit mode */
	MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

	/* wait for bus to release */
	while(MAP_I2C_isBusBusy(EUSCI_B1_BASE));

	/* set pointer to beginning of registers */
	MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE, 0);
	MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, sec);
	MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdmin);

	//if AM is choosen, writes a 0 to AM register and a 1 to 12 hour register
	if(isAM)
		MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdhour | 0x40);

	//if PM is choosen,
	if(isPM)
		MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdhour | 0x60);
	MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdday);
	MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcddate);
	MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdmonth);
	MAP_I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, bcdyear);
}


