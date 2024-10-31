#include "file_system.h"

#ifndef __IO_STREAM__
#	define __IO_STREAM__
SuperBlock getSuperBlock();
InodeList getInodeList(int Inode);
DataBlock getDataBlock(int Address);
#endif
