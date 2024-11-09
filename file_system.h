#ifndef __FILE_SYSTEM__
#	define __FILE_SYSTEM__ 1
#	include <time.h>

#	define FILENAME "myfs"
	// ALL SIZES ARE CONSIDERED IN ITS UNIT
#	define SIZE_BOOTBLOCK 2
#	define SIZE_SUPERBLOCK 48
#	define SIZE_INODELIST_IN_SUPERBLOCK 16 // ADDITIONAL DETAIL
#	define SIZE_DATABLOCK_IN_SUPERBLOCK 32 // TOO
#	define SIZE_INODELIST 128
#	define SIZE_DATABLOCK 256
#	define DIRECTORY 1
#	define GENERAL 0

typedef union {
	struct {
		unsigned char b7 : 1, b6 : 1, b5 : 1, b4 : 1, b3 : 1, b2 : 1, b1 : 1, b0 : 1;
	};
	struct {
	};
	unsigned char for_shift;
} Byte;

typedef struct {
} SuperBlock;

typedef struct {
	unsigned int size;
} InodeList;

typedef union {
    unsigned char subfiles[32][8]; // Case it is directory
    // 7 bytes : name 1 byte : inode
    unsigned char contents[256];   // Case it is general file
} DataBlock;

#endif
