#include <stdio.h> // to use FILE struct
#include <stdbool.h>

#include "io_stream.h"

void setBit(Byte *byte, int index, bool bit)
{
	switch (index)
	{
	case 0:
		byte->b0 = bit;
		break;
	case 1:
		byte->b1 = bit;
		break;
	case 2:
		byte->b2 = bit;
		break;
	case 3:
		byte->b3 = bit;
		break;
	case 4:
		byte->b4 = bit;
		break;
	case 5:
		byte->b5 = bit;
		break;
	case 6:
		byte->b6 = bit;
		break;
	case 7:
		byte->b7 = bit;
		break;
	}
}

void initFilesystem()
{

	SuperBlock sb; // = getSuperBlock();
	InodeList in[SIZE_INODELIST];
	DataBlock db[SIZE_DATABLOCK];
	int i, j;
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			sb.inode_list[i].forShift >>= 1;
			sb.inode_list[i].firstBit = 0;
		}
	}
	for (int i = 0; i < SIZE_INODELIST; i++)
	{
		in[i].typeAndDorS.forShift = 0;
		in[i].date = 0;
		in[i].size = 0;
		for (int j = 0; j < 8; j++)
		{
			in[i].direct[j] = 0;
		}
	}
	for (int i = 0; i < SIZE_DATABLOCK; i++)
	{
		for (int j = 0; j < SIZE_DATABLOCK; j++)
		{
			db[i].contents[j] = 0;
		}
	}

	setFilesystem(sb, in, db);
}

void setFilesystem(SuperBlock sb, InodeList in[SIZE_INODELIST], DataBlock db[SIZE_DATABLOCK])
{
	FILE *file;
	unsigned char BootBlockArray[SIZE_BOOTBLOCK] = {0, 0};
	file = fopen(FILENAME, "wb");
	fwrite(&BootBlockArray, sizeof(BootBlockArray), 1, file);
	fwrite(&sb, sizeof(sb), 1, file);
	fwrite(in, sizeof(*in) * SIZE_INODELIST, 1, file);
	fwrite(db, sizeof(*db) * SIZE_DATABLOCK, 1, file);
	fclose(file);
}

void setSuperBlock(int bitIndex, bool bit)
{ // 1 ~ 384로 inode와 datablock index 다 합쳐서
	FILE *file;
	file = fopen(FILENAME, "rb+");
	fseek(file, SIZE_BOOTBLOCK, SEEK_CUR); // skip boot sector

	if (bitIndex < 1 || bitIndex > (SIZE_INODELIST + SIZE_DATABLOCK))
		bitIndex = 1;

	// 1 -> 0,0
	// 2 -> 0,1
	// 8 -> 0,7
	// 128 -> 15,7
	SuperBlock sb;

	if (bitIndex > SIZE_INODELIST)
		setBit(&sb.data_block[(bitIndex - 129) / 8], (bitIndex - 129) % 8, bit);
	else
		setBit(&sb.inode_list[(bitIndex - 1) / 8], (bitIndex - 1) % 8, bit);

	fwrite(&sb, sizeof(sb), 1, file);

	fclose(file);
}

// index : inode index
// type: directory=1, file=0
// dAndI: Direct=0, Single Indirect=1
// date: date
// size: file size
// address: 일단 SingleIndirect이더라도 맨 첫 배열만 사용
void setInodeList(int index, bool type, bool dAndI, time_t date, unsigned int size, unsigned char address[6]) // 1~128
{																											  // 1 ~ 384로 inode와 datablock index 다 합쳐서
	FILE *file;
	file = fopen(FILENAME, "rb+");
	fseek(file, SIZE_BOOTBLOCK, SEEK_CUR); // skip boot sector
	fseek(file, SIZE_SUPERBLOCK, SEEK_CUR);
	// 1 -> 0,0
	// 2 -> 0,1
	// 8 -> 0,7
	// 128 -> 15,7
	InodeList in;
	in.typeAndDorS.firstBit = type;
	in.typeAndDorS.lastBit = dAndI;
	in.date = date;
	in.size = size;
	if (dAndI == 0)
	{
		for (int i = 0; i < 8; i++)
		{
			in.direct[i] = address[i];
		}
	}
	else
	{
		in.single = address[0];
	}

	fseek(file, 32 * (index - 1), SEEK_CUR);
	fwrite(&in, sizeof(in), 1, file);

	fclose(file);
}

// address : datablock address
// type : directory=1 general file=0
void setDataBlock(int address, Byte content_bin[SIZE_DATABLOCK]) // 1~128
{																											  // 1 ~ 384로 inode와 datablock index 다 합쳐서
	FILE *file;
	file = fopen(FILENAME, "rb+");
	fseek(file, SIZE_BOOTBLOCK, SEEK_CUR); // skip boot sector
	fseek(file, SIZE_SUPERBLOCK, SEEK_CUR);
	fseek(file, SIZE_INODELIST * sizeof(InodeList), SEEK_CUR);
	// 1 -> 0,0
	// 2 -> 0,1
	// 8 -> 0,7
	// 128 -> 15,7
	DataBlock db;
	for(int i = 0; i < SIZE_DATABLOCK; i++){
		db.content_bin[i] = content_bin[i];
	}
	
	fseek(file, sizeof(DataBlock) * address, SEEK_CUR);
	fwrite(&db, sizeof(db), 1, file);

	fclose(file);
}

SuperBlock getSuperBlock()
{

	// variables declaration
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
	FILE *file;
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

	// variables declaration
	FILE *file;
	int i;
	DataBlock result;

	// prepare to read file
	file = fopen(FILENAME, "rb");
	fseek(file, SIZE_BOOTBLOCK, SEEK_CUR); // skip boot sector
	fseek(file, SIZE_SUPERBLOCK * sizeof(Byte), SEEK_CUR);     // skip superblock sector
	fseek(file, SIZE_INODELIST * sizeof(InodeList), SEEK_CUR); // skip superblock sector
	// I recommend you to use the form of "SIZE * sizeof its unit" becauseof consistency of the whole code.

	fseek(file, sizeof(DataBlock) * Address, SEEK_CUR); // The first Block Address starts at 0.
	fread(&result, sizeof(DataBlock), 1, file); // read

	fclose(file);
	return result;
}
