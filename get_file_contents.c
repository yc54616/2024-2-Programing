#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "file_system.h"
#include "io_stream.h"


char* getFileContentsWithSourceFileName(char* source_file) { //source_file이 DataBlock의 contents 멤버 안에 존재하면 해당 파일의 InodeList의 directs 멤버에 해당하는 모든 contents 내용 return, 예외 발생 시 error return 
	int inode_list_index = 0, index_of_data_block = 0, index_of_contents = 0;
    char* file_content = malloc(sizeof(char) * (256 * 8));
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
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