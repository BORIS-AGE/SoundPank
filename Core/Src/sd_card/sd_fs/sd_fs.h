/*
 * sd_fs.h
 *
 *  Created on: Mar 26, 2026
 *      Author: Shrek
 */

#ifndef SRC_SD_CARD_SD_FS_SD_FS_H_
#define SRC_SD_CARD_SD_FS_SD_FS_H_

void fs_list_root(char* path);
void writeFile(uint8_t* array, unsigned int fileSize, char* name);
void read_file(char* fileName, void (*process)(uint8_t* data, uint32_t len, uint16_t iterationNumber));

#endif /* SRC_SD_CARD_SD_FS_SD_FS_H_ */
