#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "system/file_system.h"
#include "system/io_stream.h"
#include "system/data_struct.h"

int getNeededDirectAdressNumber(char*);
int allocateInodeForNewFiles(char*, int);
void writeFileContents(char* , int , int );
char getAllDirectAdressWithSourceFileName(char* , int );
char* getFileContentsWithSourceFileName(char* );