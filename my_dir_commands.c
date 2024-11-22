#include "my_dir_commands.h"
#include <time.h>
#include <unistd.h>

#define LAST_FILE 1
#define DEPTH_START -1

/* declarations of global variable */
extern chainedDirectory *working_directory;
extern int depth_working_directory;

/* It seems like _mycd */
static void _Tree(unsigned char *, unsigned char, int, char *, int);

void mymkfs(char **commands){
	char check[100] = "y";
	bool flag = 0;
	if((access(FILENAME, 0) != -1)){
		printf("파일시스템이 있습니다. 다시 만들겠습니까? (y/n) ");
		fgets(check, sizeof(check), stdin);
		check[1] = 0;
		flag = 1;
	}
	else{
		printf("파일시스템이 없습니다. 파일시스템을 만듭니다.\n");
	}

	if(check[0] == 'y'){
		if(flag)
			printf("파일시스템을 다시 만들었습니다.\n");
		initFilesystem();
		setSuperBlock(1, 1);
		setSuperBlock(SIZE_INODELIST + 1, 1);
		time_t curTime;
		time(&curTime);
		unsigned char address[8] = {0,};
		setInodeList(1, DIRECTORY, curTime, curTime, 16, 1, address, 0);
		char *str = ".\x00\x00\x00\x00\x00\x00\x01";
		writeDirectoryDataBlock(str, 0, 0);
		str = "..\x00\x00\x00\x00\x00\x01";
		writeDirectoryDataBlock(str, 0, 8);
	}
}

void mytouch(char **commands){
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
	char str[8] = {0};
	int inode_number;
	strcpy(str,argument);
	if((strlen(str) == 1 && str[0] == '.') || (strlen(str) == 2 && str[0] == '.' && str[1] == '.')){
		printf(". or .. directory can't be created..\n");
		return;
	}
	inode_number = findDictoryNameToInode(str);
	if(inode_number != 0){
		printf("FIND!!\n");
		printf("%d\n",inode_number);
	};

    time_t curTime;
    InodeList inode_list;

    time(&curTime);
    if (inode_number > 0)//파일이 존재할 때
    {
    	inode_list = getInodeList(inode_number);
    	inode_list.access_date = curTime;
		setInodeList(inode_number, inode_list.file_mode, inode_list.access_date, inode_list.birth_date, inode_list.size, inode_list.reference_count, inode_list.direct_address, inode_list.single_indirect_address);
    }
    else
    {
    	inode_number = findEmptyInode();
		int dataBlock_num = findEmptyDataBlock();
		unsigned char address[8] = {dataBlock_num,};

		setSuperBlock(inode_number, 1);
		setSuperBlock(SIZE_INODELIST + inode_number, 1);

    	setInodeList(inode_number, GENERAL, curTime, curTime, 0, 1, address, 0);

		str[7] = inode_number;

		inode_list = getInodeList(working_directory -> my_inode_number);//파일 새로 만들기
		writeDirectoryDataBlock(str, working_directory -> my_inode_number - 1, inode_list.size);
		setInodeList(working_directory->my_inode_number, DIRECTORY, curTime, inode_list.birth_date, inode_list.size+8, inode_list.reference_count, inode_list.direct_address, inode_list.single_indirect_address);

	}
	
}

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

	printf("workding d : %d\n", working_directory->my_inode_number);

	unsigned char *names = calloc(sizeof(unsigned char), 8);
	strcpy(names, argument);
	printf(">> %s\n", names);

	int inode_number = findDictoryNameToInode(names);

	if((strlen(names) == 1 && names[0] == '.') || (strlen(names) == 2 && names[0] == '.' && names[1] == '.')){
		printf(". or .. directory can't be created..\n");
		free(names);
		return;
	}

	if(inode_number != 0){
		printf("Existence directory.. or file..\n");
		printf("FIND!!\n");
		printf("%d\n",inode_number);
		free(names);
		return;
	};

	int useInode = findEmptyInode();
	int useDataBlock = findEmptyDataBlock();

	printf(">> useInode : %d, useDataBlock %d\n", useInode, useDataBlock);

	names[7] = useInode;

	InodeList curDictory = getInodeList(working_directory->my_inode_number);
	time_t curTime;
	time(&curTime);
	setSuperBlock(useInode, 1);
	setSuperBlock(SIZE_INODELIST+useInode, 1);
	unsigned char address[8] = {useDataBlock,};
	setInodeList(useInode, DIRECTORY, curTime, curTime, 8 * 2, 1, address, 0);
	unsigned char *str = calloc(sizeof(unsigned char), 8);
	strcpy(str, ".");
	str[7] = useInode;
	writeDirectoryDataBlock(str, useDataBlock, 0);
	memset(str, 0, sizeof(str));
	strcpy(str, "..");
	str[7] = working_directory->parent->my_inode_number;
	writeDirectoryDataBlock(str, useDataBlock, 8);

	writeDirectoryDataBlock(names, working_directory->my_inode_number-1, curDictory.size);
	printf("%d", curDictory.size);
	setInodeList(working_directory->my_inode_number, DIRECTORY, curTime, curDictory.birth_date, curDictory.size+8, curDictory.reference_count, curDictory.direct_address, curDictory.single_indirect_address);

	free(names);
}

