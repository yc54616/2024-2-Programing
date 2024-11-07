#ifndef __COMMANDS2_H__
#	define __COMMANDS2_H__ 1
#	include <stdio.h>
#	include <stdlib.h> // for dynamic memory allocating
#	include <string.h> // for using strncpy

#	include "file_system.h"
#	include "io_stream.h"
#	include "data_struct.h"

/* functions */
void mypwd(char **commands);
void mycd(char **commands);
void mytree(char **commands);
/* To move the first-second parameter-what you want to move
 * return value case
 * 0 : failed to move ( such directory doesn't exist )
 * 1 : successfully moved
 */
int cd(chainedDirectory **top, int *cnt, unsigned char *path);
#endif
