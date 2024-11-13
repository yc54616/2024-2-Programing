#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "file_system.h"
#include "io_stream.h"

int allocateInodeForNewFiles(char* new_file_name) {
    SuperBlock super_block = getSuperBlock();
   InodeList inode_list;
    DataBlock data_block;
    unsigned char new_inode_list_index = findEmptyInode();
    setSuperBlock(new_inode_list_index, 1);
    unsigned char new_datablock_index[8] = {0};
    for(int i = 0; i < 8; i++) {
        unsigned char tmp = findEmptyDataBlock();
        //new_datablock_index[i] = 
        //setSuperBlock(i, 1);
        setSuperBlock(128 + tmp + 1, 1);
        printf("%d ", tmp);
    }
    for(int i = 0; i < 8; i++) {
        printf(">> %d\n", new_datablock_index[i]);
    }
    setInodeList(new_inode_list_index, 0, 1, 1, 1, 8, new_datablock_index, 0);
    return new_inode_list_index;
}

void writeFileContents(char* entire_contents, int inode_list_adress) { 
    SuperBlock super_block = getSuperBlock();
   InodeList inode_list = getInodeList(inode_list_adress);
    DataBlock data_block;
    int len_of_entire_contents = strlen(entire_contents);
    char data_block_adress[8] = {0};
    char arranged_data_block_index[8] = {0};
    if (len_of_entire_contents > 256 * 8) { //
        return; //임시 에러 처리
    }

    char divided_contents[8][256] = {0};  // 8개의 256바이트 배열 선언(Direct Block)

    for (int i = 0; i < 8 && i * 256 < len_of_entire_contents; i++) {  // 256바이트씩 나눠서 배열에 복사
        strncpy(divided_contents[i], entire_contents + i * 256, 256);
    }

    for(int i = 0; i < 8; i++) {
        data_block_adress[i] = inode_list.direct_address[i];
        printf("divided_contents[%d]: %s\n", i, divided_contents[i]); //test
        data_block = getDataBlock(data_block_adress[i]);
        if(divided_contents[i] != NULL && divided_contents[i] != "\0") {
                setDataBlock(data_block_adress[i], divided_contents[i]);
                arranged_data_block_index[i] = data_block_adress[i];
                data_block = getDataBlock(data_block_adress[i]); 
                continue;
        }
    }
    for(int i = 0; i < 8; i++) printf("%d", arranged_data_block_index[i]);
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
    char* entire_contents = getFileContentsWithSourceFileName(file_name); 
    int len = strlen(entire_contents);
    if (num1 < 0 || num2 >= len || num1 > num2) {
        printf("입력값 오류"); //임시
        return;
    }

    for (int i = num1; i <= num2; i++) {
        printf("%c", entire_contents[i]);
    }
    printf("\n");
}

void myrm(char* file_name) {
    SuperBlock super_block = getSuperBlock();
   InodeList inode_list;
    DataBlock data_block;
    char direct_adress[8] = {0};
    
    for(int i = 0; i < 8; i++) { 
        direct_adress[i] = getAllDirectAdressWithSourceFileName(file_name, i);
    }

    for(int i = 0; i < 8; i++) {
        setDataBlock(direct_adress[i], NULL);
        setSuperBlock(direct_adress[i] + 128, 0);
    }
}   

int main() {
    SuperBlock super_block = getSuperBlock();
   InodeList inode_list;
    DataBlock data_block;

    int inode_list_adress = allocateInodeForNewFiles("filename.txt");
    printf("%d\n", inode_list_adress);
    writeFileContents("12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567", inode_list_adress);
    inode_list = getInodeList(inode_list_adress);
    data_block = getDataBlock(inode_list.direct_address[0]);
    printf("%s", data_block.contents);
    //printf("%s", getFileContentsWithSourceFileName("")); //파일 이름 삽입
    //writeFileContents("");
}