void myrmdir(char **commands)
{
    char *argument = commands[1];

	if (commands[1] == NULL)
        return;

    int inode_number = findDictoryNameToInode(argument);
    
    if (inode_number == 0)
        return;

    InodeList inode_list = getInodeList(inode_number);
	DataBlock curDatablock = getDataBlock(inode_number- 1);

    if (inode_list.file_mode != DIRECTORY)
        return;

    // 디렉토리가 비어있는지 확인
    if (inode_list.size > 16)
    {
        printf("Error: Directory '%s' is not empty.\n", argument);
        return;
    }

	setInodeList(working_directory -> my_inode_number, 0, 0, 0, 0, 0, 0, 0);

	/*for (int i = 0; i < 32; i++)
	{
		curDatablock.subfiles[i] = 0;
	}

	setSuperBlock(inode_number, 0);
	setSuperBlock(SIZE_INODELIST + inode_number, 0);*/
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

	// inode = getInodeList(working_directory->my_inode_number);
	// pt = localtime(&inode.access_date);
	// printf("%d/%d/%d %d:%d:%d  ", (pt->tm_year + 1900), (pt->tm_mon), (pt->tm_mday), (pt->tm_hour), (pt->tm_min), (pt->tm_sec));
	// printf("%s\t", inode.file_mode == 1 ? "directory" : "file");
	// printf("%d\t", working_directory->my_inode_number);
	// printf("%d byte  .\n", inode.size);
	// inode = getInodeList(working_directory->parent->my_inode_number);
	// pt = localtime(&inode.access_date);
	// printf("%d/%d/%d %d:%d:%d  ", (pt->tm_year + 1900), (pt->tm_mon), (pt->tm_mday), (pt->tm_hour), (pt->tm_min), (pt->tm_sec));
	// printf("%s\t", inode.file_mode == 1 ? "directory" : "file");
	// printf("%d\t", working_directory->parent->my_inode_number);
	// printf("%d byte  ..\n", inode.size);

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
			printf("%c", *(data_block.contents+j));
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
	 *
	 * so, we need to print them in descending order
	 */
	for (i = depth_working_directory - 1; i >= 0; i--)
		printf("/%s", *(directory_names + i));
	printf("\n");

	free(directory_names);
}

/* Very simple code to understand */
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
	free(path);
}

void mytree(char **commands)
{
	char *argument = commands[1];
	unsigned char c;
	int length;
	unsigned char *path = NULL;
	char *inheriting_string = (char *) malloc(sizeof(char)); // This will appear in _Tree with same usage. If you want to know more detailed usage, then you can go _Tree function and can see it.

	*inheriting_string = '\0'; // Starts "";
	if (argument == NULL) { // typed '$ mycd'
		printf(".");
		_Tree(NULL, working_directory -> my_inode_number, DEPTH_START, inheriting_string, LAST_FILE);
	} else {

		for (length = 0; (c = argument[length]) != '\0'; length++)
			;
		path = (unsigned char *)malloc(sizeof(unsigned char) * length);
		strcpy(path, argument);

		if (getExistence(path)) {
			chainedDirectory *virtual_working_directory;
			int virtual_depth_working_directory = depth_working_directory;

			copyWorkingDirectory(&virtual_working_directory);
			cd(&virtual_working_directory, &virtual_depth_working_directory, path);
			printf("%s", path);
			_Tree(NULL, virtual_working_directory -> my_inode_number, DEPTH_START, inheriting_string, LAST_FILE);
			clearVWD(&virtual_working_directory, virtual_depth_working_directory);
		} else
			printf("Such directory doesn'y exist\n");
		free(path);
	}
}

