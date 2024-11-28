/* It's too simple code to comment.
 * ( In actual, I expect you to read this without comments for understanding entire code. )
 */

#include "header.h"

void printInodeList(InodeList il);
void printDataBlock(DataBlock db);

int main(int argc, char *argv[])
{
	SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
	DataBlock data_block;
	int i, j;
	int query;

	for (i = 0; i < SIZE_INODELIST_IN_SUPERBLOCK; i++) {
		for (j = 0; j < 8; j++) {
			printf("%d", super_block.inode_list[i].first_bit);
			super_block.inode_list[i].for_shift <<= 1;
		}
		/* In this for-loop, the origin SuperBlock are going damaged.
		 * At last after the loop, SuperBlock includes only 0s.
		 * If it might cause fundamentally unfixable defect-to fix this, need to fix the logic or struct,
		 * The wise will bring the fixed opinion.
		 */
		printf(" ");
	}
	printf("\n");
	for (i = 0; i < SIZE_DATABLOCK_IN_SUPERBLOCK; i++) {
		for (j = 0; j < 8; j++) {
			printf("%d", super_block.data_block[i].first_bit);
			super_block.data_block[i].for_shift <<= 1;
		}
		printf(" ");
	}
	printf("\n");
	while (1) {
		printf("query for th? : ");
		scanf("%d", &query);
		if (query >= 1 && query <= SIZE_INODELIST) {
			printf("inode : \n");
			inode_list = getInodeList(query); // it starts at 1.
			printInodeList(inode_list);
			printf("\n");
			printf("datablock : \n");
			data_block = getDataBlock(query-1);
			printDataBlock(data_block);
		}
		printf("\n");
	}
	return 0;
}
void printInodeList(InodeList inode_list)
{
	int i;

	printf("type and direct or indirect : %d\n", inode_list.file_mode); // it wasn't used for shift ..
	printf("date : %ld\n", inode_list.birth_date); // temporary
	printf("size : %u\n", inode_list.size);
	printf("directs : ");
	for (i = 0; i < 8; i++)
		printf("%d ", inode_list.direct_address[i]);
	printf("\n");
}
void printDataBlock(DataBlock data_block)
{
	int i;

	for (i = 0; i < sizeof(DataBlock); i++){
		if(data_block.contents[i] == '\0')
			printf(" ");
		else if('a' <= data_block.contents[i] && data_block.contents[i] <= 'z')
			printf("%c", data_block.contents[i]);
		else
			printf("%d", data_block.contents[i]);
	}
}
