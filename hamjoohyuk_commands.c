#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "file_system.h"
#include "io_stream.h"

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

char* getFileContentsWithSourceFileName(char* source_file) { //source_file이 DataBlock의 contents 멤버 안에 존재하면 해당 파일의 InodeList의 directs 멤버에 해당하는 모든 contents 내용 return, 예외 발생 시 error return 
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    int inode_list_index = 0, index_of_data_block = 0, index_of_contents = 0;
    char* file_content = malloc(sizeof(char) * (256 * 8));
    bool is_source_file_name_exist = false;
    for(int i = 0; i < SIZE_DATABLOCK; i++) {
        data_block = getDataBlock(i);
        for(int j = 0; j < strlen(source_file); j++) {
            if(data_block.contents[j] == source_file[j]) {
                if(j == strlen(source_file) - 1) {
                    is_source_file_name_exist = true;
                    index_of_data_block = i;
                    index_of_contents = j;
                }
                continue;
            }
            else break;
            }
    }

    if(is_source_file_name_exist == false) {
        return "error"; //임시
    }

    for(int k = 0; k < SIZE_INODELIST; k++) {
        inode_list = getInodeList(k + 1); //Inode는 1부터 시작
        if(inode_list.file_mode == 0) { //0 == file, 1 == directory
            for(int l = 0; l < 8; l++) {
                data_block = getDataBlock(inode_list.direct_address[l]);
                strcat(file_content, data_block.contents);
            }
            printf("%s", file_content); //test
            return file_content;
        }
        else return "error"; //임시
    }
}

void writeFileContents(char* entire_contents) { //긴 문자열을 나
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    int len = strlen(entire_contents);
    
    if (len > 256 * 6) {
        return; //임시 에러 처리 
    }
    
    char devided_contents[8][256];  // 8개의 256바이트 배열 선언(Direct Block)
    
    for (int i = 0; i < 8 && i * 256 < len; i++) {  // 256바이트씩 나눠서 배열에 복사
        strncpy(devided_contents[i], entire_contents + i * 256, 256);
    }

    for (int i = 0; i < 8; i++) { 
        printf("devided_contents[%d]: %s\n", i, devided_contents[i]); //test
        for (int j = 0; j < 256; j++) {
            data_block = getDataBlock(j);
            if(data_block.contents == NULL && devided_contents[i] != NULL){
                setDataBlock(j, devided_contents[i]);
                
                break;
            }

        }

    }

    
}

int main() {
    //printf("%s", getFileContentsWithSourceFileName("")); //파일 이름 삽입
    writeFileContents("");
}