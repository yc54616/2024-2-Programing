#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "system/file_system.h"
#include "system/io_stream.h"
#include "system/data_struct.h"
#include "hamjoohyuk_commands.h"

time_t getCurTime()
{
    time_t cur_time;
    time(&cur_time);
    return cur_time;
}

int getNeededDirectAdressNumber(char *entire_contents)
{
    if (strlen(entire_contents) > 256 * 8)
    {
        return 9; // single indirect adress 필요
    }
    return strlen(entire_contents) / 256 + 1;
}

int allocateInodeForNewFiles(char *new_file_name, int direct_adress_number, int content_size, int inode)
{ // bigbig, 9, 2050
    SuperBlock super_block = getSuperBlock();
    InodeList inode_list;
    DataBlock data_block;
    unsigned char new_inode_list_index = findEmptyInode();
    unsigned char new_data_block_index[8] = {0};
    int directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int start_index = 0;
    char new_file_name_for_save[8] = {
        0,
    };
    setSuperBlock(new_inode_list_index, 1);
    time_t cur_time;
    time(&cur_time);

    for (int i = 0; i < direct_adress_number && i < 8; i++)
    {
        new_data_block_index[i] = findEmptyDataBlock();
        setSuperBlock(128 + new_data_block_index[i] + 1, 1);
    }

    if (direct_adress_number < 9)
    {
        setInodeList(new_inode_list_index, GENERAL, cur_time, cur_time, content_size, direct_adress_number, new_data_block_index, 0);
    }
    else
    {
        unsigned new_single_indirect_block = allocateSingleIndirectBlock();
        setInodeList(new_inode_list_index, GENERAL, cur_time, cur_time, content_size, direct_adress_number, new_data_block_index, new_single_indirect_block);
    }

    strcpy(new_file_name_for_save, new_file_name);
    for (int i = 0; i < 7 - strlen(new_file_name); i++)
        strcat(new_file_name_for_save, "\0");
    new_file_name_for_save[7] = new_inode_list_index;
    writeDirectory(new_file_name_for_save, inode, DIRECTORY);
    return new_inode_list_index;
}

int allocateSingleIndirectBlock()
{
    unsigned char new_single_indirect_block_adress = findEmptyDataBlock();
    setSuperBlock(SIZE_INODELIST + new_single_indirect_block_adress + 1, 1);
    return new_single_indirect_block_adress;
}

void writeFileContents(char *entire_contents, int inode_list_adress, int direct_adress_number)
{
    SuperBlock super_block = getSuperBlock();
    InodeList inode_list = getInodeList(inode_list_adress);
    DataBlock data_block;
    char data_block_adress[8] = {0};
    unsigned char divided_contents[8][256] = {0}; // 8개의 256바이트 배열 선언(Direct Block)

    int copy_count = 0;
    for (copy_count; copy_count < direct_adress_number && copy_count * 256 < strlen(entire_contents) && copy_count < 8; copy_count++)
    { // 256바이트씩 나눠서 배열에 복사
        strncpy(divided_contents[copy_count], entire_contents + copy_count * 256, 256);
    }

    for (int i = 0; i < direct_adress_number && i < 8; i++)
    {
        data_block_adress[i] = inode_list.direct_address[i];
        setDataBlock(data_block_adress[i], divided_contents[i]);
        data_block = getDataBlock(data_block_adress[i]);
    }

    if (direct_adress_number > 8)
    {
        unsigned char divided_contents[256] = {0};
        unsigned char contents_of_single_indirect_block[256] = {0};
        int single_indirect_address = inode_list.single_indirect_address;
        int needed_extra_datablock_count = 0;
        int length = strlen(entire_contents);
        length -= 2048;
        while (length > 0)
        {
            needed_extra_datablock_count++;
            unsigned char new_data_block_adress_for_single_indirect_block = findEmptyDataBlock();
            setSuperBlock(SIZE_INODELIST + new_data_block_adress_for_single_indirect_block + 1, 1);
            contents_of_single_indirect_block[0] = needed_extra_datablock_count;
            contents_of_single_indirect_block[needed_extra_datablock_count] = new_data_block_adress_for_single_indirect_block;
            setDataBlock(single_indirect_address, contents_of_single_indirect_block);
            data_block = getDataBlock(single_indirect_address);
            int i = 0;
            for (i = 0; i < 256; i++)
            {
                if (*(entire_contents + copy_count * 256) == 0)
                    break;
            }

            *(divided_contents) = 0;

            for (int j = 0; j < i; j++)
            {
                divided_contents[j] = entire_contents[copy_count * 256 + j];
            }

            setDataBlock(new_data_block_adress_for_single_indirect_block, divided_contents);
            copy_count++;
            length -= 256;
        }
    }
}

