#include "file_system.h"

#ifndef __IO_STREAM__
#	define __IO_STREAM__
#   include <stdbool.h>
void setBit(Byte *byte, int index, bool bit);
void initFilesystem();
void setFilesystem(SuperBlock sb, InodeList in[SIZE_INODELIST], DataBlock db[SIZE_DATABLOCK]);
void setSuperBlock(int bitIndex, bool bit);
void setInodeList(int index, bool file_mode, time_t access_date, time_t birth_date, unsigned int size, unsigned char reference_count, unsigned char *direct_address, unsigned char single_indirect_address);
void setDataBlock(int address, unsigned char *contents);
SuperBlock getSuperBlock();
InodeList getInodeList(int Inode);
DataBlock getDataBlock(int Address);
#endif
