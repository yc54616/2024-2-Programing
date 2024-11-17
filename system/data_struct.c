/* This file treats
 * 1. working_directory stack and its virtual version
 * 2. the function for directory
 */
#include "data_struct.h"
#include  <malloc.h>

/* definition of global variable */
chainedDirectory *working_directory;
int depth_working_directory;


int getDirectoryInodeNumber() {
	int result = working_directory->my_inode_number;
	return result;
}

unsigned char findDictoryNameToInode(char *argument)
{
	char names[8];
	strcpy(names, argument);
	DataBlock data_block = getDataBlock(working_directory->my_inode_number - 1);
	InodeList inodelist = getInodeList(working_directory->my_inode_number);
	
	char compareStr[8];

	for(int i = 2; i < (inodelist.size)/8; i++)
	{
		memset(compareStr, 0, sizeof(compareStr));
		for (int j = 0; j < 7; j++)
		{
			compareStr[j] = data_block.contents[i*8+j];
		}
		if (strcmp(compareStr, names) == 0){
			return data_block.contents[i*8+7];
		}
	}

	return 0;
}

/* It has quite simple logic, is similar as strcmp, but we need this because
 * If directory name length is 7, then strcmp can't think that where it ends since '\0' doesn't exist in 7 digits array.
 * Therefore we need some special function like that below.
 * Especially, it needs to check for the situation when the length of name of directory is 7.
 */
int compare_directory_names(unsigned char *dir1, unsigned char *dir2)
{
	int i;
	unsigned char c1, c2;

	for (i = 0; i < 7; i++)
	{
		c1 = *(dir1 + i);
		c2 = *(dir2 + i);
		if (c1 != c2)
			return 0;
		else if (c1 == '\0') // Already checked that c1 == c2.
			break;
	}
	return 1;
}
/* Algorithm's below
 * from i = 0 to 7(Max length of directory name)
 * in i 'th place :
 * 	1. If different characters exist in the same place before the comparison => They are different.
 *	2. (else) If this place is the end of the string on both sides => They are the same. ( close for-loop )
 *	( Why we break this comparison is that there can exist different strings next to its ending )
 *	ex) {'a', 'b', '\0', 32, ...} == {'a', 'b', '\0', 123, ...} but 32 != 123
 * Before it, we saw all posibillities.
 */

/* This function is used for checking the existence of directory exactly in the path from working directory.*/
int getExistence(unsigned char *path)
{
	int i;
	chainedDirectory temp_CD;
	chainedDirectory *temp_CD_ptr;
	chainedDirectory *virtual_working_directory; // Virtually we use it alternately instead of virtual working directory.
	int virtual_depth_working_directory;		 // Similar as above.
	int written_characters = 0;
	char directory_name[7];
	char c;

	/* Coping real w.d into virtual w.d */
	copyWorkingDirectory(&virtual_working_directory); // We must free them. => using clearVWD before returning(function ending)
	virtual_depth_working_directory = depth_working_directory;

	if (path == NULL)
	{ // '$ mycd' => go to the root
		clearVWD(&virtual_working_directory, virtual_depth_working_directory);
		return 1; // The root always exists.
	}
	else
	{
		if (path[0] == '/')
		{ // If the path starts with the letter referring to the root
			while (virtual_depth_working_directory != 0)
				_mycd(&virtual_working_directory, &virtual_depth_working_directory, "..");
			written_characters++; // Counting for '/'
		}
		do
		{
			directory_name[0] = '\0';
			for (i = 0; i < 7; i++)
			{
				c = path[written_characters];
				written_characters++;
				if (c == '\0' || c == '/')
					break;
				else
					directory_name[i] = c;
			}
			if (i == 7)
			{
				c = path[written_characters];
				written_characters++;
				if (c != '\0' && c != '/')
				{ // the path ends neither '\0' nor '/' means user typed directory of which length is more than max length.
					clearVWD(&virtual_working_directory, virtual_depth_working_directory);
					return 0;
					// The length of the directory name field exceeds
				}
			}
			directory_name[i] = '\0';
			/* Actually changing directory */
			if (_mycd(&virtual_working_directory, &virtual_depth_working_directory, directory_name) == -1)
			{ // If there doesn't exist such directory
				clearVWD(&virtual_working_directory, virtual_depth_working_directory);
				return 0;
			}
		} while (c != '\0');
	}
	clearVWD(&virtual_working_directory, virtual_depth_working_directory);
	return 1;
}