char getAllDirectAdressWithSourceFileName(char *source_file, int result_index)
{
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int inode_number_of_file;
    bool is_source_file_name_exist = false;
    SuperBlock super_block = getSuperBlock();
    InodeList inode_list;
    DataBlock data_block;
    inode_list = getInodeList(now_working_directory_inode_number);

    for (int i = 0; i < 8; i++)
    {
        data_block = getDataBlock(inode_list.direct_address[i]);
        for (int j = 0; j < 32; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                if (source_file[k] == data_block.contents[j * 8 + k])
                {
                    if (k == 6)
                    {
                        inode_number_of_file == *(data_block.contents + (j * 8 + 7));
                        is_source_file_name_exist = true;
                    }
                }
                else
                    break;
            }
        }
    }

    if (is_source_file_name_exist == false)
    {
        return 'e'; // 임시
    }

    inode_list = getInodeList(inode_number_of_file);
    return inode_list.direct_address[result_index];
}

int getInodeNumberWithSourceFileName(char *source_file)
{
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int inode_number_of_file;
    bool is_source_file_name_exist = false;
    SuperBlock super_block = getSuperBlock();
    InodeList inode_list;
    DataBlock data_block;
    inode_list = getInodeList(now_working_directory_inode_number);

    for (int i = 0; i < 8; i++)
    {
        data_block = getDataBlock(inode_list.direct_address[i]);
        for (int j = 0; j < 32; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                if (source_file[k] == data_block.contents[j * 8 + k])
                {
                    if (k == 6)
                    {
                        inode_number_of_file == *(data_block.contents + (j * 8 + 7));
                        is_source_file_name_exist = true;
                    }
                }
                else
                    break;
            }
        }
    }

    if (is_source_file_name_exist == false)
    {
        return -1; // 임시
    }

    return inode_number_of_file;
}

char *getFileContentsWithSourceFileName(char *source_file, bool first_call)
{
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int inode_number_of_file;
    char *file_name = (char *)malloc(sizeof(char) * 7);
    bool is_source_file_name_exist = false;
    char *file_content = (char *)calloc(256 * 9, sizeof(char) + 2);
    SuperBlock super_block = getSuperBlock();
    InodeList inode_list;
    DataBlock data_block;

    inode_list = getInodeList(now_working_directory_inode_number);
    for (int i = 0; i < 8; i++)
    {
        if (i > 0 && *(inode_list.direct_address + i) == 0)
            break;
        data_block = getDataBlock(*(inode_list.direct_address + i));
        for (int j = 0; j < 32; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                if (*(source_file + k) == *(data_block.contents + (j * 8 + k)))
                {
                    if (k == 6)
                    {
                        inode_number_of_file = *(data_block.contents + (j * 8 + k + 1));
                        is_source_file_name_exist = true;
                        break;
                    }
                }
                else
                    break;
            }
            if (is_source_file_name_exist)
                break;
        }
    }

    if (!first_call)
        return "그런 파일이 없습니다.\n";

    if (is_source_file_name_exist == false)
    {
        if (getFileContentsWithSourceFileName(source_file, false) == "그런 파일이 없습니다.\n")
        {
            return "그런 파일이 없습니다.\n";
        }
    }

    inode_list = getInodeList(inode_number_of_file);

    for (int i = 0; i < 8; i++)
    {
        data_block = getDataBlock(*(inode_list.direct_address + i));
        for (int j = 0; j < strlen(data_block.contents); j++)
        {
            if (data_block.contents[j] != '\0')
                data_block.contents[j];
        }
    }

    for (int i = 0; i < 8; i++)
    {
        data_block = getDataBlock(*(inode_list.direct_address + i));
        strcat(file_content, data_block.contents);
    }
    printf("\n\n");
    return file_content;
}

