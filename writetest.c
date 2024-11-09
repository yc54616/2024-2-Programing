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
    setInodeList(2, 1, 0, curTime, 0, address);
    Byte test[SIZE_DATABLOCK] = {0,};
    test[0].forShift = 'a';
    setDataBlock(0, test);

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