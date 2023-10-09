#include <stdbool.h>
#include <stdint.h>

#define EDGB_LOCAL_HC_CARD 0x01

#define EDGB_REG_SPI_DATA (*((volatile uint8_t*) 0xBD00))
#define EDGB_REG_SPI_CTRL (*((volatile uint8_t*) 0xBD01))
#define EDGB_SPI_SS         0x01
#define EDGB_SPI_FULL_SPEED 0x02
#define EDGB_SPI_AREAD      0x04
#define EDGB_SPI_BUSY       0x80
#define EDGB_REG_KEY (*((volatile uint8_t*) 0xBD0A))
#define EDGB_KEY_UNLOCK 0xA5
#define EDGB_KEY_LOCK   0x00

void edgb_unlock(void) {
    EDGB_REG_KEY = EDGB_KEY_UNLOCK;
}

void edgb_lock(void) {
    EDGB_REG_KEY = EDGB_KEY_LOCK;
}

uint8_t edgb_spi_send(uint8_t value) {
    EDGB_REG_SPI_DATA = value;
    while (EDGB_REG_SPI_CTRL & EDGB_SPI_BUSY);
    return EDGB_REG_SPI_DATA;
}
#define edgb_spi_recv() edgb_spi_send(0xFF)

uint8_t edgb_mmc_cmd(uint8_t cmd, uint8_t crc, uint32_t arg) {
    EDGB_REG_SPI_CTRL &= ~EDGB_SPI_SS;
    edgb_spi_recv(); // clear receive buffer
    edgb_spi_send(cmd);
    edgb_spi_send(arg >> 24);
    edgb_spi_send(arg >> 16);
    edgb_spi_send(arg >> 8);
    edgb_spi_send(arg);
    edgb_spi_send(crc);
    uint8_t resp = edgb_spi_recv();
    uint16_t timeout = 0;
    while (resp == 0xFF && (++timeout) != 0) { // wait for successful response
        resp = edgb_spi_recv();
    }
    EDGB_REG_SPI_CTRL |= EDGB_SPI_SS;
    return resp;
}

bool edgb_sd_read_start(uint32_t sector) {
    if (!(fcio_gb_local & EDGB_LOCAL_HC_CARD)) sector <<= 9;
    if (edgb_mmc_cmd(SDC_READ_SINGLE_BLOCK, 0x95, sector)) return false;
    return true;
}

bool edgb_sd_write_start(uint32_t sector) {
    if (!(fcio_gb_local & EDGB_LOCAL_HC_CARD)) sector <<= 9;
    if (edgb_mmc_cmd(SDC_WRITE_BLOCK, 0x95, sector)) return false;
    return true;
}

bool edgb_sd_init(void) {
    uint8_t retries;

    EDGB_REG_SPI_CTRL = EDGB_SPI_SS;
    fcio_gb_local = 0;
    edgb_mmc_cmd(SDC_GO_IDLE_STATE, 0x95, 0); // Reset card

    if (!(edgb_mmc_cmd(SDC_SEND_IF_COND, 0x87, 0x000001AA) & ~SDC_R1_IDLE)) {
        EDGB_REG_SPI_CTRL &= ~EDGB_SPI_SS;
        uint8_t r3_0 = edgb_spi_recv();
        uint8_t r3_1 = edgb_spi_recv();
        uint8_t r3_2 = edgb_spi_recv();
        uint8_t r3_3 = edgb_spi_recv();
        EDGB_REG_SPI_CTRL |= EDGB_SPI_SS;

        if ((r3_2 & 0xF) == 0x1 && r3_3 == 0xAA) {
            // attempt ACMD41 SDHC init
            retries = 0;
            while (++retries) {
                if (!(edgb_mmc_cmd(SDC_APP_PREFIX, 0x95, 0) & ~SDC_R1_IDLE)) {
                    if (!(edgb_mmc_cmd(SDC_APP_SEND_OP_COND, 0x95, 1UL << 30))) {
                        break;
                    }
                }
            }
            if (retries) {
                // read OCR to check for SDHC card
                if (!(edgb_mmc_cmd(SDC_READ_OCR, 0x95, 0))) {
                    EDGB_REG_SPI_CTRL &= ~EDGB_SPI_SS;
                    r3_0 = edgb_spi_recv();
                    r3_1 = edgb_spi_recv();
                    r3_2 = edgb_spi_recv();
                    r3_3 = edgb_spi_recv();
                    EDGB_REG_SPI_CTRL |= EDGB_SPI_SS;

                    if (r3_0 & 0x40) {
                        fcio_gb_local |= EDGB_LOCAL_HC_CARD;
                    }
                }
                goto card_init_complete;
            }
        }
    }

    // attempt ACMD41 init
    retries = 0;
    while (++retries) {
        if (!(edgb_mmc_cmd(SDC_APP_PREFIX, 0x95, 0) & ~SDC_R1_IDLE)) {
            if (!(edgb_mmc_cmd(SDC_APP_SEND_OP_COND, 0x95, 1UL << 30))) {
                goto card_init_complete;
            }
        }
    }

    // attempt CMD1 init
    retries = 0;
    while (++retries) {
        if (!(edgb_mmc_cmd(SDC_SEND_OP_COND, 0x95, 0))) {
            goto card_init_complete;
        }
    }

    // no more init modes
    return false;

card_init_complete:
    // set block size to 512
    if (edgb_mmc_cmd(SDC_SET_BLOCKLEN, 0x95, 512)) {
        return false;
    }

    EDGB_REG_SPI_CTRL |= EDGB_SPI_FULL_SPEED;
    return true;
}