char *getIndirectBlockContentsWithSourceFileName(char *source_file)
{
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int inode_number_of_file = 0;
    int number_of_indirect = 0;
    bool is_source_file_name_exist = false;
    SuperBlock super_block = getSuperBlock();
    InodeList inode_list;
    DataBlock data_block;
    inode_list = getInodeList(now_working_directory_inode_number);
    for (int i = 0; i < 8; i++)
    {
        if (i > 0 && *(inode_list.direct_address + i) == 0)
            break;
        data_block = getDataBlock(*(inode_list.direct_address + i));
        for (int j = 0; j < 32; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                if (*(source_file + k) == *(data_block.contents + (j * 8 + k)))
                {
                    if (k == 6)
                    {
                        inode_number_of_file = *(data_block.contents + (j * 8 + k + 1));
                        is_source_file_name_exist = true;
                        break;
                    }
                }
                else
                    break;
            }
            if (is_source_file_name_exist)
                break;
        }
    }
    inode_list = getInodeList(inode_number_of_file);
    int single_indirect_adress = inode_list.single_indirect_address;
    data_block = getDataBlock(single_indirect_adress);
    int indirect_count = data_block.contents[0];
    for (number_of_indirect; data_block.contents[number_of_indirect] != 0; number_of_indirect++)
        ;
    char *indirect_content = (char *)calloc(256 * number_of_indirect + 1, sizeof(char) + 2);

    for (int i = 1; i < number_of_indirect + 1; i++)
    {
        int new_content = data_block.contents[i];
        data_block = getDataBlock(new_content);
        strcat(indirect_content, data_block.contents);
        data_block = getDataBlock(single_indirect_adress);
    }
    return indirect_content;
}

int getUsingIndirectBlock(char *source_file)
{
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int inode_number_of_file = 0;
    int number_of_indirect = 0;
    bool is_source_file_name_exist = false;
    SuperBlock super_block = getSuperBlock();
    InodeList inode_list;
    DataBlock data_block;
    inode_list = getInodeList(now_working_directory_inode_number);

    for (int i = 0; i < 8; i++)
    {
        data_block = getDataBlock(inode_list.direct_address[i]);
        for (int j = 0; j < 32; j++)
        {
            for (int k = 0; k < 7; k++)
            {
                if (source_file[k] == data_block.contents[j * 8 + k])
                {
                    if (k == 6)
                    {
                        inode_number_of_file == *(data_block.contents + (j * 8 + 7));
                    }
                }
                else
                    break;
            }
        }
    }
    inode_list = getInodeList(inode_number_of_file);
    int single_indirect_adress = inode_list.single_indirect_address;
    data_block = getDataBlock(single_indirect_adress);
    int indirect_count = data_block.contents[0];
    for (number_of_indirect; data_block.contents[number_of_indirect] != 0; number_of_indirect++)
        ;
    return number_of_indirect;
}

void mycat(char **commands)
{
    char *file_name = (char *)calloc(sizeof(char) + 1, 8);
    int i = 0;
    int printlen = 0;
    if (*(commands + 1) == NULL)
    {
        errmsg("인자가 불충분합니다.\n");
        free(file_name);
        return;
    }
    else if (strlen(*(commands + 1)) > 7)
    {
        errmsg("그런 파일이 없습니다.\n");
        free(file_name);
        return;
    }
    strcpy(file_name, *(commands + 1));
    char *file_content = (char *)calloc(256 * 8 + 1, sizeof(char) + 2);
    strcpy(file_content, getFileContentsWithSourceFileName(file_name, true));

    if (!strcmp(file_content, "그런 파일이 없습니다.\n"))
    {
        printf("그런 파일이 없습니다.\n");
        // 정적 문자열이므로 free 하지 않음
        return;
    }

    printlen = strlen(file_content) - (7 - strlen(file_content) / 256);
    for (int i = 0; i < printlen; i++)
    {
        if (file_content[i] != '\0')
            printf("%c", file_content[i]);
    }

    if (getUsingIndirectBlock(file_name))
    { // indirect 출력
        int using_indirect_block_number = getUsingIndirectBlock(file_name);
        char *indirect_content = (char *)calloc(256 * using_indirect_block_number + 1, sizeof(char) + 2);
        strcpy(indirect_content, getIndirectBlockContentsWithSourceFileName(file_name));
        int indirect_printlen = strlen(indirect_content) - using_indirect_block_number;
        for (int i = 0; i < indirect_printlen; i++)
            printf("%c", indirect_content[i]);
        free(indirect_content);
    }

    printf("\n");
    // free(file_name);
    free(file_content);
}

