#include "file_system.h"

#ifndef __IO_STREAM__
#	define __IO_STREAM__
/* They get nothing, or number refers to the sector
 * read each sector what the programmer want from "myfs" file(more exactly from permanent memory)
 * return the object(structed memory)
 */
#   include <stdbool.h>
#	include "filesystem.h"
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
