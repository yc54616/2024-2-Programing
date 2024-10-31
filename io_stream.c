#include <stdio.h> // to use FILE struct

#include "io_stream.h"

SuperBlock getSuperBlock(void)
{
	FILE *file;
	int i;
	SuperBlock result;

	// prepare to read file
	file = fopen(FILENAME, "rb");
	fseek(file, SIZE_BOOTBLOCK, SEEK_CUR); // skip boot sector

	fread(result.inode_list, sizeof(Byte), SIZE_INODELIST_IN_SUPERBLOCK, file); // read Inodes
	fread(result.data_block, sizeof(Byte), SIZE_DATABLOCK_IN_SUPERBLOCK, file); // read DataBlocks

	fclose(file);
	return result;
}

InodeList getInodeList(int Inode)
{
	FILE * file;
	int i;
	InodeList result;

	// prepare to read file
	file = fopen(FILENAME, "rb");
	fseek(file, SIZE_BOOTBLOCK, SEEK_CUR); // skip boot sector
	fseek(file, SIZE_SUPERBLOCK * sizeof(Byte), SEEK_CUR); // skip superblock sector
	// I recommend you to use the form of "SIZE * sizeof its unit" becauseof consistency of the whole code.

	fseek(file, sizeof(InodeList) * ( Inode - 1 ), SEEK_CUR); // The first Inode starts at 1.
	fread(&result, sizeof(InodeList), 1, file); // read

	fclose(file);
	return result;
}

DataBlock getDataBlock(int Address)
{
	FILE * file;
	int i;
	DataBlock result;

	// prepare to read file
	file = fopen(FILENAME, "rb");
	fseek(file, SIZE_BOOTBLOCK, SEEK_CUR); // skip boot sector
	fseek(file, SIZE_SUPERBLOCK * sizeof(Byte), SEEK_CUR); // skip superblock sector
	fseek(file, SIZE_INODELIST * sizeof(InodeList), SEEK_CUR); // skip superblock sector
	// I recommend you to use the form of "SIZE * sizeof its unit" becauseof consistency of the whole code.

	fseek(file, sizeof(DataBlock) * Address, SEEK_CUR); // The first Block Address starts at 0.
	fread(&result, sizeof(DataBlock), 1, file); // read

	fclose(file);
	return result;
}
