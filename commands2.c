#include "commands2.h"

/* declarations of global variable */
extern chainedDirectory *working_directory;
extern int depth_working_directory;

void mypwd(char **commands)
{
	int i, j;
	chainedDirectory *current_directory;
	unsigned char (*directory_names)[8];

	if (depth_working_directory == 0) // Exception
		printf("/");

	directory_names = (unsigned char (*)[8]) malloc(sizeof(unsigned char (*)[8]) * depth_working_directory); // from the global field in the shell.h
	current_directory = working_directory;
	for (i = 0; i < depth_working_directory; i++) {
		for (j = 0; j < 7; j++)
			*(*(directory_names + i) + j) = *(current_directory -> my_name + j);
		*(*(directory_names + i) + j) = '\0'; // j == 7 so [8]
		current_directory = current_directory -> parent;
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
	
	if (argument != NULL) {
		for (length = 0; (c = argument[length]) != '\0'; length++)
			;
		path = (unsigned char *)malloc(sizeof(unsigned char) * length);
		strcpy(path, argument);
	}
	if (cd(&working_directory, &depth_working_directory, path) == 0) { // If failed cd,
		printf("Such directory doesn't exist\n");
	}
	free(path);
}

void mytree(char **commands)
{

}
