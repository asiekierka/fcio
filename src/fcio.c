// Bank definition (GBDK)
#pragma bank 255

#include <stdio.h>

#include "../include/fcio.h"

#include "driver/gb/diskio.c"

#ifdef FCIO_FS_PETIT
#include "pff/pff.c"

uint8_t fcio_mount(fcio_fatfs_t* fs) FCIO_BANKED {
    return pf_mount((FATFS*) fs);
}
uint8_t fcio_open(const char* path) FCIO_BANKED {
    return pf_open(path);
}
uint8_t fcio_read(void* buff, unsigned int btr, unsigned int* br) FCIO_BANKED {
    return pf_read(buff, btr, br);
}
#ifdef FCIO_FS_WRITE
uint8_t fcio_write(const void* buff, unsigned int btw, unsigned int* bw) FCIO_BANKED {
    return fcio_write(buff, btw, bw);
}
#endif
#ifdef FCIO_FS_SEEK
uint8_t fcio_lseek(uint32_t ofs) FCIO_BANKED {
    return fcio_lseek(ofs);
}
#endif
#ifdef FCIO_FS_READDIR
uint8_t fcio_opendir(fcio_dir_t* dj, const char* path) FCIO_BANKED {
    return pf_opendir((DIR*) dj, path);
}
uint8_t fcio_readdir(fcio_dir_t* dj, fcio_filinfo_t* fno) FCIO_BANKED {
    return pf_readdir((DIR*) dj, (FILINFO*) fno);
}
#endif

#endif
