/*
 * RDA.c
 *
 *  Created on: Mar 17, 2026
 *      Author: Shrek
 */


#include "main.h"

const uint8_t RDA_WRITE = 0b00100000;
const uint8_t RDA_READ = 0b00100001;


void rda_init(
		I2C_HandleTypeDef *hi2c,
		float frequency,
		uint8_t bassBoost,
		uint8_t enableRDSDecode,
		uint8_t enableNewDemodMethod,
		uint8_t isUSA
) {

	char data[12] = {0};

	// 02H
	data[0] = 0b11100000;
	if (bassBoost) {
		data[0] = data[0] |= 0b00010000; // enable bass boost
	}

	data[1] = 0b00000001;
	if(enableRDSDecode) {
		data[1] = data[1] |= 0b00001000;
	}
	if(enableNewDemodMethod) {
		data[1] = data[1] |= 0b00000100;
	}

	// 03H
	uint16_t calculatedFreq = calculateFrequency(frequency);
	data[2] = (uint8_t)(calculatedFreq >> 2);
	data[3] = ((calculatedFreq << 6) & 0b0000000011000000);
	data[3] |= 0b00011000;

	// 04H
	data[4] = 0b00000001; // change 1 to 0 if want to enable auto frequency tune (fine)

	if(isUSA) {
		data[4] |= data[4] | 0b00100000; // set RDS mode for usa and filter frequencies
	} else {
		data[4] |= data[4] | 0b00001000; // set RDS mode for EU and filter frequencies
	}

	data[5] = 0b00000000;

	// 05H
	data[6] = 0b00001000; // default
	data[7] = 0b10110111; // select antena and 2.1mA current for it. Last 4 bit - volume

	// 06H
	data[8] = 0b00000000;
	data[9] = 0b00000000;

    // 07H
	data[10] = 0b01000000; // default
	data[11] = 0b00000010; // default


	HAL_I2C_Master_Transmit(hi2c, RDA_WRITE, data, sizeof(data), 100);
}

uint16_t calculateFrequency(float frequency) {
	float startFrequency = 76; // 76MHz
	float step = 0.1; // 100kHz

	return (frequency - startFrequency) / step;
}
