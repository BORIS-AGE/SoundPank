/*
 * DISPLAY.c
 *
 *  Created on: Mar 22, 2026
 *      Author: Shrek
 */

#include "main.h"
#include "SCREEN_CONFIG.h"
extern SPI_HandleTypeDef hspiX;

const uint16_t tempArray[76800];
const uint16_t array2[76800];

void displaySetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void displaySendData(uint8_t* data, uint16_t size);
void displaySendCommand(uint8_t cmd);

void displayInit() {
	HAL_GPIO_WritePin(SCREEN_CS_GPIO_Port, SCREEN_CS_Pin, GPIO_PIN_RESET);

	displayReset();

	displaySendCommand(0x01); // Software reset
    HAL_Delay(100);

    displaySendCommand(0x11); // Sleep out
    HAL_Delay(120);


    // Pixel format = RGB565
    uint8_t data = 0x55;
    displaySendCommand(0x3A);
    displaySendData(&data, 1);

    // Memory Access Control (orientation) set to horizontal
    data = 0b01001000;
    displaySendCommand(0x36);
    displaySendData(&data, 1);

    displaySendCommand(0x29); // Display ON

    displaySetWindow(0, 0, 239, 319);

    // display image

	HAL_GPIO_WritePin(SCREEN_CS_GPIO_Port, SCREEN_CS_Pin, GPIO_PIN_SET);
}

void displayImage(uint8_t* array, uint16_t size, uint8_t isContinue) {
	HAL_GPIO_WritePin(SCREEN_CS_GPIO_Port, SCREEN_CS_Pin, GPIO_PIN_RESET);

	if(isContinue) {
	    displaySendCommand(0x3C);
	} else {
	    displaySendCommand(0x2C);
	}

    displaySendData(array, size);

	HAL_GPIO_WritePin(SCREEN_CS_GPIO_Port, SCREEN_CS_Pin, GPIO_PIN_SET);

}

void displayPartOfImage(uint16_t y, uint8_t* array, uint16_t size, uint16_t partHeight) {
	HAL_GPIO_WritePin(SCREEN_CS_GPIO_Port, SCREEN_CS_Pin, GPIO_PIN_RESET);
	displaySetWindow(0, y, 239, y + partHeight - 1);

    displaySendCommand(0x2C);

    displaySendData(array, size);

	HAL_GPIO_WritePin(SCREEN_CS_GPIO_Port, SCREEN_CS_Pin, GPIO_PIN_SET);
}


void displaySetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    uint8_t data[4];

    // X
    displaySendCommand(0x2A);
    data[0] = x1 >> 8;
    data[1] = x1 & 0xFF;
    data[2] = x2 >> 8;
    data[3] = x2 & 0xFF;
    displaySendData(data, 4);

    // Y
    displaySendCommand(0x2B);
    data[0] = y1 >> 8;
    data[1] = y1 & 0xFF;
    data[2] = y2 >> 8;
    data[3] = y2 & 0xFF;
    displaySendData(data, 4);
}

void displayReset() {
    HAL_GPIO_WritePin(SCREEN_RESET_GPIO_Port, SCREEN_RESET_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(SCREEN_RESET_GPIO_Port, SCREEN_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

void displaySendCommand(uint8_t cmd) {
	HAL_GPIO_WritePin(SCREEN_DS_GPIO_Port, SCREEN_DS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspiX, &cmd, 1, HAL_MAX_DELAY);
}

void displaySendData(uint8_t* data, uint16_t size) {
	HAL_GPIO_WritePin(SCREEN_DS_GPIO_Port, SCREEN_DS_Pin, GPIO_PIN_SET);

    HAL_SPI_Transmit(&hspiX, data, size, HAL_MAX_DELAY);
}