void myshowfile(char **commands)
{
    int block_address;
    DataBlock data_block;
    int i = 0;
    int num1 = 0;
    int num2 = 0;
    char file_name[7] = {0};

    if (commands[1] == NULL)
    {
        errmsg("인자가 불충분합니다.\n");
        return;
    }

    for (i = 0; commands[1][i] != '\0'; i++)
    {
        if (commands[1][i] < '0' || commands[1][i] > '9')
        {
            errmsg("범위는 양수여야 합니다.\n");
            return;
        }
    }

    if (i == 0)
    {
        errmsg("인자가 불충분합니다.\n");
        return;
    }

    if (commands[2] == NULL)
    {
        errmsg("인자가 불충분합니다.\n");
        return;
    }

    for (i = 0; commands[2][i] != '\0'; i++)
    {
        if (commands[2][i] < '0' || commands[2][i] > '9')
        {
            errmsg("범위는 양수여야 합니다.\n");
            return;
        }
    }

    if (i == 0)
    {
        errmsg("인자가 불충분합니다.\n");
        return;
    }

    if (commands[3] == NULL)
    {
        errmsg("인자가 불충분합니다.\n");
        return;
    }

    else if (strlen(commands[3]) > 7)
    {
        errmsg("그런 파일이 없습니다.\n");
        return;
    }

    num1 = strtol(commands[1], NULL, 10);
    num2 = strtol(commands[2], NULL, 10);
    strcpy(file_name, commands[3]);
    char *file_content = (char *)calloc(256 * 8 + 1, sizeof(char) + 2);
    strcpy(file_content, getFileContentsWithSourceFileName(file_name, true));
    int printlen = strlen(file_content) - (7 - strlen(file_content) / 256);
    int contentlen = strlen(file_content);
    for (int i = 0; i < 7 - contentlen / 256; i++)
    {
        file_content[contentlen - 1 - i] = 0;
    }

    if (getUsingIndirectBlock(file_name))
    {
        file_content = (char *)realloc(file_content, (256 * 8 + getUsingIndirectBlock(file_name) * 256 + 1) * sizeof(char) + 2);
        if (file_content == NULL)
        {
            printf("파일 포인터 에러!");
            return;
        }
        int using_indirect_block_number = getUsingIndirectBlock(file_name);
        char *indirect_content = (char *)calloc(256 * using_indirect_block_number + 1, sizeof(char) + 2);
        strcpy(indirect_content, getIndirectBlockContentsWithSourceFileName(file_name));
        int indirect_printlen = strlen(indirect_content) - using_indirect_block_number;
        int indirect_strlen = strlen(indirect_content);
        for (int i = 0; i < using_indirect_block_number; i++)
            indirect_content[indirect_strlen - 1 - i] = 0;
        strcat(file_content, indirect_content);
        if (indirect_content != NULL)
            free(indirect_content);
    }

    if (!strcmp(file_content, "그런 파일이 없습니다.\n"))
    {
        printf("그런 파일이 없습니다.\n");
        if (file_content != NULL)
            free(file_content);
        return;
    }

    if (num1 < 0 || num2 >= strlen(file_content) || num1 > num2)
    {
        printf("범위가 올바르지 않습니다.\n"); // 임시
        if (file_content != NULL)
            free(file_content);
        return;
    }

    for (i = num1 - 1; i <= num2 - 1; i++)
    {
        if (file_content[i] == '\0')
        {
            printf("범위가 올바르지 않습니다.\n");
            if (file_content != NULL)
                free(file_content);
            return;
        }
    }

    for (i = num1 - 1; i <= num2 - 1; i++)
    {
        printf("%c", file_content[i]);
    }

    printf("\n");
    if (file_content != NULL)
        free(file_content);
}

