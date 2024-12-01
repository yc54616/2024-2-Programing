#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "system/file_system.h"
#include "system/io_stream.h"
#include "system/data_struct.h"

time_t getCurTime();
int getNeededDirectAdressNumber(char*);
int allocateInodeForNewFiles(char* , int , int, int);
int allocateSingleIndirectBlock();
void writeFileContents(char* , int , int );
char getAllDirectAdressWithSourceFileName(char* , int );
char* getFileContentsWithSourceFileName(char* , bool);
void mycat(char**);
void myshowfile(char** );
void mycp(char** );
void mycpto(char** );
void mycpfrom(char**);