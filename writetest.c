#include "IOStream.h"
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
    setSuperBlock(2, 1);
    setSuperBlock(3, 1);
    setSuperBlock(4, 1);
    setSuperBlock(5, 1);
    setSuperBlock(6, 1);
    setSuperBlock(7, 1);
    setSuperBlock(SIZE_INODELIST + 1, 1);
    // setSuperBlock(SIZE_INODELIST + 2, 1);
    // setSuperBlock(SIZE_INODELIST + 3, 1);

    time_t curTime;
        
    // InodeList il;
    // int i, j;
	// int query;
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

	SuperBlock sb = getSuperBlock();
	DataBlock db; // ?
	for ( i = 0; i < SIZE_INODELIST_IN_SUPERBLOCK; i++ ) {
		for ( j = 0; j < 8; j++ ) {
			printf("%d", sb.inode_list[i].firstBit);
			sb.inode_list[i].forShift <<= 1;
		}
		/* In this for-loop, the origin SuperBlock are going damaged.
		At last after the loop, SuperBlock includes only 0s.
		If it might cause fundamentally unfixable defect-to fix this, need to fix the logic or struct, The wise will bring the fixed opinion.*/
		printf(" ");
	}
	printf("\n");
	for ( i = 0; i < SIZE_DATABLOCK_IN_SUPERBLOCK; i++ ) {
		for ( j = 0; j < 8; j++ ) {
			printf("%d", sb.data_block[i].firstBit);
			sb.data_block[i].forShift <<= 1;
		}
		printf(" ");
	}
	printf("\n");

	while(1) {
		printf("query for InodeList : ");
		scanf("%d", &query);
		if ( query >= 1 && query <= SIZE_INODELIST ) {
			il = getInodeList(query); // it starts at 1.
			printInodeList(il);
		}

		printf("\n");

		printf("query for DataBlock : ");
		scanf("%d", &query);
		if ( query >= 0 && query <= SIZE_DATABLOCK - 1 ) { // since it starts at 0.
			db = getDataBlock(query);
			printDataBlock(db);
		}

		printf("\n");

	}
	return 0;
}
void printInodeList(InodeList il) {
	int i;
	printf("typeAndDorS : %d\n", il.typeAndDorS.forShift); // it wasn't used for shift ..
	printf("date : %ld\n", il.date); // temporary
	printf("size : %u\n", il.size);
	printf("directs : ");
	for ( i = 0; i < 8; i++ )
		printf("%d ", il.direct[i]);
	printf("\n");
}
void printDataBlock(DataBlock db) {
	int i;
	for ( i = 0; i < sizeof(DataBlock); i++ )
		printf("%c", db.contents[i]);
}