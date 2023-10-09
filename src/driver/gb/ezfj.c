#include <stdint.h>

#define EZFJ_REG_KEY1 (*((volatile uint8_t*) 0x7F00))
#define EZFJ_REG_KEY2 (*((volatile uint8_t*) 0x7F10))
#define EZFJ_REG_KEY3 (*((volatile uint8_t*) 0x7F20))
#define EZFJ_REG_KEY4 (*((volatile uint8_t*) 0x7FF0))
#define EZFJ_REG_SD_SRAM_CTRL (*((volatile uint8_t*) 0x7F30))
#define EZFJ_SD_SRAM_UNMAP 0x00
#define EZFJ_SD_SRAM_MAP_DATA 0x01
#define EZFJ_SD_SRAM_MAP_STATUS 0x03
#define EZFJ_REG_SD_SECTOR (*((volatile uint32_t*) 0x7FB0))
#define EZFJ_REG_SD_CTRL (*((volatile uint8_t*) 0x7FB4))
#define EZFJ_SD_READ(n) (n)
#define EZFJ_SD_WRITE(n) (0x80 | (n))
#define EZFJ_SD_SRAM ((volatile uint8_t*) 0xA000)

void ezfj_unlock(void) {
    EZFJ_REG_KEY1 = 0xE1;
    EZFJ_REG_KEY2 = 0xE2;
    EZFJ_REG_KEY3 = 0xE3;
}

void ezfj_lock(void) {
    EZFJ_REG_KEY4 = 0xE4;
}

static void ezfj_sd_busywait(void) {
    EZFJ_REG_SD_SRAM_CTRL = EZFJ_SD_SRAM_MAP_STATUS;
    while (EZFJ_SD_SRAM[0] & 0xF0);
}

void ezfj_sd_read_start(uint32_t sector) {
    EZFJ_REG_SD_SRAM_CTRL = EZFJ_SD_SRAM_UNMAP;
    EZFJ_REG_SD_SECTOR = sector;
    EZFJ_REG_SD_CTRL = EZFJ_SD_READ(1);
    ezfj_sd_busywait();
    EZFJ_REG_SD_SRAM_CTRL = EZFJ_SD_SRAM_MAP_DATA;
}

void ezfj_sd_read_finish(void) {
    EZFJ_REG_SD_SRAM_CTRL = EZFJ_SD_SRAM_UNMAP;
}

void ezfj_sd_write_start(uint32_t sector) {
    EZFJ_REG_SD_SRAM_CTRL = EZFJ_SD_SRAM_MAP_DATA;
    EZFJ_REG_SD_SECTOR = sector;
}

void ezfj_sd_write_finish(void) {
    EZFJ_REG_SD_CTRL = EZFJ_SD_WRITE(1);
    ezfj_sd_busywait();
    EZFJ_REG_SD_SRAM_CTRL = EZFJ_SD_SRAM_UNMAP;
}
