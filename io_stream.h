#ifndef __IO_STREAM_H__
#	define __IO_STREAM_H__ 1
/* They get nothing, or number refers to the sector
 * read each sector what the programmer want from "myfs" file(more exactly from permanent memory)
 * return the object(structed memory)
 */
#	include <stdbool.h>

#	include "file_system.h"
unsigned char findEmptySuperBlock();
unsigned char findEmptyInode();
unsigned char findEmptyDataBlock();
void writeDirectoryDataBlock(char name[], int datablockIndex, int startIndex);
void setBit(Byte *byte, int index, bool bit);
void initFilesystem();
void setFilesystem(SuperBlock sb, InodeList in[SIZE_INODELIST], DataBlock db[SIZE_DATABLOCK]);
void setSuperBlock(int bitIndex, bool bit);
void setInodeList(int index, bool type, bool dAndI, time_t date, unsigned int size, unsigned char address[6]);
void setDataBlock(int address, Byte content_bin[SIZE_DATABLOCK]);
SuperBlock getSuperBlock();
InodeList getInodeList(int Inode);
DataBlock getDataBlock(int Address);
#endif
