#include "fs_status_print.h"

void myinode(char **commands)
{
	int inode_number;
	InodeList inode_list;
	char c;
	int i;
	int direct_pointer_count;
	
	if (commands[1] == NULL) {
		errmsg("The first argument must be filled!\n");
		return;
	}

	for (i = 0; (c = commands[1][i]) != '\0'; i++) {
		if (c < '0' || c > '9') {
			errmsg("The first arg must be positive integer format!\n");
			return;
		}
	}

	if (i == 0) {
		errmsg("The first arg must be filled!\n");
		return;
	}

	inode_number = strtol(commands[1], NULL, 10);
	if (! (inode_number >= 1 && inode_number <= SIZE_INODELIST)) {
		errmsg("The first arg must be in the (closed) section-[1, %d]!\n", SIZE_INODELIST);
		return;
	}

	inode_list = getInodeList(inode_number);
	direct_pointer_count = (inode_list.reference_count > SIZE_DIRECT_POINTER) ? SIZE_DIRECT_POINTER : inode_list.reference_count;
	printf("종류 : %s\n", (inode_list.file_mode == DIRECTORY) ? "디렉토리" : "파일");
	printf("생성일자 : %ld\n", inode_list.birth_date);
	printf("크기 : %d\n", inode_list.size);
	printf("직접 블럭 목록 :\n");
	for (i = 0; i < direct_pointer_count; i++)
		printf("  #%d 직접 데이터 블록 : %d\n", i, inode_list.direct_address[i]);
	printf("간접 블록 번호 : %d\n", inode_list.single_indirect_address);
}
void mydatablock(char **commands) {
	int block_address;
	DataBlock data_block;
	char c;
	int i;
	
	if (commands[1] == NULL) {
		errmsg("The first argument must be filled!\n");
		return;
	}

	for (i = 0; (c = commands[1][i]) != '\0'; i++) {
		if (c < '0' || c > '9') {
			errmsg("The first arg must be positive integer format!\n");
			return;
		}
	}

	if (i == 0) {
		errmsg("The first arg must be filled!\n");
		return;
	}

	block_address = strtol(commands[1], NULL, 10);
	if (! (block_address >= 0 && block_address <= SIZE_DATABLOCK - 1)) {
		errmsg("The first arg must be in the (closed) section-[0, %d]!\n", SIZE_DATABLOCK - 1);
		return;
	}
	
	data_block = getDataBlock(block_address);
	for (i = 0; i < sizeof(data_block); i++)
		printf("%c", data_block.contents[i]);
	printf("\n");
}
void mystatus(char **commands) {
	SuperBlock super_block;
	int i, j, k;
	int index;
	int loop_size;
	int used;
	Byte * map;
	
	super_block = getSuperBlock();
	used = 0;
	map = (Byte *)malloc(sizeof(Byte) * SIZE_INODELIST_IN_SUPERBLOCK);
	for (i = 0; i < SIZE_INODELIST_IN_SUPERBLOCK; i++) {
		for (j = 0; j < 8; j++) {
			map[i].for_shift <<= 1;
			map[i].last_bit = super_block.inode_list[i].first_bit;
			if (super_block.inode_list[i].first_bit)
				used++;
			super_block.inode_list[i].for_shift <<= 1;
		}
	}

	printf("Inode state : \n");
	printf("    Total : %d\n", SIZE_INODELIST);
	printf("    Used : %d\n", used);
	printf("    Available : %d\n", SIZE_INODELIST - used);
	printf("    Inode Map :\n");
	index = 0;
	for (i = 0; i < 2; i++) {
		printf("        ");
		for (j = 0, loop_size = SIZE_INODELIST_IN_SUPERBLOCK / 2; j < loop_size; j++) {
			for (k = 0; k < 4; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			for ( ; k < 8; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			index++;
		}
		printf("\n");
	}
	free(map);

	used = 0;
	map = (Byte *)malloc(sizeof(Byte) * SIZE_DATABLOCK_IN_SUPERBLOCK);
	for (i = 0; i < SIZE_DATABLOCK_IN_SUPERBLOCK; i++) {
		for (j = 0; j < 8; j++) {
			map[i].for_shift <<= 1;
			map[i].last_bit = super_block.data_block[i].first_bit;
			if (super_block.data_block[i].first_bit)
				used++;
			super_block.data_block[i].for_shift <<= 1;
		}
	}
	printf("\n");

	printf("Data Block state : \n");
	printf("    Total : %d blocks / %ld bytes\n", SIZE_DATABLOCK, SIZE_DATABLOCK * sizeof(DataBlock));
	printf("    Used : %d / %ld bytes\n", used, used * sizeof(DataBlock));
	printf("    Available : %d / %ld bytes\n", SIZE_DATABLOCK - used, ( SIZE_DATABLOCK - used ) * sizeof(DataBlock));
	printf("    Data Block Map :\n");
	index = 0;
	for (i = 0; i < 4; i++) {
		printf("        ");
		for (j = 0, loop_size = SIZE_DATABLOCK_IN_SUPERBLOCK / 4; j < loop_size; j++) {
			for (k = 0; k < 4; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			for ( ; k < 8; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			index++;
		}
		printf("\n");
	}
	printf("\n");
	free(map);
}
