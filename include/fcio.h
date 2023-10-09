#ifndef __FCIO_H__
#define __FCIO_H__

#include <stdint.h>

#if defined(__SDCC)
#define FCIO_BANKED __banked
#else
#define FCIO_BANKED
#endif

#include "fcio_config.h"

void fcio_unlock(void);
void fcio_lock(void);

/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */

#ifdef FCIO_FS_PETIT
// Petit FatFs types.
/* File system object structure */

typedef struct {
	uint8_t	fs_type;	/* FAT sub type */
	uint8_t	flag;		/* File status flags */
	uint8_t	csize;		/* Number of sectors per cluster */
	uint8_t	pad1;
	uint16_t	n_rootdir;	/* Number of root directory entries (0 on FAT32) */
	uint32_t	n_fatent;	/* Number of FAT entries (= number of clusters + 2) */
	uint32_t	fatbase;	/* FAT start sector */
	uint32_t	dirbase;	/* Root directory start sector (Cluster# on FAT32) */
	uint32_t	database;	/* Data start sector */
	uint32_t	fptr;		/* File R/W pointer */
	uint32_t	fsize;		/* File size */
	uint32_t	org_clust;	/* File start cluster */
	uint32_t	curr_clust;	/* File current cluster */
	uint32_t	dsect;		/* File current data sector */
} fcio_fatfs_t;

/* Directory object structure */

typedef struct {
	uint16_t	index;		/* Current read/write index number */
	uint8_t*	fn;			/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
	uint32_t	sclust;		/* Table start cluster (0:Static table) */
	uint32_t	clust;		/* Current cluster */
	uint32_t	sect;		/* Current sector */
} fcio_dir_t;

/* File status structure */

typedef struct {
	uint32_t	fsize;		/* File size */
	uint16_t	fdate;		/* Last modified date */
	uint16_t	ftime;		/* Last modified time */
	uint8_t	fattrib;	/* Attribute */
	char	fname[13];	/* File name */
} fcio_filinfo_t;

// Petit FatFs functions.
uint8_t fcio_mount(fcio_fatfs_t* fs) FCIO_BANKED;								/* Mount/Unmount a logical drive */
uint8_t fcio_open(const char* path) FCIO_BANKED;							/* Open a file */
uint8_t fcio_read(void* buff, unsigned int btr, unsigned int* br) FCIO_BANKED;			/* Read data from the open file */
#ifdef FCIO_FS_WRITE
uint8_t fcio_write(const void* buff, unsigned int btw, unsigned int* bw) FCIO_BANKED;	/* Write data to the open file */
#endif
#ifdef FCIO_FS_SEEK
uint8_t fcio_lseek(uint32_t ofs) FCIO_BANKED;								/* Move file pointer of the open file */
#endif
#ifdef FCIO_FS_READDIR
uint8_t fcio_opendir(fcio_dir_t* dj, const char* path) FCIO_BANKED;				/* Open a directory */
uint8_t fcio_readdir(fcio_dir_t* dj, fcio_filinfo_t* fno) FCIO_BANKED;					/* Read a directory item from the open directory */
#endif
#else

#error Full FatFs not yet implemented.

#endif

#endif /* __FCIO_H__ */
