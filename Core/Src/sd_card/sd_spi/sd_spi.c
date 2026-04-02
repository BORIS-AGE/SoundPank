#include "sd_spi.h"

#define MAX_ATTEMPTS_COUNT 100


extern SPI_HandleTypeDef hspi2;

#define CS_LOW()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET)

const uint16_t DATA_RECEIVE_SIZE = 512;


static uint8_t is_sdhc = 0;
static uint8_t dummy_buffer[512];

static uint8_t spi_txrx(uint8_t data) {
    uint8_t rx;
    HAL_SPI_TransmitReceive(&hspi2, &data, &rx, 1, 100);
    return rx;
}

void spi_txrx_massive(uint8_t* data, uint8_t* result, uint16_t size) {
    HAL_SPI_TransmitReceive(&hspi2, data, result, size, HAL_MAX_DELAY);
}

static void spi_dummy_clocks(void) {
    CS_HIGH();
    for (int i = 0; i < 10; i++) spi_txrx(0xFF);
}

static uint8_t sd_cmd(uint8_t cmd, uint32_t arg, uint8_t crc) {
    uint8_t res;

    CS_LOW();

    spi_txrx(0xFF);
    spi_txrx(cmd | 0x40);
    spi_txrx(arg >> 24);
    spi_txrx(arg >> 16);
    spi_txrx(arg >> 8);
    spi_txrx(arg);
    spi_txrx(crc);

    for (int i = 0; i < 10; i++) {
        res = spi_txrx(0xFF);
        if (!(res & 0x80)) break;
    }

    return res;
}

uint8_t sd_startup(void) {
    uint8_t res;

    memset(dummy_buffer, 0xFF, 512);

    spi_dummy_clocks();

    // CMD0
    uint16_t attemptCounter = MAX_ATTEMPTS_COUNT;
    do {
        res = sd_cmd(0, 0, 0x95);
        spi_txrx(0xFF);
    } while (attemptCounter-- != 0 && res != 0x01);

    if(attemptCounter == 0) {
    	return 1;
    }

    // CMD8
    res = sd_cmd(8, 0x1AA, 0x87);
    spi_txrx(0xFF);

    // ACMD41 loop
    attemptCounter = MAX_ATTEMPTS_COUNT;

    do {
        sd_cmd(55, 0, 0);
        spi_txrx(0xFF);

        res = sd_cmd(41, 0x40000000, 0);
        spi_txrx(0xFF);
    } while (attemptCounter-- != 0 && res != 0x00);

    if(attemptCounter == 0) {
    	return 2;
    }

    return 0;
}

uint8_t sd_read_block(uint32_t sector, uint8_t *buffer) {
    uint32_t addr = is_sdhc ? sector : sector * 512;

    uint8_t token;
    int timeout = 100000;

    if (sd_cmd(17, addr, 0) != 0) {
        CS_HIGH();
        return 1;
    }

    while ((token = spi_txrx(0xFF)) != 0xFE) {
        if (--timeout == 0) {
            CS_HIGH();
            return 1;
        }
    }

    spi_txrx_massive(dummy_buffer, buffer, DATA_RECEIVE_SIZE);

    spi_txrx(0xFF);
    spi_txrx(0xFF);

    CS_HIGH();
    spi_txrx(0xFF);

    return 0;
}

uint8_t sd_write_block(uint32_t sector, const uint8_t *buffer){
    uint32_t addr = is_sdhc ? sector : sector * 512;

    if (sd_cmd(24, addr, 0) != 0) {
        CS_HIGH();
        return 1;
    }

    spi_txrx(0xFF);
    spi_txrx(0xFE); // start token

    spi_txrx_massive(buffer, dummy_buffer, DATA_RECEIVE_SIZE);


    spi_txrx(0xFF); // dummy CRC
    spi_txrx(0xFF);

    uint8_t resp = spi_txrx(0xFF);
    if ((resp & 0x1F) != 0x05) {
        CS_HIGH();
        return 1;
    }

    // wait busy
    while (spi_txrx(0xFF) == 0);

    CS_HIGH();
    spi_txrx(0xFF);

    return 0;
}

uint8_t sd_read_ocr(void) {
    uint8_t ocr[4];

    if (sd_cmd(58, 0, 0) != 0) return 1;

    for (int i = 0; i < 4; i++)
        ocr[i] = spi_txrx(0xFF);

    CS_HIGH();
    spi_txrx(0xFF);

    if (ocr[0] & 0x40) {
        is_sdhc = 1;
    }

    return is_sdhc;
}