/* It's for qsort */
int _compare_files(const void *file1, const void *file2)
{
	return strncmp((unsigned char *) file1, (unsigned char *) file2, 7);
}

static void _Tree(unsigned char *name, unsigned char inode_number, int depth, char *inherited_string, int last_file) // It may be frequently and reculsively called.
{
	InodeList inode_list = getInodeList(inode_number);
	int i;
	char c;

	// printing myself according to depth and last file
	printf("%s", inherited_string);
	if (depth != DEPTH_START) { // This directory is the direct child of the (virtual) working directory.
		if (last_file == LAST_FILE) // If this is the last child of (v).w.d,
			printf("└── ");
		else
			printf("├── ");
		/* Printing my name */
		for (i = 0; i < 7; i++) {
			c = *(name + i);
			if (c == '\0')
				break;
			printf("%c", c);
		}
	}
	printf("\n");

	if (inode_list.file_mode == DIRECTORY) {
		// using variable scope
		int count_files = inode_list.size / 8;
		unsigned char (*properties_of_children)[8] = (unsigned char (*)[8]) malloc(sizeof(unsigned char (*)[8]) * count_files); // We will make this sorted.
		DataBlock data_block;
		DataBlock indirect_block;
		int number_of_using_pointers;
		int count_listed_files = 0;
		int j;
		int remaining_space = count_files % 32; // It's used for for-loop, that treats the left space without fully using data block.
		char *inheriting_string; // In fact, that was a fool. If you are a programmer, you can easily guess the meaning of it, and that guessing may be correct.
		
		if (inode_list.reference_count == 9) { // If it uses a single indirect pointer
			indirect_block = getDataBlock(inode_list.single_indirect_address);
			number_of_using_pointers = indirect_block.contents[0];
			/* looping in the indirect block */
			for (i = 1; i < number_of_using_pointers - 1; i++) { // without the last data block which generally does not take up the entire space
				data_block = getDataBlock(indirect_block.contents[i]);
				for (j = 0; j < 32; j++) {
					strncpy(*(properties_of_children + count_listed_files), data_block.subfiles[j], 7);
					*(*(properties_of_children + count_listed_files) + 7) = data_block.subfiles[j][7];
					count_listed_files++;
				}
			}
			/* Processing the one remaining data block, which generally does not take up the entire space */
			data_block = getDataBlock(indirect_block.contents[i]);
			for (j = 0; j < remaining_space; j++) {
				strncpy(*(properties_of_children + count_listed_files), data_block.subfiles[j], 7);
				*(*(properties_of_children + count_listed_files) + 7) = data_block.subfiles[j][7];
				count_listed_files++;
			}
		} else {
			for (i = 0; i < inode_list.reference_count - 1; i++) {
				data_block = getDataBlock(inode_list.direct_address[i]);
				for (j = 0; j < 32; j++) {
					strncpy(*(properties_of_children + count_listed_files), data_block.subfiles[j], 7);
					*(*(properties_of_children + count_listed_files) + 7) = data_block.subfiles[j][7];
					count_listed_files++;
				}
			}
			data_block = getDataBlock(inode_list.direct_address[i]);
			for (j = 0; j < remaining_space; j++) {
				strncpy(*(properties_of_children + count_listed_files), data_block.subfiles[j], 7);
				*(*(properties_of_children + count_listed_files) + 7) = data_block.subfiles[j][7];
				count_listed_files++;
			}
		}

		qsort(properties_of_children, count_files, sizeof(unsigned char *), _compare_files);
		
		if (count_files != 2) {
			depth++;
			inheriting_string = (char *) malloc(sizeof(char) * (4 * depth + 1));
			strcpy(inheriting_string, inherited_string);
			if (depth != DEPTH_START + 1) {
				if (last_file == LAST_FILE)
					strcat(inheriting_string, "    ");
				else
					strcat(inheriting_string, "│   ");
			}
			for (i = 2; i < count_files - 1; i++)
				_Tree(*(properties_of_children + i), *(*(properties_of_children + i) + 7), depth, inheriting_string, !LAST_FILE);
				_Tree(*(properties_of_children + i), *(*(properties_of_children + i) + 7), depth, inheriting_string, LAST_FILE);
			free(inheriting_string);
		}
		free(properties_of_children);
	}
}