void mycp(char **commands)
{
    // char *source_file;
    // char *dest_file;
    // int inode_list_address_of_dest_file;

    // if (commands[1] == NULL)
    // {
    //     printf("인자가 불충분합니다.");
    //     return;
    // }
    // else if (strlen(commands[1]) > 7)
    // {
    //     printf("그런 파일이 없습니다.");
    //     return;
    // }
    // if (commands[2] == NULL)
    // {
    //     printf("인자가 불충분합니다.");
    //     return;
    // }
    // else if (strlen(commands[2]) > 7)
    // {
    //     printf("파일 이름이 너무 깁니다.");
    //     return;
    // }

    // source_file = malloc(sizeof(char) * strlen(commands[1]) + 1);
    // dest_file = malloc(sizeof(char) * strlen(commands[2]) + 1);
    // strcpy(source_file, commands[1]);
    // strcpy(dest_file, commands[2]);
    // // if (getFileContentsWithSourceFileName(source_file, true) == "그런 파일이 없습니다.\n")
    // // {
    // //     printf("그런 파일이 없습니다.\n");
    // //     return;
    // // }

    // SuperBlock super_block = getSuperBlock();
    // InodeList inode_list;
    // DataBlock data_block;
    // char *source_file_content = (char *)calloc(256 * 8 + 1, sizeof(char) + 2);
    // char *sliced_source_file_content = (char *)calloc(256 * 8 + 1, sizeof(char) + 2);
    // char *source_indirect_content = NULL;
    // char *sliced_source_indirect_content = NULL;
    // bool is_indirect_block_used = false;

    // if (getUsingIndirectBlock(source_file))
    // {
    //     is_indirect_block_used = true;
    //     int using_indirect_block_number = getUsingIndirectBlock(source_file);
    //     sliced_source_file_content = (char *)realloc(sliced_source_file_content, (256 * 8 + 256 * using_indirect_block_number + 1) * sizeof(char) + 1);
    // }

    // strcpy(source_file_content, getFileContentsWithSourceFileName(source_file, true));
    // strncpy(sliced_source_file_content, source_file_content, strlen(source_file_content) - (7 - (strlen(source_file_content) / 256)));

    // if (is_indirect_block_used)
    // {
    //     source_indirect_content = (char *)calloc(256 * getUsingIndirectBlock(source_file) + 1, sizeof(char) + 2);
    //     sliced_source_indirect_content = (char *)calloc(256 * getUsingIndirectBlock(source_file) + 1, sizeof(char) + 2);
    //     strcpy(source_indirect_content, getIndirectBlockContentsWithSourceFileName(source_file));
    //     strncpy(sliced_source_indirect_content, source_indirect_content, strlen(source_indirect_content) - getUsingIndirectBlock(source_file));
    //     strcat(sliced_source_file_content, sliced_source_indirect_content);
    //     if (source_indirect_content != NULL)
    //     {
    //         free(source_indirect_content);
    //         source_indirect_content = NULL;
    //     }
    //     if (sliced_source_indirect_content != NULL)
    //     {
    //         free(sliced_source_indirect_content);
    //         sliced_source_indirect_content = NULL;
    //     }
    // }
    // getNeededDirectAdressNumber(sliced_source_file_content);
    // inode_list_address_of_dest_file = allocateInodeForNewFiles(dest_file, getNeededDirectAdressNumber(sliced_source_file_content), strlen(sliced_source_file_content));
    // writeFileContents(sliced_source_file_content, inode_list_address_of_dest_file, getNeededDirectAdressNumber(sliced_source_file_content));

    // if (source_file != NULL)
    // {
    //     free(source_file);
    //     source_file = NULL;
    // }
    // if (dest_file != NULL)
    // {
    //     free(dest_file);
    //     dest_file = NULL;
    // }
    // if (source_file_content != NULL)
    // {
    //     free(source_file_content);
    //     source_file_content = NULL;
    // }
    // if (sliced_source_file_content != NULL)
    // {
    //     free(sliced_source_file_content);
    //     sliced_source_file_content = NULL;
    // }
}