/* Actually changing the directory, either real working directory or virtual */
int _mycd(chainedDirectory **top, int *cnt, unsigned char *directory_name)
{
	chainedDirectory *temp_CD_ptr;
	InodeList inode_list;
	DataBlock data_block;
	DataBlock indirect_block;
	int i, j;
	unsigned int remaining_space;			// It's used for for-loop that treats the left space without fully using data block
	unsigned char number_of_using_pointers; // If the directory use indirect pointer then, it has the number of using direct pointers in indirect block.

	if (compare_directory_names("..", directory_name))
	{ // if the parameter is ".."
		/* Predicting the problem when cd from root to .. */
		if (*cnt != 0)
		{
			temp_CD_ptr = *top;	   // Temporarilly saving for freeing dynamically allocated memory.
			*top = (*top)->parent; // Moving to parent ex) /a/b/c to /a/b
			free(temp_CD_ptr);	   // The programmer's duty.
			(*cnt)--;
		}
		return 0;
	}
	else if (compare_directory_names(".", directory_name))
		return 0;
	else if (compare_directory_names("", directory_name))
		return 0;
	else
	{
		inode_list = getInodeList((*top)->my_inode_number);
		if (inode_list.reference_count == 9)
		{ // If it uses a single indirect pointer
			indirect_block = getDataBlock(inode_list.single_indirect_address);
			number_of_using_pointers = indirect_block.contents[0];
			/* looping in the indirect block */
			for (i = 1; i < number_of_using_pointers - 1; i++)
			{
				data_block = getDataBlock(indirect_block.contents[i]);
				/* looping in the data block */
				for (j = 0; j < 32; j++)
				{
					if (compare_directory_names(data_block.subfiles[j], directory_name))
					{ // Found the file ( directory of general )
						if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY)
						{ // It was a directory
							temp_CD_ptr = (chainedDirectory *)malloc(sizeof(chainedDirectory));
							strncpy(temp_CD_ptr->my_name, directory_name, 7);
							temp_CD_ptr->my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
							temp_CD_ptr->parent = *top;

							*top = temp_CD_ptr;
							(*cnt)++;
							return 0;
						}
						else // This is not a directory.
							return -1;
					}
				}
			}
			/* Processing the one remaining data block, which generally does not take up the entire space */
			data_block = getDataBlock(indirect_block.contents[i]);
			remaining_space = inode_list.size % sizeof(DataBlock) / 8;
			/* looping in the data block */
			for (j = 0; j < remaining_space; j++)
			{
				if (compare_directory_names(data_block.subfiles[j], directory_name))
				{
					if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY)
					{
						temp_CD_ptr = (chainedDirectory *)malloc(sizeof(chainedDirectory));
						strncpy(temp_CD_ptr->my_name, directory_name, 7);
						temp_CD_ptr->my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
						temp_CD_ptr->parent = *top;

						*top = temp_CD_ptr;
						(*cnt)++;
						return 0;
					}
					else // This is not a directory
						return -1;
				}
			}

			return -1; // There is no such directory.
		}
		else
		{ // It uses direct pointer(s)
			/* looping in the direct pointers */
			for (i = 0; i < inode_list.reference_count - 1; i++)
			{
				data_block = getDataBlock(inode_list.direct_address[i]);
				/* looping in the data block */
				for (j = 0; j < 32; j++)
				{
					if (compare_directory_names(data_block.subfiles[j], directory_name))
					{ // Found the file ( directory of general )
						if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY)
						{ // It was a directory
							temp_CD_ptr = (chainedDirectory *)malloc(sizeof(chainedDirectory));
							strncpy(temp_CD_ptr->my_name, directory_name, 7);
							temp_CD_ptr->my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
							temp_CD_ptr->parent = *top;

							*top = temp_CD_ptr;
							(*cnt)++;
							return 0;
						}
						else // This is not a directory
							return -1;
					}
				}
			}
			/* Processing the one remaining data block, which generally does not take up the entire space */
			data_block = getDataBlock(inode_list.direct_address[i]);
			remaining_space = inode_list.size % sizeof(DataBlock) / 8;
			/* looping in the data block*/
			for (j = 0; j < remaining_space; j++)
			{
				if (compare_directory_names(data_block.subfiles[j], directory_name))
				{ // Found the file ( directory of general )
					if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY)
					{ // It was a directory
						temp_CD_ptr = (chainedDirectory *)malloc(sizeof(chainedDirectory));
						strncpy(temp_CD_ptr->my_name, directory_name, 7);
						temp_CD_ptr->my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
						temp_CD_ptr->parent = *top;

						*top = temp_CD_ptr;
						(*cnt)++;
						return 0;
					}
					else // This is not a directory
						return -1;
				}
			}
			return -1; // There is no such directory.
		}
	}
}
/* will use when we need to copy working directory */
void copyWorkingDirectory(chainedDirectory **top)
{
	int i;
	chainedDirectory *temp_CD_ptr;
	chainedDirectory *virtual_working_directory = working_directory;														   // It's not the copied.
	chainedDirectory **linked_directories = (chainedDirectory **)malloc(sizeof(chainedDirectory *) * depth_working_directory); // More detailed notion is described in the PDF, read it.

	for (i = 0; i < depth_working_directory; i++)
	{
		temp_CD_ptr = (chainedDirectory *)malloc(sizeof(chainedDirectory));
		strncpy(temp_CD_ptr->my_name, virtual_working_directory->my_name, 7);
		temp_CD_ptr->my_inode_number = virtual_working_directory->my_inode_number;
		*(linked_directories + i) = temp_CD_ptr;

		virtual_working_directory = virtual_working_directory->parent; // exploring
	}

	/* Now, the array consists of directories in descending order.
	 * ex)  /as/df/gh
	 *    => gh df as
	 * index  0  1  2
	 */

	/* root */
	temp_CD_ptr = (chainedDirectory *)malloc(sizeof(chainedDirectory));
	(temp_CD_ptr->my_name)[0] = '\0';
	temp_CD_ptr->my_inode_number = 1;
	temp_CD_ptr->parent = temp_CD_ptr;

	for (i = 0; i < depth_working_directory - 1; i++)
	{
		(*(linked_directories + i))->parent = *(linked_directories + i + 1);
	}
	if (depth_working_directory == 0)
		*top = temp_CD_ptr;
	else
	{
		(*(linked_directories + i))->parent = temp_CD_ptr;
		*top = *linked_directories;
		free(linked_directories);
	}
	/* We perfectly copied. */
}
/* free- */
void clearVWD(chainedDirectory **top, int cnt)
{
	int i;
	chainedDirectory *temp_CD_ptr;

	for (i = 0; i < cnt; i++)
	{
		temp_CD_ptr = (*top)->parent;
		free(*top);
		*top = temp_CD_ptr;
	}
	free(*top); // frees the root
}

