#ifndef __FCIO_CONFIG_H__
#define __FCIO_CONFIG_H__

#ifndef __FCIO_H__
#error Include <fcio.h>, not <fcio_config.h>!
#endif

/* Filesystem Options */

// Use Petit FatFs instead of full FatFs.
#define FCIO_FS_PETIT

// Options for both FatFs libraries
#define FCIO_FS_WRITE
#define FCIO_FS_SEEK
#define FCIO_FS_READDIR

// Options for "full" FatFs only

#endif /* __FCIO_CONFIG_H__ */
