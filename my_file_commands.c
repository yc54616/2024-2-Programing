#include "my_file_commands.h"

/* declarations of global variable */
extern chainedDirectory *working_directory;
extern int depth_working_directory;

void mytouch(char **commands)
{
    if (commands[1] == NULL)
    {
        printf("mytouch: missing file operand"); // error
        return;
    }
    char *argument = commands[1];
    char str[8] = {0};
    int inode_number;
    strcpy(str, argument);
    inode_number = findDictoryNameToInode(str);
    // if (inode_number != 0)
    // {
    //     printf("FIND!!\n");
    //     printf("%d\n", inode_number);
    // };
    time_t curTime;
    InodeList inode_list;
    time(&curTime);
    if (inode_number > 0) // 파일이 존재할 때
    {
        inode_list = getInodeList(working_directory->my_inode_number);
        inode_list.access_date = curTime;
        setInodeList(inode_number, inode_list.file_mode, inode_list.access_date, inode_list.birth_date, inode_list.size, inode_list.reference_count, inode_list.direct_address, inode_list.single_indirect_address);
    }
    else
    {
        inode_number = findEmptyInode();
        int dataBlock_num = findEmptyDataBlock();
        unsigned char address[8] = {
            dataBlock_num,
        };
        setInodeList(inode_number, GENERAL, curTime, curTime, 0, 1, address, 0);

        str[7] = inode_number;
        InodeList inode_list = getInodeList(working_directory->my_inode_number); // 파일 새로 만들기
        writeDirectoryDataBlock(str, working_directory->my_inode_number - 1, inode_list.size - 16);
    }
    setSuperBlock(inode_number, 1);
    setSuperBlock(SIZE_INODELIST + inode_number, 1);
}
