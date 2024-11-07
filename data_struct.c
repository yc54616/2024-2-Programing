#include "data_struct.h"

/* declarations of global variable */
extern *chainedDirectory working_directory;
extern int depth_working_directory;

int compare_directory_names(unsigned char *dir1, unsigned char *dir2)
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
 * 	1. If different characters exist in the same place before the comparison => They are different.
 *	2. (else) If this place is the end of the string on both sides => They are the same. ( close for-loop )
 *	( Why we break this comparison is that there can exist different strings next to its ending )
 *	ex) {'a', 'b', '\0', 32, ...} == {'a', 'b', '\0', 123, ...} but 32 != 123
 * Before it, we saw all posibillities. 
 */

int getExistence(unsigned char *path)
{
	int i;
	chainedDirectory temp_CD;
	chainedDirectory *temp_CD_ptr;
	chainedDirectory *linked_directories; // First, 
	chainedDirectory *virtual_working_directory; // Virtually we use it alternately instead of virtual working directory.
	int virtual_depth_working_directory; // Similar as above.
	int written_characters = 0;
	char directory_name[7];
	char c;

	/* Coping real w.d into virtual w.d */
	virtual_depth_working_directory = depth_working_directory;
	virtual_working_directory = working_directory;

	linked_directories = (chainedDirectory*)malloc(sizeof(chainedDirectory) * depth_working_directory);
	for (i = 0; i < virtual_depth_working_directory; i++) {
		*(linked_directories + i) = *virtual_working_directory;
		virtual_working_directory = virtual_working_directory -> parent; // exploring
	}
	/* Now, the array consists of directories in descending order.
	 * ex)  /as/df/gh
	 *    => gh df as
	 * index  0  1  2
	 * So, we need to change these in increasing order.
	 */
	for (i = 0; i < virtual_depth_working_directory / 2; i++) {
		temp_CD = *(linked_directories + i);
		*(linked_directories + i) = *(linked_directories + virtual_depth_working_directory - i - 1);
		*(linked_directories + virtual_depth_working_directory - i - 1) = temp_CD;
	}

	/* root */
	temp_CD_ptr = (chainedDirectory *)malloc(sizeof(chainedDirectory));
	temp_CD_ptr -> my_name[0] = '\0';
	temp_CD_ptr -> my_inode_number = 1;
	temp_CD_ptr -> parent = temp_CD_ptr;
	for (i = 0; i < virtual_depth_working_directory; i++) {
		(*(linked_directories + i)).parent = temp_CD_ptr;
		temp_CD_ptr = *(linked_directories + i);
	}
	virtual_working_directory = temp_CD_ptr;
	free(linked_directories);
	/* We perfectly copied. */

	if (path == NULL) { // '$ mycd' => go to the root
		return 1; // The root always exists.
	} else {
		if (path[0] == '/') { // If the path starts with the letter referring to the root
			while (virtual_depth_working_directory != 0)
				_mycd(&virtual_working_directory, &virtual_depth_working_directory, "..");
			written_characters++; // Counting for '/'
		}
		do {
			directory_name[0] = '\0';
			for (i = 0; i < 7; i++) {
				c = path[written_characters];
				written_characters++;
				if (c == '\0' || c == '/')
					break;
				else
					directory_name[i] = c;
			}
			if (i == 7) {
				c = path[written_characters];
				written_characters++;
				if (c != '\0' && c != '/')
					return 0;
					// The length of the directory name field exceeds
			}
			directory_name[i] = '\0';
			/* Actually changing directory */
			if (_mycd(&virtual_working_directory, &virtual_depth_working_directory, directory_name) == -1) { // If there doesn't exist such directory
				return 0;
			}
			printf("!%s,%d!\n", virtual_working_directory.my_name, virtual_working_directory.my_inode_number);
		} while (c != '\0');
	}
	return 1;
}

int _mycd(chainedDirectory *top, int *cnt, unsigned char *directory_name)
{
	printf("in _mycd : %s\n", directory_name);
	chainedDirectory temp = {};
	InodeList inode_list;
	DataBlock data_block;
	DataBlock indirect_block;
	int i, j;
	unsigned int remaining_space;
	unsigned char number_of_using_pointer;

	// chainedDirectory *temp;
	if (compare_directory_names("..", directory_name)) { // if the parameter is ".."
		*top = *(top -> parent);
		/* Predicting the problem when cd from root to .. */
		if (*cnt != 0)
			(*cnt)--;
		return 0;
	} else if (compare_directory_names(".", directory_name))
		return 0;
	else if (compare_directory_names("", directory_name))
		return 0;
	else {
		inode_list = getInodeList(top -> my_inode_number);
		if (inode_list.reference_count == 9) { // If it uses a single indirect pointer
			indirect_block = getDataBlock(inode_list.single_indirect_address);
			number_of_using_pointer = indirect_block.contents[0];
			for (i = 1; i < number_of_using_pointer; i++) {
				data_block = getDataBlock(indirect_block.contents[i]);
				for (j = 0; j < 32; j++) {
					if (compare_directory_names(data_block.subfiles[j], directory_name)) {
						if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY) {
							printf("DEBUG 4\n");
							strncpy(temp.my_name, directory_name, 7);
							temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
							temp.parent = top;
							
							strncpy((*top).my_name, directory_name, 7);
							(*cnt)++;
							return 0;
						} else // This is not a directory.
							return -1;
					}
				}
			}
			/* Processing the one remaining data block, which generally does not take up the entire space */
			data_block = getDataBlock(indirect_block.contents[i]);
			remaining_space = inode_list.size % sizeof(DataBlock);
			for (j = 0; j < remaining_space / 8; j++) {
				if (compare_directory_names(data_block.subfiles[j], directory_name)) {
					if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY) {
						printf("DEBUG 1\n");
						strncpy(temp.my_name, directory_name, 7);
						temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
						temp.parent = top;
							
						*top = temp;
						(*cnt)++;
						return 0;
					} else // This is not a directory
						return -1;
				}
			}
				
			return -1; // There is no such directory.
		} else { // It uses direct pointer(s)
			for (i = 0; i < inode_list.reference_count - 1; i++) {
				data_block = getDataBlock(inode_list.direct_address[i]);
				for (j = 0; j < 32; j++) {
					if (compare_directory_names(data_block.subfiles[j], directory_name)) {
						if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY) {
							printf("DEBUG 2\n");
							strncpy(temp.my_name, directory_name, 7);
							temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
							temp.parent = top;
							
							*top = temp;
							(*cnt)++;
							return 0;
						} else // This is not a directory
							return -1;
					}
				}
			}
			/* Processing the one remaining data block, which generally does not take up the entire space */
			data_block = getDataBlock(inode_list.direct_address[i]);
			remaining_space = inode_list.size % sizeof(DataBlock);
			for (j = 0; j < remaining_space / 8; j++) {
				if (compare_directory_names(data_block.subfiles[j], directory_name)) {
					if (getInodeList(data_block.subfiles[j][7]).file_mode == DIRECTORY) {
						printf("DEBUG 3\n");
						strncpy(temp.my_name, directory_name, 7);
						temp.my_inode_number = data_block.subfiles[j][7]; // 8th member of the array.
						temp.parent = top;
							
						top = temp;
						(*cnt)++;
						printf("-%d,%s-\n", top -> parent -> my_inode_number);
						return 0;
					} else // This is not a directory
						return -1;
				}
			}
			return -1; // There is no such directory.
		}
	}
}
