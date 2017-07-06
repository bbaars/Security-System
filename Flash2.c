#include "driverlib.h"
#include "ST7735.h"
#include "Variables.h"
#include "Flash2.h"
#include "RTC.h"
#include "Keypad.h"
#include "flash.h"

#define CALIBRATION_START 0x000200000 // Start address for flash.
#define MARGIN_TOP 30 // Text margin top.
#define MARGIN_LEFT 5 // Text margin left.

/* Date Information array that will be stored in flash. */
uint8_t AlarmInformation[11][7] = {
		{0,0,0,0,0,0,0}, //alarm
		{0,0,0,0,0,0,0}, //alarm
		{0,0,0,0,0,0,0}, //alarm
		{0,0,0,0,0,0,0}, //alarm
		{0,0,0,0,0,0,0}, //alarm
		{0,0,0,0,0,0,0}, //arm & disarm
		{0,0,0,0,0,0,0}, //arm & disarm
		{0,0,0,0,0,0,0}, //arm & disarm
		{0,0,0,0,0,0,0}, //arm & disarm
		{0,0,0,0,0,0,0}, //arm & disarm
		{0,0,0,0,0,0,0}  //customization
};

/*****************************************************************************
 * Function call from outside .c file to read the values stored in flash
 ******************************************************************************/
void FlashRead(void){

	//reads the data from flash into the date information array
	ReadFromFlash(&AlarmInformation);

	//loads the last 5 times onto the screen
	LoadTimes();

}

/*******************************************************************************
 * Function call from outside .c file to write the date and time to flash
 *****************************************************************************/
void FlashWrite(void){

	uint8_t i = 0;

	//read from flash to store date and times into array
	ReadFromFlash(&AlarmInformation);

	//pushes the dates and times up to account for recent 5 alarm trips
	PushDateTimeUp(&AlarmInformation);

	//updates the first part to background Color
	if(backgroundColor == ST7735_BLUE)
		AlarmInformation[10][0] = 1;
	else if(backgroundColor == ST7735_GREEN)
		AlarmInformation[10][0] = 2;
	else if(backgroundColor == ST7735_BLACK)
		AlarmInformation[10][0] = 3;
	else if(backgroundColor == ST7735_WHITE)
		AlarmInformation[10][0] = 4;

	//updates the second part to text Color
	if(textColor == ST7735_BLACK)
		AlarmInformation[10][1] = 1;
	else if(textColor == ST7735_WHITE)
		AlarmInformation[10][1] = 2;
	else if(textColor == ST7735_RED)
		AlarmInformation[10][1] = 3;
	else if(textColor == ST7735_YELLOW)
		AlarmInformation[10][1] = 4;

	//updates the third part to animations & alarm status
	if(isAnimations && isArmed)
		AlarmInformation[10][2] = 1;
	else if(isAnimations && !isArmed)
		AlarmInformation[10][2] = 2;
	else if(!isAnimations && isArmed)
		AlarmInformation[10][2] = 3;
	else if(!isAnimations && !isArmed)
		AlarmInformation[10][2] = 4;

	if(!isAnimations && isArmed && isLocked)
		AlarmInformation[10][2] = 5;
	else if(isAnimations && isArmed && isLocked)
		AlarmInformation[10][2] = 6;
	else if(isAnimations && !isArmed && isLocked)
		AlarmInformation[10][2] = 7;
	else if(!isAnimations && !isArmed && isLocked)
		AlarmInformation[10][2] = 8;

	//stores the users pin into flash memory
	for(i = 0 ; i < 4 ; i++){
		AlarmInformation[10][3 + i] = pin[i];
	}

	//saves the date and time
	SaveToFlash(&AlarmInformation);
}

/******************************************************************************
 * This function updates the users settings once the watchdog or power has
 * caused the system to restart. This will restore settings
 *
 * Call after you have read from flash
 *
 * Returns true if there is a PIN saved
 *******************************************************************************/
