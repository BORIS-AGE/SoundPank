/*
 * RDA.h
 *
 *  Created on: Mar 17, 2026
 *      Author: Shrek
 */

#ifndef SRC_RDA_LIB_RDA_H_
#define SRC_RDA_LIB_RDA_H_



#endif /* SRC_RDA_LIB_RDA_H_ */


void rda_init(
		I2C_HandleTypeDef *hi2c,
		float frequency,
		uint8_t bassBoost,
		uint8_t enableRDSDecode,
		uint8_t enableNewDemodMethod,
		uint8_t isUSA
);

uint16_t calculateFrequency(float frequency);
