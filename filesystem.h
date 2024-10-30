#ifndef __FILE_SYSTEM__
#	define __FILE_SYSTEM_
#include <time.h>
typedef struct {
	unsigned char b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1; 
} Byte;

typedef struct {
	Byte inode_list_bit[16]; // 8 * 16 bit = 128 bit
	Byte data_block_bit[32]; // 8 * 32 bit = 256 bit
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
} DataBlock;

typedef struct {
	Byte boots[2];
	SuperBlock superBlock;
	InodeList Ilist[384];
	DataBlock DBlock[256];
} FileSystem;
#endif
