#include <stdio.h>
#include <stdlib.h> // to use strtoi

#include "file_system.h"
#include "io_stream.h"

#ifndef __FS_STATUS_PRINT__
#	define __FS_STATUS_PRINT__
#	define errmsg printf
void myinode(char **command);
void mydatablock(char **command);
void mystatus(char **command);
#endif
