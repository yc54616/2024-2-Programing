#include "commands2.h"
#include <time.h>

/* declarations of global variable */
extern chainedDirectory *working_directory;
extern int depth_working_directory;

void mymkdir(char **commands)
{
	if(commands[1] == NULL){
		printf("Params Empty..");
		return;
	}
	char *argument = commands[1];
	int len = strlen(argument);
	if(len < 1 || len > 7){
		printf("names too long..\n");
		return;
	}

	unsigned char *names = calloc(sizeof(unsigned char), 8);
	strcpy(names, argument);
	printf(">> %s", names);

	int useInode = findEmptyInode();
	int useDataBlock = findEmptyDataBlock();

	names[7] = useInode;

	InodeList curDictory = getInodeList(working_directory->my_inode_number);

	time_t curTime;
	time(&curTime);
	setSuperBlock(useInode, 1);
	setSuperBlock(SIZE_INODELIST+useInode, 1);
	unsigned char address[8] = {useDataBlock,};
	setInodeList(useInode, DIRECTORY, curTime, curTime, 8 * 2, 1, address, 0);

	writeDirectoryDataBlock(names, working_directory->my_inode_number-1, curDictory.size-16);
	setInodeList(working_directory->my_inode_number, DIRECTORY, curTime, curDictory.birth_date, curDictory.size+8, curDictory.reference_count, curDictory.direct_address, curDictory.single_indirect_address);

	free(names);
}

void myls(char **commands)
{
	DataBlock data_block = getDataBlock(working_directory->my_inode_number - 1);
	InodeList inode;
	struct tm *pt;
	int *inodelist = NULL;

	int cnt = 0;
	for (int i = 0; i < sizeof(DataBlock); i++)
	{
		if ((i + 1) % 8 == 0 && data_block.contents[i] != 0)
		{
			cnt++;
			inodelist = (int *)realloc(inodelist, sizeof(int) * cnt);
			*(inodelist + (cnt - 1)) = *(data_block.contents + i);
		}
	}

	inode = getInodeList(working_directory->my_inode_number);
	pt = localtime(&inode.access_date);
	printf("%d/%d/%d %d:%d:%d  ", (pt->tm_year + 1900), (pt->tm_mon), (pt->tm_mday), (pt->tm_hour), (pt->tm_min), (pt->tm_sec));
	printf("%s\t", inode.file_mode == 1 ? "directory" : "file");
	printf("%d\t", working_directory->my_inode_number);
	printf("%d byte  .\n", inode.size);
	inode = getInodeList(working_directory->parent->my_inode_number);
	pt = localtime(&inode.access_date);
	printf("%d/%d/%d %d:%d:%d  ", (pt->tm_year + 1900), (pt->tm_mon), (pt->tm_mday), (pt->tm_hour), (pt->tm_min), (pt->tm_sec));
	printf("%s\t", inode.file_mode == 1 ? "directory" : "file");
	printf("%d\t", working_directory->parent->my_inode_number);
	printf("%d byte  ..\n", inode.size);

	for (int i = 0; i < cnt; i++)
	{
		inode = getInodeList(*(inodelist + i));
		pt = localtime(&inode.access_date);
		printf("%d/%d/%d %d:%d:%d  ", (pt->tm_year + 1900), (pt->tm_mon), (pt->tm_mday), (pt->tm_hour), (pt->tm_min), (pt->tm_sec));
		printf("%s\t", inode.file_mode == 1 ? "directory" : "file");
		printf("%d\t", *(inodelist + i));
		printf("%d byte  ", inode.size);
		for (int j = i * 8; j < i * 8 + 7; j++)
		{
			printf("%c", data_block.contents[j]);
		}
		printf("\n");
	}
	free(inodelist);
}

void mypwd(char **commands)
{
	int i, j;
	chainedDirectory *current_directory;
	unsigned char(*directory_names)[8];

	if (depth_working_directory == 0) // Exception
		printf("/");

	directory_names = (unsigned char(*)[8])malloc(sizeof(unsigned char(*)[8]) * depth_working_directory); // from the global field in the shell.h
	current_directory = working_directory;
	for (i = 0; i < depth_working_directory; i++)
	{
		for (j = 0; j < 7; j++)
			*(*(directory_names + i) + j) = *(current_directory->my_name + j);
		*(*(directory_names + i) + j) = '\0'; // j == 7 so [8]
		current_directory = current_directory->parent;
	}

	/* now, directory_name consists of all directories that reach the working directory from the root in descending order
	 * ex) /a/b/c/d
	 * => d c b a
	 */

	/* so, we need to print them in descending order */
	for (i = depth_working_directory - 1; i >= 0; i--)
		printf("/%s", *(directory_names + i));
	printf("\n");

	free(directory_names);
}

void mycd(char **commands)
{
	char *argument = commands[1];
	unsigned char c;
	int length;
	unsigned char *path = NULL;

	if (argument != NULL)
	{
		for (length = 0; (c = argument[length]) != '\0'; length++)
			;
		path = (unsigned char *)malloc(sizeof(unsigned char) * length);
		strcpy(path, argument);
	}
	if (cd(&working_directory, &depth_working_directory, path) == 0)
	{ // If failed cd,
		printf("Such directory doesn't exist\n");
	}
	// free(path);
}

void mytree(char **commands)
{
}