int cd(chainedDirectory **top, int *cnt, unsigned char *path)
{
	int i;
	int written_characters = 0;
	char directory_name[7];
	char c;
	/* We have to use the variable below, since
	 * the path can have nothing
	 */
	if (path == NULL)
	{ // '$ mycd' => go to the root
		while (*cnt != 0)
		{
			_mycd(top, cnt, "..");
		}
		written_characters++; // Counting for '/'
	}
	else if (getExistence(path))
	{

		if (path[0] == '/')
		{ // If the path starts with the letter referring to the root
			while (*cnt != 0)
				_mycd(top, cnt, "..");
			written_characters++; // Counting for '/'
		}
		do
		{
			directory_name[0] = '\0';
			for (i = 0; i < 7; i++)
			{
				c = path[written_characters];
				written_characters++;
				if (c == '\0' || c == '/')
					break;
				else
					directory_name[i] = c;
			}
			if (i == 7)
				written_characters++;
			directory_name[i] = '\0';
			/* Actually changing directory */
			_mycd(top, cnt, directory_name); // If there doesn't exist such directory
		} while (c != '\0');
	}
	else
		return 0;
	return 1;
}

void myinode(char **commands)
{
	int inode_number;
	InodeList inode_list;
	char c;
	int i;
	int direct_pointer_count;
	
	if (commands[1] == NULL) {
		errmsg("The first argument must be filled!\n");
		return;
	}

	for (i = 0; (c = commands[1][i]) != '\0'; i++) {
		if (c < '0' || c > '9') {
			errmsg("The first arg must be positive integer format!\n");
			return;
		}
	}

	if (i == 0) {
		errmsg("The first arg must be filled!\n");
		return;
	}

	inode_number = strtol(commands[1], NULL, 10);
	if (! (inode_number >= 1 && inode_number <= SIZE_INODELIST)) {
		errmsg("The first arg must be in the (closed) section-[1, %d]!\n", SIZE_INODELIST);
		return;
	}

	inode_list = getInodeList(inode_number);
	direct_pointer_count = (inode_list.reference_count > SIZE_DIRECT_POINTER) ? SIZE_DIRECT_POINTER : inode_list.reference_count;
	printf("종류 : %s\n", (inode_list.file_mode == DIRECTORY) ? "디렉토리" : "파일");
	printf("생성일자 : %ld\n", inode_list.birth_date);
	printf("크기 : %d\n", inode_list.size);
	printf("직접 블럭 목록 :\n");
	for (i = 0; i < direct_pointer_count; i++)
		printf("  #%d 직접 데이터 블록 : %d\n", i, inode_list.direct_address[i]);
	printf("간접 블록 번호 : %d\n", inode_list.single_indirect_address);
}
void mydatablock(char **commands) {
	int block_address;
	DataBlock data_block;
	char c;
	int i;
	
	if (commands[1] == NULL) {
		errmsg("The first argument must be filled!\n");
		return;
	}

	for (i = 0; (c = commands[1][i]) != '\0'; i++) {
		if (c < '0' || c > '9') {
			errmsg("The first arg must be positive integer format!\n");
			return;
		}
	}

	if (i == 0) {
		errmsg("The first arg must be filled!\n");
		return;
	}

	block_address = strtol(commands[1], NULL, 10);
	if (! (block_address >= 0 && block_address <= SIZE_DATABLOCK - 1)) {
		errmsg("The first arg must be in the (closed) section-[0, %d]!\n", SIZE_DATABLOCK - 1);
		return;
	}
	
	data_block = getDataBlock(block_address);
	for (i = 0; i < sizeof(data_block); i++)
		printf("%c", data_block.contents[i]);
	printf("\n");
}
void mystatus(char **commands) {
	SuperBlock super_block;
	int i, j, k;
	int index;
	int loop_size;
	int used;
	Byte * map;
	
	super_block = getSuperBlock();
	used = 0;
	map = (Byte *)malloc(sizeof(Byte) * SIZE_INODELIST_IN_SUPERBLOCK);
	for (i = 0; i < SIZE_INODELIST_IN_SUPERBLOCK; i++) {
		for (j = 0; j < 8; j++) {
			map[i].for_shift <<= 1;
			map[i].last_bit = super_block.inode_list[i].first_bit;
			if (super_block.inode_list[i].first_bit)
				used++;
			super_block.inode_list[i].for_shift <<= 1;
		}
	}

	printf("Inode state : \n");
	printf("    Total : %d\n", SIZE_INODELIST);
	printf("    Used : %d\n", used);
	printf("    Available : %d\n", SIZE_INODELIST - used);
	printf("    Inode Map :\n");
	index = 0;
	for (i = 0; i < 2; i++) {
		printf("        ");
		for (j = 0, loop_size = SIZE_INODELIST_IN_SUPERBLOCK / 2; j < loop_size; j++) {
			for (k = 0; k < 4; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			for ( ; k < 8; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			index++;
		}
		printf("\n");
	}
	free(map);

	used = 0;
	map = (Byte *)malloc(sizeof(Byte) * SIZE_DATABLOCK_IN_SUPERBLOCK);
	for (i = 0; i < SIZE_DATABLOCK_IN_SUPERBLOCK; i++) {
		for (j = 0; j < 8; j++) {
			map[i].for_shift <<= 1;
			map[i].last_bit = super_block.data_block[i].first_bit;
			if (super_block.data_block[i].first_bit)
				used++;
			super_block.data_block[i].for_shift <<= 1;
		}
	}
	printf("\n");

	printf("Data Block state : \n");
	printf("    Total : %d blocks / %ld bytes\n", SIZE_DATABLOCK, SIZE_DATABLOCK * sizeof(DataBlock));
	printf("    Used : %d / %ld bytes\n", used, used * sizeof(DataBlock));
	printf("    Available : %d / %ld bytes\n", SIZE_DATABLOCK - used, ( SIZE_DATABLOCK - used ) * sizeof(DataBlock));
	printf("    Data Block Map :\n");
	index = 0;
	for (i = 0; i < 4; i++) {
		printf("        ");
		for (j = 0, loop_size = SIZE_DATABLOCK_IN_SUPERBLOCK / 4; j < loop_size; j++) {
			for (k = 0; k < 4; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			for ( ; k < 8; k++) {
				printf("%d", map[index].first_bit);
				map[index].for_shift <<= 1;
			}
			printf(" ");
			index++;
		}
		printf("\n");
	}
	printf("\n");
	free(map);
}

