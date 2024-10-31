#ifndef __FILE_SYSTEM__
#	define __FILE_SYSTEM__
#	define FILENAME "myfs"
	// ALL SIZES ARE CONSIDERED IN ITS UNIT
#	define SIZE_BOOTBLOCK 2
#	define SIZE_SUPERBLOCK 48
#	define SIZE_INODELIST_IN_SUPERBLOCK 16 // ADDITIONAL DETAIL
#	define SIZE_DATABLOCK_IN_SUPERBLOCK 32 // TOO
#	define SIZE_INODELIST 128
#	define SIZE_DATABLOCK 256
#include <time.h>
typedef union {
	struct {
		unsigned char b7 : 1, b6 : 1, b5 : 1, b4 : 1, b3 : 1, b2 : 1, b1 : 1, b0 : 1;
	};
	struct {
		unsigned char lastBit : 1, skip : 6, firstBit : 1; 
	};
	unsigned char forShift;
} Byte;

typedef struct {
	Byte inode_list[16]; // 8 * 16 bit = 128 bit
	Byte data_block[32]; // 8 * 32 bit = 256 bit
} SuperBlock;
typedef struct {
	Byte typeAndDorS; // Data type ( directory or general file ) and ( Direct or Single Indirect )
	//                       0                                           0000000
	time_t date;
	unsigned int size;
	union {
		unsigned char direct[8];
		unsigned char single;
	};
} InodeList;

typedef union {
    unsigned char subfiles[32][8]; // Case it is directory
    // 7 bytes : name 1 byte : inode
    unsigned char contents[256];   // Case it is general file
    Byte content_bin[256];
} DataBlock;

#endif
