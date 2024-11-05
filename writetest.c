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
    setSuperBlock(384, 1);

    time_t curTime;
        
    InodeList il;
    int i, j;
	int query;
    time(&curTime);
    unsigned char address[8] = {128,188,12,13,1,1,9,10};
    setInodeList(2, 1, curTime, curTime, 0, 8, address, 0);
    unsigned char test[SIZE_DATABLOCK] = {0,};
    test[0] = 'a';
    setDataBlock(0, test);

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
