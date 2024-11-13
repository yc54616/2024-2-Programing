#ifndef __DATA_STRUCT_H__
#	define __DATA_STRUCT_H__ 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h> // temporary

#include "file_system.h"
#include "io_stream.h"

#define errmsg printf

typedef struct chainedDirectory {
	unsigned char my_name[7];
	unsigned char my_inode_number;

	struct chainedDirectory *parent;
} chainedDirectory; // It has parent pointing to its parent, .. goes on the end, finally we can arrives at the root.    
typedef void (*func)(char **);
struct FunctionCallByString
{
	/**
	@brief 함수 호출을 입력받은 문자열을 통해 하기 위해 사용한다.
	name은 명령어(함수)의 이름(문자열), command는 명령어(함수) 호출이다.
	name == #command
	*/
	char name[30];
	func command;
};

/* functions */
unsigned char findDictoryNameToInode(char * argument);
int compare_directory_names(unsigned char *, unsigned char *);
int getExistence(unsigned char *);
/* To move the first-second parameter-what you want to move
 * return value case
 * 0 : failed to move ( such directory doesn't exist )
 * 1 : successfully moved
 */
int _mycd(chainedDirectory **, int *, unsigned char *);
void copyWorkingDirectory(chainedDirectory **);
/* Used to free the stack */
void clearVWD(chainedDirectory **, int);
/* To move the first-second parameter-what you want to move
 * return value case
 * 0 : failed to move ( such directory doesn't exist )
 * 1 : successfully moved
 */
int cd(chainedDirectory **, int *, unsigned char *);

void myinode(char **command);
void mydatablock(char **command);
void mystatus(char **command);
#endif
