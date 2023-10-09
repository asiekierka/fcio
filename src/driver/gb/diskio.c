#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../diskio.h"
#include "../sd.c"


#define FCIO_GB_DRIVER_EDGB 0x00
#define FCIO_GB_DRIVER_EZFJ 0x01
#define FCIO_GB_DRIVER_UNK  0x0F

uint8_t fcio_gb_driver = FCIO_GB_DRIVER_UNK;
// EDGB: SD card flags
// EZFJ: Write position (0-512)
static uint16_t fcio_gb_local;

#include "edgb.c"
#include "ezfj.c"

DSTATUS disk_initialize (void) {
	switch (fcio_gb_driver) {
	case FCIO_GB_DRIVER_EDGB:
		if (!edgb_sd_init())
			return STA_NOINIT;
		break;
	}
	return 0;
}

void fcio_unlock(void) {
	if (fcio_gb_driver == FCIO_GB_DRIVER_UNK) {
		// Auto-detection code

		// EverDrive GB: SPI byte only sets lowest four bits
		// TODO: Is there a better way?
		edgb_unlock();
		EDGB_REG_SPI_CTRL = 0xF7;
		if ((EDGB_REG_SPI_CTRL & 0x7F) == 0x07) {
			EDGB_REG_SPI_CTRL = EDGB_SPI_SS;
			if (EDGB_REG_SPI_CTRL == EDGB_SPI_SS) {
				fcio_gb_driver = FCIO_GB_DRIVER_EDGB;
				// Already unlocked, so return.
				return;
			}
		}

		// EZ-Flash Junior: If all else fails...
		fcio_gb_driver = FCIO_GB_DRIVER_EZFJ;
	}
	switch (fcio_gb_driver) {
	case FCIO_GB_DRIVER_EDGB:
		edgb_unlock();
		break;
	case FCIO_GB_DRIVER_EZFJ:
		ezfj_unlock();
		break;
	}
}

void fcio_lock(void) {
	switch (fcio_gb_driver) {
	case FCIO_GB_DRIVER_EDGB:
		edgb_lock();
		break;
	case FCIO_GB_DRIVER_EZFJ:
		ezfj_lock();
		break;
	}
}

DRESULT disk_readp (
	uint8_t* buff,		/* Pointer to the destination object */
	uint32_t sector,	/* Sector number (LBA) */
	uint16_t offset,	/* Offset in the sector */
	uint16_t count		/* Byte count (bit15:destination) */
)
{
	switch (fcio_gb_driver) {
	case FCIO_GB_DRIVER_EDGB: {
 		// TODO: Implement read to buffer
		if (!edgb_sd_read_start(sector))
			return RES_ERROR;
		EDGB_REG_SPI_CTRL &= ~EDGB_SPI_SS;
		uint8_t resp = edgb_spi_recv();
		while (resp == 0xFF) {
			resp = edgb_spi_recv();
		}
		if (resp != 0xFE)
			return RES_ERROR;
		while (offset > 0) {
			edgb_spi_recv();
			offset--;
		}
		while (count > 0) {
			*(buff++) = edgb_spi_recv();
			count--;
		}
		int remainder = 514 - count - offset;
		while (remainder > 0) { 
			edgb_spi_recv();
			remainder--;
		}
		EDGB_REG_SPI_CTRL |= EDGB_SPI_SS;
		return RES_OK;
	}
	case FCIO_GB_DRIVER_EZFJ: {
		ezfj_sd_read_start(sector);
		memcpy(buff, EZFJ_SD_SRAM + offset, count);
		ezfj_sd_read_finish();
		return RES_OK;
	}
	default:
		return RES_PARERR;
	}
}

#ifdef FCIO_FS_WRITE
DRESULT disk_writep (
	uint8_t* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	uint32_t sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	switch (fcio_gb_driver) {
	case FCIO_GB_DRIVER_EDGB:
		// TODO: EDGB write support
		return RES_ERROR;
	case FCIO_GB_DRIVER_EZFJ: {
		if (!buff) {
			if (sc) {
				ezfj_sd_write_start(sc);
				fcio_gb_local = 0;
			} else {
				ezfj_sd_write_finish();
			}
		} else {
			memcpy(EZFJ_SD_SRAM, EZFJ_SD_SRAM + fcio_gb_local, sc);
			fcio_gb_local += sc;
		}
		return RES_OK;
	}
	default:
		return RES_PARERR;
	}
}
#endif
