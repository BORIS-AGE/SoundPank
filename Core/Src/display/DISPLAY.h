/*
 * DISPLAY.h
 *
 *  Created on: Mar 22, 2026
 *      Author: Shrek
 */


#ifndef SRC_DISPLAY_DISPLAY_H_
#define SRC_DISPLAY_DISPLAY_H_

void displayInit();
//void displaySendData(uint8_t* data, uint16_t size);
void displayReset();

void displayImage(uint8_t* array, uint16_t size, uint8_t isContinue);

void displayPartOfImage(uint16_t y, uint8_t* array, uint16_t size, uint16_t partHeight);


#endif /* SRC_DISPLAY_DISPLAY_H_ */

