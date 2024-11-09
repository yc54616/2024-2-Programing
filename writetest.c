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
    
    for(int i = 1; i <= 384; i++){
        setSuperBlock(i, 0);
    }
    setSuperBlock(1, 1);
    setSuperBlock(2, 1);
    setSuperBlock(3, 1);
    setSuperBlock(4, 1);
    setSuperBlock(5, 1);
    setSuperBlock(6, 1);
    setSuperBlock(7, 1);
    setSuperBlock(SIZE_INODELIST + 1, 1);
    setSuperBlock(SIZE_INODELIST + 2, 1);
    setSuperBlock(SIZE_INODELIST + 3, 1);

    time_t curTime;
        
    InodeList il;
    int i, j;
	int query;
    time(&curTime);
    unsigned char address[8] = {0,};
    setInodeList(1, DIRECTORY, curTime, curTime, 16, 1, address, 0); // /
    address[0] = 1;
    setInodeList(2, DIRECTORY, curTime, curTime, 24, 1, address, 0); // /hel
    address[0] = 2;
    setInodeList(3, DIRECTORY, curTime, curTime, 8, 1, address, 0); // /hel/lo
    setInodeList(4, DIRECTORY, curTime, curTime, 0, 0, address, 0); // /hel/lo/world
    setInodeList(5, DIRECTORY, curTime, curTime, 0, 0, address, 0); // /hel/loworld
    setInodeList(6, GENERAL, curTime, curTime, 0, 0, address, 0); // /hel/apple
    setInodeList(7, GENERAL, curTime, curTime, 0, 0, address, 0); // /I-phone
    unsigned char test[SIZE_DATABLOCK] = {0,};
    test[0] = 'h', test[1] = 'e', test[2] = 'l', test[3] = '\0', test[7] = 2;
    test[8] = 'I', test[9] = '-', test[10] = 'p', test[11] = 'h', test[12] = 'o', test[13] = 'n', test[14] = 'e', test[15] = 7;
    setDataBlock(0, test);
    test[0] = 'l', test[1] = 'o', test[2] = 'w', test[3] = 'o', test[4] = 'r', test[5] = 'l', test[6] = 'd', test[7] = 5;
    test[8] = 'l', test[9] = 'o', test[10] = '\0', test[15] = 3;
    test[16] = 'a', test[17] = 'p', test[18] = 'p', test[19] = 'l', test[20] = 'e', test[21] = '\0', test[23] = 6;
    setDataBlock(1, test);
    test[0] = 'w', test[1] = 'o', test[2] = 'r', test[3] = 'l', test[4] = 'd', test[5] = '\0', test[7] = 4;
    setDataBlock(2, test);

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
