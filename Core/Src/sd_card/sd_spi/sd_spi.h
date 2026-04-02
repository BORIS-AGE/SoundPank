#ifndef SD_SPI_H
#define SD_SPI_H

#include "stm32f4xx_hal.h"

uint8_t sd_startup(void);
uint8_t sd_read_block(uint32_t sector, uint8_t *buffer);
uint8_t sd_write_block(uint32_t sector, const uint8_t *buffer);
uint8_t sd_read_ocr();

#endif
