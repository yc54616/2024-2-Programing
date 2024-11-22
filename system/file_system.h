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
#	define SIZE_DIRECT_POINTER 8
#	define DIRECTORY 1
#	define GENERAL 0

typedef union {
	struct {
		unsigned char b7 : 1, b6 : 1, b5 : 1, b4 : 1, b3 : 1, b2 : 1, b1 : 1, b0 : 1;
	};
	struct {
		unsigned char last_bit : 1, skip : 6, first_bit : 1;
	};
	unsigned char for_shift;
} Byte;

typedef struct {
	Byte inode_list[SIZE_INODELIST/8]; // 16 bytes = 128 bits
	Byte data_block[SIZE_DATABLOCK/8]; // 32 bytes = 256 bits
} SuperBlock;

typedef struct {
	unsigned char file_mode; // excluding permission ex) rwxr--r--

	time_t access_date;
	time_t birth_date;
	unsigned int size;
	unsigned char reference_count; //referring to number of pointers are consisted by a file.
	unsigned char direct_address[SIZE_DIRECT_POINTER];
	unsigned char single_indirect_address;
} InodeList;

typedef union {
    unsigned char subfiles[32][8]; // Case it is directory
    // 7 bytes : name 1 byte : inode
    unsigned char contents[256];   // Case it is general file
} DataBlock;

#endif
