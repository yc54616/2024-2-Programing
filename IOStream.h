#ifndef __IO_STREAM__
#	define __IO_STREAM__
#	include "filesystem.h"
SuperBlock getSuperBlock();
InodeList getInodeList(int Inode);
DataBlock getDataBlock(int Address);
#endif
