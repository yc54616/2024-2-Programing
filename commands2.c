#include "commands2.h"

/* declarations of global variable */
extern chainedDirectory working_directory;
extern int depth_working_directory;

static int compare_directory_names(unsigned char *, unsigned char *);
/* It can drive at a directly linked directory
   It returns status code ) 0 -> changed successfully, -1 -> there occurs an error ( typically, there doesn't exist such directory )
 */
static int _mycd(unsigned char *);

void mypwd(char **commands)
{
	int i, j;
	chainedDirectory current_directory;
	unsigned char (*directory_names)[8];

	directory_names = (unsigned char (*)[8]) malloc(sizeof(unsigned char (*)[8]) * depth_working_directory); // from global field in shell.h
	current_directory = working_directory;
	for (i = 0; i < depth_working_directory; i++) {
		for (j = 0; j < 7; j++)
			*(*(directory_names + i) + j) = *(current_directory.my_name + j);
		*(*(directory_names + i) + j) = '\0'; // j == 7 so [8]
		current_directory = *(current_directory.parent);
	}

	/* now, directory_name consists of all directories to reach the working directory from root in descending order
	 * ex) /a/b/c/d
	 * => d c b a
	 */

	/* so, we need to print them in decsending order */
	for (i = depth_working_directory - 1; i >= 0; i--)
		printf("/%s", *(directory_names + i));
	printf("\n");

	free(directory_names);
}

static int compare_directory_names(unsigned char *dir1, unsigned char *dir2)
{
	int i;
	unsigned char c1, c2;

	for (i = 0; i < 7; i++) {
		c1 = *(dir1 + i);
		c2 = *(dir2 + i);
		if (c1 != c2)
			return 0;
		else if (c1 == '\0')
			break;
	}
	return 1;
}
/* Algorithm's below
 * from i = 0 to 7(Max length of directory name)
 * in i 'th place :
 * 	1. if there exist different character in same place before the comparison => They are different.
 *	2. (else) if this place is end of the string in both sides => They are same. ( close for-loop )
 *	( Why we break this comparison is there can exist different string next to its ending )
 *	ex) {'a', 'b', '\0', 32, ...} == {'a', 'b', '\0', 123, ...} but 32 != 123
 * Before it, we saw all posibillity.
 */

static int _mycd(unsigned char *directory_name)
{
	chainedDirectory temp = {};
	InodeList inode_list;
	DataBlock data_block;
	DataBlock indirect_block;
	int i, j;
	unsigned int remaining_space;
	unsigned char number_of_using_pointer;

	// chainedDirectory *temp;
	if (compare_directory_names("..", directory_name)) { // if the parameter is ".."
		working_directory = *(working_directory.parent);
		/* Predicting the problem when cd from root to .. */
		if (depth_working_directory != 0)
			depth_working_directory--;
		return 0;
	} else if (compare_directory_names(".", directory_name))
		return 0;
	else if (compare_directory_names("", directory_name))
		return 0;
	else {
		inode_list = getInodeList(working_directory.my_inode_number);
		if (inode_list.reference_count == 9) { // If it uses single indirect pointer
			indirect_block = getDataBlock(inode_list.single_indirect_address);
			number_of_using_pointer = indirect_block.contents[0];
			for (i = 1; i < number_of_using_pointer; i++) {
				data_block = getDataBlock(indirect_block.contents[i]);
				for (j = 0; j < 32; j++) {
					if (compare_directory_names(data_block.subfiles[j], directory_name)) {
						strncpy(temp.my_name, directory_name, 7);
						temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
						temp.parent = &working_directory;
						
						working_directory = temp;
						depth_working_directory++;
						return 0;
					}
				}
			}
			/* Processing the one remaining data block which generally not takes up full space */
			data_block = getDataBlock(indirect_block.contents[i]);
			remaining_space = inode_list.size % sizeof(DataBlock);
			for (j = 0; j < remaining_space / 8; j++) {
				if (compare_directory_names(data_block.subfiles[j], directory_name)) {
					strncpy(temp.my_name, directory_name, 7);
					temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
					temp.parent = &working_directory;
						
					working_directory = temp;
					depth_working_directory++;
					return 0;
				}
			}
				
			return -1; // There are no such directory.
		} else { // It uses direct pointer(s)
			for (i = 0; i < inode_list.reference_count - 1; i++) {
				data_block = getDataBlock(inode_list.direct_address[i]);
				for (j = 0; j < 32; j++) {
					if (compare_directory_names(data_block.subfiles[j], directory_name)) {
						strncpy(temp.my_name, directory_name, 7);
						temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
						temp.parent = &working_directory;
						
						working_directory = temp;
						depth_working_directory++;
						return 0;
					}
				}
			}
			/* Processing the one remaining data block which generally not takes up full space */
			data_block = getDataBlock(inode_list.direct_address[i]);
			remaining_space = inode_list.size % sizeof(DataBlock);
			for (j = 0; j < remaining_space / 8; j++) {
				if (compare_directory_names(data_block.subfiles[j], directory_name)) {
					strncpy(temp.my_name, directory_name, 7);
					temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
					temp.parent = &working_directory;
						
					working_directory = temp;
					depth_working_directory++;
					return 0;
				}
			}

			return -1; // There are no such directory.
		}
	}
}
void mycd(char **commands)
{
	int i;
	int written_characters = 0;
	char directory_name[7];
	char c;
	char *argument = commands[1];
	
	if (argument == NULL) { // '$ mycd' => go to root
		for (i = 0; i < depth_working_directory; i++)
			_mycd("..");
	} else {
		if (argument[0] == '/') { // If the path starts with the letter referring to the root
			for (i = 0; i < depth_working_directory; i++)
				_mycd("..");
			written_characters++;
		}
		do {
			directory_names[0] = '\0';
			for (i = 0; i < 7; i++) {
				c = argument[written_characters];
				written_characters++;
				if (c == '\0' || c == '/')
					break;
				else
					directory_name[i] = c;
			}
			if (i == 7) {
				c = argument[written_characters];
				written_characters++;
				if (c != '\0' && c != '/')
					// The length of directory name field exceeds
			}
			if (_mycd(directory_name) == -1) { // If there doesn't exist such directory
				// TODO
			}
		} while (c != '\0');
	}
}

void mytree(char **commands)
{
}
