#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <fcio.h>
#include <gbdk/platform.h>

void main(void) {
  int res;

  #if !defined(MSXDOS) && !defined(NINTENDO_NES)
  set_default_palette();
  #endif

  printf("fcio test program\n");

  printf("Unlocking... ");
  fcio_unlock();
  printf("OK\n");

  printf("Mounting FS... ");
  fcio_fatfs_t fs;
  res = fcio_mount(&fs);
  printf("%d\n", res);
  if (res != 0) return;

  printf("Listing files... ");
  fcio_dir_t dp;
  fcio_filinfo_t fno;
  res = fcio_opendir(&dp, "/");
  printf("%d\n", res);
  if (res != 0) return;

  while (true) {
    res = fcio_readdir(&dp, &fno);
    if (res != 0 || fno.fname[0] == 0) break;
    if (fno.fattrib & AM_DIR) {
      printf("%s/\n", fno.fname);
    } else {
      printf("%s\n", fno.fname);
    }
  }

  // Loop forever
  while(1) {

    // Yield CPU till the end of each frame
    vsync();
  }
}
