#include "diskio.h"
#include "sd_spi.h"

typedef uint32_t LBA_t;

DSTATUS disk_initialize(BYTE pdrv) {
    if (sd_startup()== 0){
    	sd_read_ocr();
        return 0;
    }
    return STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) {
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
    for (UINT i = 0; i < count; i++) {
        if (sd_read_block(sector + i, buff + i * 512) != 0)
            return RES_ERROR;
    }
    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
    for (UINT i = 0; i < count; i++) {
        if (sd_write_block(sector + i, buff + i * 512) != 0)
            return RES_ERROR;
    }
    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD*)buff = 1;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}
