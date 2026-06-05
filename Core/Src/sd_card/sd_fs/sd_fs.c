/*
 * sd_fs.c
 *
 *  Created on: Mar 26, 2026
 *      Author: Shrek
 */

#include "main.h"
#include "ff.h"

FATFS fs;
DIR dir;
FILINFO fno;

const uint16_t tempArray[76800];

void fs_list_root(char* path) {
    if (f_mount(&fs, "", 1) != FR_OK) {
        SEGGER_RTT_printf(0, "Mount error\n");
        return;
    }

    if (f_opendir(&dir, path) != FR_OK) {
    	SEGGER_RTT_printf(0, "Open dir error\n");
        return;
    }

    while (1) {
        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0)
            break;

        if (fno.fattrib & AM_DIR) {
        	SEGGER_RTT_printf(0, "[DIR]  %s\n", fno.fname);
        } else {
        	SEGGER_RTT_printf(0, "[FILE] %s\n", fno.fname);
        }
    }

    f_closedir(&dir);
}

void writeFile(uint8_t* array, unsigned int fileSize, char* name) {
	FIL file;
	FRESULT res;
	UINT written;

	// открыть/создать файл
	res = f_open(&file, name, FA_CREATE_ALWAYS | FA_WRITE);
	if (res != FR_OK) {
	    SEGGER_RTT_printf(0, "Open error\n");
	    return;
	}

	res = f_write(&file, array, fileSize, &written);
	if (res != FR_OK || written != fileSize) {
	    SEGGER_RTT_printf(0, "Write error\n");
	}

	SEGGER_RTT_printf(0, "Write file success\n");
	// закрыть файл
	f_close(&file);
}

void read_file(char* fileName, void (*process)(uint8_t* data, uint32_t len, uint16_t iterationNumber)) {
    FIL file;
    UINT read;
    uint8_t buffer[MAX_LOAD_SIZE];
    uint16_t iteration = 0;

    if (f_open(&file, fileName, FA_READ) != FR_OK) {
    	SEGGER_RTT_printf(0, "File \"%s\" doesn't exist\n", fileName);
        return;
    }

    while (1) {
        if (f_read(&file, buffer, sizeof(buffer), &read) != FR_OK) {
            break;
        }

        if (read == 0) break;

        process(buffer, read, iteration++);
    }

    f_close(&file);
}