bool checkPin(void){

	uint8_t i = 0, count = 0, count2 =0;

		//checks if the pin saved is 0000 (could potentially be dangerous)
		for( i = 0 ; i < 4 ; i++){
			if(AlarmInformation[10][3+i] == 255)
				count++;
		}
		for( i = 0 ; i < 4 ; i++){
			if(AlarmInformation[10][3+i] == 0)
				count2++;
		}

		//if the pin saved is 0000 then no pin is saved
		if(count == 4 || count2 == 4)
			return false;

		//otherwise store the user pin
		else{
			for( i = 0 ; i < 4 ; i++)
				pin[i] = AlarmInformation[10][3+i];
			return true;
		}

}

void updateUserSettings(void){

//updates the first part to background Color
	if(AlarmInformation[10][0] == 1)
		backgroundColor = ST7735_BLUE;
	else if(AlarmInformation[10][0] == 2)
		backgroundColor = ST7735_GREEN;
	else if(AlarmInformation[10][0] == 3)
		backgroundColor = ST7735_BLACK;
	else if(AlarmInformation[10][0] == 4)
		backgroundColor = ST7735_WHITE;

	//updates the second part to text Color
	if(AlarmInformation[10][1] == 1)
		textColor = ST7735_BLACK;
	else if(AlarmInformation[10][1] == 2)
		textColor = ST7735_WHITE;
	else if(AlarmInformation[10][1] == 3)
		textColor = ST7735_RED;
	else if(AlarmInformation[10][1] == 4)
		textColor = ST7735_YELLOW;

	//updates the third part to animations & alarm status
	if(AlarmInformation[10][2] == 1){
		isAnimations = true;
		isArmed = true;
	}
	else if(AlarmInformation[10][2] == 2){
		isAnimations = true;
		isArmed = false;
	}
	else if(AlarmInformation[10][2] == 3){
		isAnimations = false;
		isArmed = true;
	}
	else if(AlarmInformation[10][2] == 4){
		isAnimations = false;
		isArmed = false;
	}
	else if(AlarmInformation[10][2] == 5){
			isAnimations = false;
			isArmed = true;
			isLocked = true;
	}
	else if(AlarmInformation[10][2] == 6){
			isAnimations = true;
			isArmed = true;
			isLocked = true;
	}
	else if(AlarmInformation[10][2] == 7){
		isAnimations = true;
		isArmed = false;
		isLocked = true;
	}
	else if(AlarmInformation[10][2] == 8){
			isAnimations = false;
			isArmed = false;
			isLocked = true;
		}

	ST7735_FillScreen(backgroundColor);
	textBackgroundColor = backgroundColor;
}

/******************************************************************************
 * Loads the flash information and prints it onto the screen
 ******************************************************************************/
void LoadTimes(){

	uint8_t i = 0;

	for (i = 0; i < 10; i++) {

		if(AlarmInformation[i][2] != 0x00){

			LCDIntOut(AlarmInformation[i][0], 5 + MARGIN_LEFT, 10 * i + MARGIN_TOP);
			ST7735_DrawCharS(15 + MARGIN_LEFT, 10 * i + MARGIN_TOP, '/',textColor, textBackgroundColor, 1);
			LCDIntOut(AlarmInformation[i][1], 20 + MARGIN_LEFT, 10 * i + MARGIN_TOP);
			ST7735_DrawCharS(30 + MARGIN_LEFT, 10 * i + MARGIN_TOP, '/', textColor, textBackgroundColor, 1);
			LCDIntOut(AlarmInformation[i][2], 35 + MARGIN_LEFT, 10 * i + MARGIN_TOP);

			LCDIntOut(AlarmInformation[i][3], 50 + MARGIN_LEFT, 10 * i + MARGIN_TOP);
			ST7735_DrawCharS(60 + MARGIN_LEFT, 10 * i + MARGIN_TOP, ':', textColor, textBackgroundColor, 1);
			LCDIntOut(AlarmInformation[i][4], 65 + MARGIN_LEFT, 10 * i + MARGIN_TOP);
			ST7735_DrawCharS(75 + MARGIN_LEFT, 10 * i + MARGIN_TOP, ':', textColor, textBackgroundColor, 1);
			LCDIntOut(AlarmInformation[i][5], 80 + MARGIN_LEFT, 10 * i + MARGIN_TOP);

			if(AlarmInformation[i][6] == 1)
				ST7735_DrawString(95 + MARGIN_LEFT, 10 * i + MARGIN_TOP,"WIND", textColor, textBackgroundColor, 1);
			else if(AlarmInformation[i][6] == 2)
				ST7735_DrawString(95 + MARGIN_LEFT, 10 * i + MARGIN_TOP,"DOOR", textColor, textBackgroundColor, 1);
			else if(AlarmInformation[i][6] == 3)
				ST7735_DrawString(95 + MARGIN_LEFT, 10 * i + MARGIN_TOP,"MOT", textColor, textBackgroundColor, 1);
			else if(AlarmInformation[i][6] == 4)
				ST7735_DrawString(95 + MARGIN_LEFT, 10 * i + MARGIN_TOP,"FIRE", textColor, textBackgroundColor, 1);
			else if(AlarmInformation[i][6] == 5)
				ST7735_DrawString(95 + MARGIN_LEFT, 10 * i + MARGIN_TOP,"ARM", textColor, textBackgroundColor, 1);
			else if(AlarmInformation[i][6] == 6)
				ST7735_DrawString(95 + MARGIN_LEFT, 10 * i + MARGIN_TOP,"DIS", textColor, textBackgroundColor, 1);
		}
	}
}



