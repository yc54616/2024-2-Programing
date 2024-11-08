#include "io_stream.h"
#include <stdio.h>

// bootblock 2byte=16bit
// superblock 128+256bit=48bit*8bit=48byte=384bit
// inode 32byte*128=4096byte=32768bit
// dataBlock 256byte*256=65536byte=524288bit
// total 69682byte=557456
void printInodeList(InodeList il);
void printDataBlock(DataBlock db);
int main()
{
    initFilesystem();
    
    // for(int i = 1; i <= 384; i++){
    //     setSuperBlock(i, 0);
    // }
    setSuperBlock(1, 1);
    // setSuperBlock(2, 1);
    // setSuperBlock(3, 1);
    // setSuperBlock(4, 1);
    // setSuperBlock(5, 1);
    setSuperBlock(SIZE_INODELIST + 1, 1);
    // setSuperBlock(SIZE_INODELIST + 2, 1);
    // setSuperBlock(SIZE_INODELIST + 3, 1);

    time_t curTime;
        
    // InodeList il;
    // int i, j;
	// int query;
    time(&curTime);
    unsigned char address[8] = {0,};
    setInodeList(1, DIRECTORY, curTime, curTime, 8*2, 1, address, 0); // /
    // address[0] = 1;
    // setInodeList(2, DIRECTORY, curTime, curTime, 8*4, 1, address, 0); // /hel
    // address[0] = 2;
    // setInodeList(3, DIRECTORY, curTime, curTime, 8*3, 1, address, 0); // /hel/lo
    // setInodeList(4, DIRECTORY, curTime, curTime, 16, 0, address, 0); // /hel/lo/world
    // setInodeList(5, DIRECTORY, curTime, curTime, 16, 0, address, 0); // /hel/loworld
    // //unsigned char test[SIZE_DATABLOCK] = {0,};

    // // // writeDirectoryDataBlock(name, datablockIndex, startIndex)
    // writeDirectoryDataBlock("hel\x00\x00\x00\x00\x02", 0, 0);
    // // test[0] = 'h', test[1] = 'e', test[2] = 'l', test[3] = '\0', test[7] = 2;
    // // setDataBlock(0, test);
    // writeDirectoryDataBlock("lo\x00\x00\x00\x00\x00\x03", 1, 0);
    // writeDirectoryDataBlock("loworl\x00\x05", 1, 8);

    // // test[0] = 'l', test[1] = 'o', test[2] = '\0', test[7] = 3;
    // // test[8] = 'l', test[9] = 'o', test[10] = 'w', test[11] = 'o', test[12] = 'r', test[13] = 'l', test[14] = 'd', test[15] = 5;
    // // setDataBlock(1, test);

    // writeDirectoryDataBlock("world\x00\x00\x05", 2, 0);
    // // test[0] = 'w', test[1] = 'o', test[2] = 'r', test[3] = 'l', test[4] = 'd', test[5] = '\0', test[7] = 4;
    // // setDataBlock(2, test);
    // printf("%d", findEmptyInode());

	return 0;
}
void printInodeList(InodeList il) {
	int i;
	printf("size : %u\n", il.size);
	printf("\n");
}
void printDataBlock(DataBlock db) {
	int i;
	for ( i = 0; i < sizeof(DataBlock); i++ )
		printf("%c", db.contents[i]);
}
