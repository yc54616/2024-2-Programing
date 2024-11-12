#ifndef __COMMANDS2_H__
#	define __COMMANDS2_H__ 1
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>

#	include "data_struct.h"
#	include "file_system.h"
#	include "io_stream.h"

/* functions */
void mytouch(char **commands);
void mymkdir(char **commands);
void myrmdir(char **commands);
void myls(char **commands);
void mypwd(char **commands);
void mycd(char **commands);
void mytree(char **commands);

#endif