/********************************************************************************
 * LCD Integer Out easily converts a BCD to friendly integer and then prints
 * it out to the LCD.
 * @param data The BCD value.
 * @param x X-Coordinate to display.
 * @param y Y-Coordinate to display.
 ********************************************************************************/
void LCDIntOut(int data, int x, int y){
	ST7735_DrawCharS(x, y, (((data & 0xF0) >> 4) + 48), textColor, textBackgroundColor, 1);
	ST7735_DrawCharS(x+5, y, ((data & 0x0F) + 48), textColor, textBackgroundColor, 1);
}

/********************************************************************************
 * Push DateTime Up will push older dates up in the date information array.
 * This will allow for older dates to drop off and new dates to be added to
 * the array easily.
 * @param Date Array containing all five dates.
 *******************************************************************************/
void PushDateTimeUp(uint8_t * Date) {
	int i, x;
	for (x = 0; x < 7; x++)
		for (i = 70; i > 0; i--) {
			*(Date + i) = *(Date + (i - 1));
		}

	*(Date) = month;
	*(Date + 1) = date;
	*(Date + 2) = year;
	*(Date + 3) = hour;
	*(Date + 4) = min;
	*(Date + 5) = sec;
	*(Date + 6) = whatDetected;
}

void flashInitRead(void){
	ReadFromFlash(&AlarmInformation);
}

/** ****************************************************************************
 * Reads the date and time information from flash and stores it in the
 * date information array.
 * @param Date Date information array.
 ******************************************************************************/
void ReadFromFlash(uint8_t * Date) {
	// point to address in flash
	uint8_t* addr_pointer = CALIBRATION_START + 1;
	int i;

	for(i = 0; i < 77; i++) // read values in flash before programming
		*(Date + i) = *addr_pointer++;
}


/*******************************************************************************
 * Save to flash allows the date information to be saved to flash so that
 * when the device is powered down and then back up the date information will
 * be saved.
 * @param Date Pointer to Date information array to save.
 ******************************************************************************/
void SaveToFlash(uint8_t * Date){
	/* Setting MCLK to 48MHz for faster programming */
	MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);

	FlashCtl_setWaitState(FLASH_BANK0, 2);
	FlashCtl_setWaitState(FLASH_BANK1, 2);

	/* Unprotecting Info Bank 0, Sector 0 */
	FlashCtl_unprotectSector(FLASH_INFO_MEMORY_SPACE_BANK0, FLASH_SECTOR0);

	/* Erase the flash sector starting CALIBRATION_START. */
	while(!FlashCtl_eraseSector(CALIBRATION_START));

	/* Program the flash with the new data. */
	while (!FlashCtl_programMemory(AlarmInformation, (void*) CALIBRATION_START + 1, 78)); // Note: leave first byte unprogrammed

	/* Setting the sector back to protected */
	FlashCtl_protectSector(FLASH_INFO_MEMORY_SPACE_BANK0, FLASH_SECTOR0);
}
