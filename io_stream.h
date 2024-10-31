#include "file_system.h"

#ifndef __IO_STREAM__
#	define __IO_STREAM__
/* They get nothing, or number refers to the sector
 * read each sector what the programmer want from "myfs" file(more exactly from permanent memory)
 * return the object(structed memory)
 */
SuperBlock getSuperBlock(void);
InodeList getInodeList(int Inode);
DataBlock getDataBlock(int Address);
#endif
