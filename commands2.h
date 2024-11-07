#ifndef __COMMANDS2_H__
#	define __COMMANDS2_H__ 1
#	include <stdio.h>
#	include <stdlib.h> // for dynamic memory allocating
#	include <string.h> // for using strncpy

#	include "data_struct.h"
#	include "file_system.h"
#	include "io_stream.h"

/* functions */
void mypwd(char **commands);
void mycd(char **commands);
void mytree(char **commands);
#endif
