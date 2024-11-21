#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "system/file_system.h"
#include "system/io_stream.h"
#include "system/data_struct.h"

time_t getCurTime() {
    time_t cur_time;
    time(&cur_time);
    return cur_time;
}

int getNeededDirectAdressNumber(char* entire_contents) {
    if (strlen(entire_contents) > 256 * 8) { 
        return 9; //single indirect adress 필요
    }
    return strlen(entire_contents) / 256 + 1;
}

int allocateInodeForNewFiles(char* new_file_name, int direct_adress_number, int content_size) {
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    unsigned char new_inode_list_index = findEmptyInode();
    unsigned char new_data_block_index[8] = {0};
    int directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int start_index = 0;
    char new_file_name_for_save[8];
    setSuperBlock(new_inode_list_index, 1);
    time_t cur_time;
	time(&cur_time);
    // for(int i = 0; i < 32; i++) {
    //     if(data_block.subfiles[i][0] != '\0') ;
    //     else {
    //         start_index = i * 8;
    //         printf("\n%d\n\n", start_index);
    //         break;
    //     }
    // }

    for(int i = 0; i < direct_adress_number; i++) {
        new_data_block_index[i] = findEmptyDataBlock();
        setSuperBlock(128 + new_data_block_index[i] + 1, 1);
    }

    //inode_list = getInodeList(directory_inode_number);
    setInodeList(new_inode_list_index, 0, cur_time, cur_time, content_size, direct_adress_number, new_data_block_index, 0);
    strcpy(new_file_name_for_save, new_file_name);
    for(int i = 0; i < 7 - strlen(new_file_name); i++) strcat(new_file_name_for_save, "\0");
    inode_list = getInodeList(now_working_directory_inode_number);
    new_file_name_for_save[7] = new_inode_list_index;
    writeDirectoryDataBlock(new_file_name_for_save, inode_list.direct_address[0], inode_list.size);
    setInodeList(now_working_directory_inode_number, 1, cur_time, inode_list.birth_date, inode_list.size+8, inode_list.reference_count, inode_list.direct_address, inode_list.single_indirect_address);
    return new_inode_list_index;
}

void writeFileContents(char* entire_contents, int inode_list_adress, int direct_adress_number) { 
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list = getInodeList(inode_list_adress);
    DataBlock data_block;
    char data_block_adress[8] = {0};
    unsigned char divided_contents_but_it_will_be_replaced_because_it_has_error_and_i_love_long_long_variable_name[8][256] = {0};  // 8개의 256바이트 배열 선언(Direct Block)
    unsigned char new_contents[256] = {0};

    for (int i = 0; i < direct_adress_number && i * 256 < strlen(entire_contents); i++) {  // 256바이트씩 나눠서 배열에 복사
        strncpy(divided_contents_but_it_will_be_replaced_because_it_has_error_and_i_love_long_long_variable_name[i], entire_contents + i * 256, 256);
    }

    for(int i = 0; i < direct_adress_number; i++) {
        data_block_adress[i] = inode_list.direct_address[i];
        if(divided_contents_but_it_will_be_replaced_because_it_has_error_and_i_love_long_long_variable_name[i] != NULL && strlen(divided_contents_but_it_will_be_replaced_because_it_has_error_and_i_love_long_long_variable_name[i]) != 0) {
                for(int j = 0; j < 256; j++) new_contents[j] = divided_contents_but_it_will_be_replaced_because_it_has_error_and_i_love_long_long_variable_name[i][j];
                setDataBlock(data_block_adress[i], new_contents);
                data_block = getDataBlock(data_block_adress[i]); 
                continue;
        }
    }
}

char getAllDirectAdressWithSourceFileName(char* source_file, int result_index) {
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    int inode_list_of_file = 0;
    bool is_source_file_name_exist = false;
    for(int i = 0; i < SIZE_DATABLOCK; i++) {
        data_block = getDataBlock(i);
        for(int j = 0; j < 32; j++) {
            for(int k = 0; k < 7; k++) {
                if(source_file[k] == data_block.subfiles[j][k]) {
                    if(k == 7) {
                        inode_list_of_file = data_block.subfiles[j][8];
                        is_source_file_name_exist = true;
                    }
                }
            }
    }
    }

    if(is_source_file_name_exist == false) {
        return 'e'; //임시
    }

    inode_list = getInodeList(inode_list_of_file);
    return inode_list.direct_address[result_index];

}