void mycpto(char **commands)
{
    char *source_file;
    char *host_dest_file;

    if (*(commands + 1) == NULL)
    {
        printf("인자가 불충분합니다.");
        return;
    }
    else if (strlen(*(commands + 1)) > 7)
    {
        printf("그런 파일이 없습니다.");
        return;
    }
    if (*(commands + 2) == NULL)
    {
        printf("인자가 불충분합니다.");
        return;
    }

    source_file = malloc(sizeof(char) * strlen(commands[1]) + 1);
    host_dest_file = malloc(sizeof(char) * strlen(commands[2]) + 1);

    strcpy(source_file, *(commands + 1));
    strcpy(host_dest_file, *(commands + 2));

    char *source_file_content = (char *)calloc(256 * 8 + 1, sizeof(char) + 2);
    char *sliced_source_file_content = (char *)calloc(256 * 8, sizeof(char) + 2);
    char *source_indirect_content = NULL;
    char *sliced_source_indirect_content = NULL;
    bool is_indirect_block_used = false;

    if (getUsingIndirectBlock(source_file))
    {
        is_indirect_block_used = true;
        int using_indirect_block_number = getUsingIndirectBlock(source_file);
        sliced_source_file_content = (char *)realloc(sliced_source_file_content, (256 * 8 + 256 * using_indirect_block_number + 1) * sizeof(char) + 2);
    }

    strcpy(source_file_content, getFileContentsWithSourceFileName(source_file, true));
    strncpy(sliced_source_file_content, source_file_content, strlen(source_file_content) - (7 - (strlen(source_file_content) / 256)));

    if (is_indirect_block_used)
    {
        source_indirect_content = (char *)calloc(256 * getUsingIndirectBlock(source_file) + 1, sizeof(char) + 2);
        sliced_source_indirect_content = (char *)calloc(256 * getUsingIndirectBlock(source_file) + 1, sizeof(char) + 2);
        strcpy(source_indirect_content, getIndirectBlockContentsWithSourceFileName(source_file));
        strncpy(sliced_source_indirect_content, source_indirect_content, strlen(source_indirect_content) - getUsingIndirectBlock(source_file));
        // strcpy(sliced_source_indirect_content, source_indirect_content);
        strcat(sliced_source_file_content, sliced_source_indirect_content);
        if (source_indirect_content != NULL)
        {
            free(source_indirect_content);
            source_indirect_content = NULL;
        }
        if (sliced_source_indirect_content != NULL)
        {
            free(sliced_source_indirect_content);
            sliced_source_indirect_content = NULL;
        }
    }

    FILE *host_txt;
    host_txt = fopen(host_dest_file, "w+");
    if (host_txt == NULL)
    {
        printf("호스트 파일을 만들거나 열 수 없습니다.\n");
        return;
    }
    for (int i = 0; i < strlen(sliced_source_file_content); i++)
    {
        fputc(*(sliced_source_file_content + i), host_txt);
    }
    fclose(host_txt);
    if (source_file != NULL)
        free(source_file);
    if (host_dest_file != NULL)
        free(host_dest_file);
    if (source_file_content != NULL)
        free(source_file_content);
    if (sliced_source_file_content != NULL)
        free(sliced_source_file_content);
}

void mycpfrom(char **commands)
{
    if (commands[1] == NULL || commands[2] == NULL)
    {
        printf("인자가 불충분합니다.\n"); // 임시
        return;
    }

    char *argument = commands[1];
	char *argument2 = commands[2];

    printf("argument : %s, argument2 : %s\n", argument, argument2);

	unsigned char *host_source_file = (unsigned char *)calloc(sizeof(unsigned char), strlen(argument));
	unsigned char *new_name = (unsigned char *)calloc(sizeof(unsigned char), strlen(argument2));
	strcpy(host_source_file, argument);
	strcpy(new_name, argument2);

	int inode_number = findNameToInode(host_source_file);

	unsigned char *path = (unsigned char *)calloc(sizeof(unsigned char), strlen(new_name));
	unsigned char *dest_file = (unsigned char *)calloc(sizeof(unsigned char), 8);
	
	int inode_number_base = findNameToBaseInode(new_name, path, dest_file);

    printf("host_source_file : %s, new_name : %s, path : %s, dest_file : %s\n", host_source_file, new_name, path, dest_file);

    SuperBlock super_block = getSuperBlock();
    InodeList inode_list;
    DataBlock data_block;
    FILE *host_txt;
    int host_txt_size;
    char *buffer;
    int file_count;
    int inode_list_address_of_dest_file = 0;

    host_txt = fopen(host_source_file, "r");
    if (host_txt == NULL)
    {
        printf("그런 호스트 파일이 없습니다.\n");
        free(host_source_file);
        free(new_name);
        free(path);
        free(dest_file);
        return;
    }

    fseek(host_txt, 0, SEEK_END);
    host_txt_size = ftell(host_txt);
    buffer = malloc(host_txt_size + 1);
    memset(buffer, 0, host_txt_size + 1);
    fseek(host_txt, 0, SEEK_SET);
    file_count = fread(buffer, host_txt_size, 1, host_txt);

    inode_list_address_of_dest_file = allocateInodeForNewFiles(dest_file, getNeededDirectAdressNumber(buffer), strlen(buffer), inode_number_base);
    writeFileContents(buffer, inode_list_address_of_dest_file, getNeededDirectAdressNumber(buffer));

    free(host_source_file);
    free(new_name);
    free(path);
    free(dest_file);
    free(buffer);
}
