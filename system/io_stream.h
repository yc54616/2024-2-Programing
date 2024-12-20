#ifndef __IO_STREAM_H__
#	define __IO_STREAM_H__ 1
/* They get nothing, or number refers to the sector
 * read each sector what the programmer want from "myfs" file(more exactly from permanent memory)
 * return the object(structed memory)
 */
#	include <stdbool.h>

#	include "file_system.h"
int dataBlockPossible(int host_txt_size);
int howUseDataBlockInode();
int findEmptyInode();
int findEmptyDataBlock();
void writeIndirectDataBlock(int address, int datablockIndex);
void deleteDirectoryDataBlock(int datablockIndex, int startIndex);
void writeDirectoryDataBlock(char name[], int datablockIndex, int startIndex);
void setBit(Byte *byte, int index, bool bit);
void initFilesystem();
void setFilesystem(SuperBlock sb, InodeList in[SIZE_INODELIST], DataBlock db[SIZE_DATABLOCK]);
void setSuperBlock(int bitIndex, bool bit);
void initInodeList(int index);
void setInodeList(int index, bool file_mode, time_t access_date, time_t birth_date, unsigned int size, unsigned char reference_count, unsigned char *direct_address, unsigned char single_indirect_address);
void initDataBlock(int address);
void setDataBlock(int address, unsigned char *contents);
SuperBlock getSuperBlock();
InodeList getInodeList(int Inode);
DataBlock getDataBlock(int Address);
#endif