char* getFileContentsWithSourceFileName(char* source_file) {
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    int inode_list_of_file = 0;
    char* file_content = malloc(sizeof(char) * (256 * 8));
    bool is_source_file_name_exist = false;
    for(int i = 0; i < SIZE_DATABLOCK; i++) {
        data_block = getDataBlock(i);
        for(int j = 0; j < 32; j++) {
            for(int k = 0; k < 7; k++) {
                if(source_file[k] == data_block.subfiles[j][k]) {
                    if(k == 7) {
                        inode_list_of_file = data_block.subfiles[j][8];
                        is_source_file_name_exist = true;
                    }
                }
            }
    }
    }

    if(is_source_file_name_exist == false) {
        return "error"; //임시
    }

    inode_list = getInodeList(inode_list_of_file);
    for(int i = 0; i < 8; i++) {
        data_block = getDataBlock(inode_list.direct_address[i]);
        strcat(file_content, data_block.contents);
    }
    printf("%s", file_content); //test
    return file_content;

}


void myshowfile(int num1, int num2, char* file_name) {
    //char* entire_contents = getFileContentsWithSourceFileName(file_name); 
    //int len = strlen(entire_contents);
    if (num1 < 0 || num2 >= strlen(getFileContentsWithSourceFileName(file_name)) || num1 > num2) {
        printf("범위가 올바르지 않습니다."); //임시
        return;
    }

    for (int i = num1; i <= num2; i++) {
        printf("%c", getFileContentsWithSourceFileName(file_name)[i]);
    }
    printf("\n");
}

void myrm(char** commands) {
    if(commands[1] == NULL) {
        printf("인자가 불충분합니다.");
        return ;
    }
    else if(strlen(commands[1]) > 7) {
        printf("파일 이름이 너무 깁니다."); //임시
        return ;

    }
    char* file_name = commands[1];
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    inode_list = getInodeList(now_working_directory_inode_number);
    data_block = getDataBlock(inode_list.direct_address[0]);
}   

void mycpfrom(char** commands) {
    char* host_source_file;
    char* dest_file;
    if(commands[1] == NULL) {
        printf("인자가 불충분합니다."); //임시
        return ;
    }
    else if(commands[2] == NULL) {
        printf("인자가 불충분합니다."); //임시
        return ;
    }
    else if(strlen(commands[2]) > 7) {
        printf("파일 이름이 너무 깁니다."); //임시
        return ;
    }
    host_source_file = malloc(sizeof(char) * strlen(commands[1]) + 1);
    dest_file = malloc(sizeof(char) * strlen(commands[2]) + 1);
    strcpy(host_source_file, commands[1]);
    strcpy(dest_file, commands[2]);
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    FILE *host_txt;
    char *host_content = {0};
    int host_txt_size;
    char *buffer;
    int file_count;
    int inode_list_address_of_dest_file = 0;

    host_txt = fopen(host_source_file, "r");
    if(host_txt == NULL) {
        printf("그런 호스트 파일이 없습니다.");
        return ;
    }

    fseek(host_txt, 0, SEEK_END);    
    host_txt_size = ftell(host_txt);   
    buffer = malloc(host_txt_size + 1);
    memset(buffer, 0, host_txt_size + 1); 
    fseek(host_txt, 0, SEEK_SET);
    file_count = fread(buffer, host_txt_size, 1, host_txt);    

    getNeededDirectAdressNumber(buffer);
    inode_list_address_of_dest_file = allocateInodeForNewFiles(dest_file, getNeededDirectAdressNumber(buffer), sizeof(buffer));
    writeFileContents(buffer, inode_list_address_of_dest_file, getNeededDirectAdressNumber(buffer));

    fclose(host_txt);   
    free(buffer);  
    free(host_source_file);
    free(dest_file);
    // inode_list = getInodeList(working_directory.my_inode_number);
    // setInodeList(working_directory.my_inode_number, 1, inode_list.birth_date, 1, 1, inode_list.reference_count + 1,  inode_list.direct_address, inode_list.single_indirect_address);
    
}
