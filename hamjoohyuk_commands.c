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

    for(int i = 0; i < direct_adress_number; i++) {
        new_data_block_index[i] = findEmptyDataBlock();
        setSuperBlock(128 + new_data_block_index[i] + 1, 1);
    }

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
                for(int j = 0; j < 256; j++) {
                    new_contents[j] = divided_contents_but_it_will_be_replaced_because_it_has_error_and_i_love_long_long_variable_name[i][j];
                }
                setDataBlock(data_block_adress[i], new_contents);
                data_block = getDataBlock(data_block_adress[i]); 
                continue;
        }
    }
}

char getAllDirectAdressWithSourceFileName(char* source_file, int result_index) {
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int inode_number_of_file;
    bool is_source_file_name_exist = false;
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    inode_list = getInodeList(now_working_directory_inode_number);
   
    for(int i = 0; i < 8; i++) {
        data_block = getDataBlock(inode_list.direct_address[i]);
        for(int j = 0; j < 32; j++) {
            for(int k = 0; k < 7; k++) {
                if(source_file[k] == data_block.contents[j * 8 + k]) {
                    if(k == 6) {
                        inode_number_of_file == *(data_block.contents + (j * 8 + 7));
                        is_source_file_name_exist = true;
                    }
                }
                else break;
            }
        }
    }

    if(is_source_file_name_exist == false) {
        return 'e'; //임시
    }
    
    inode_list = getInodeList(inode_number_of_file);
    return inode_list.direct_address[result_index];
}

char* getFileContentsWithSourceFileName(char* source_file, bool first_call) {
    int now_working_directory_inode_number = getNowWorkingDirectoryInodeNumber();
    int inode_number_of_file;
    char* file_name = (char*)malloc(sizeof(char) * 7);
    bool is_source_file_name_exist = false;
    char* file_content = (char *)calloc(256 * 8, sizeof(char));
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;

    inode_list = getInodeList(now_working_directory_inode_number);
    for(int i = 0; i < 8; i++) {
        if(i > 0 && *(inode_list.direct_address + i) == 0) break;
        data_block = getDataBlock(*(inode_list.direct_address + i));       
        for(int j = 0; j < 32; j++) {
            for(int k = 0; k < 7; k++) {
                if(*(source_file + k) == *(data_block.contents + (j * 8 + k))) {
                    if(k == 6) {
                        inode_number_of_file =  *(data_block.contents + (j * 8 + k + 1));
                        is_source_file_name_exist = true;
                        break;
                    }
                }
                else break;
            }
            if(is_source_file_name_exist) break;
        }
    }

    if(!first_call) return "그런 파일이 없습니다.\n";
    
    if(is_source_file_name_exist == false) {
        if(getFileContentsWithSourceFileName(source_file, false) == "그런 파일이 없습니다.\n") {
            return "그런 파일이 없습니다.\n";
        }
    }

    inode_list = getInodeList(inode_number_of_file);

    for(int i = 0; i < 8; i++) {
        data_block = getDataBlock(*(inode_list.direct_address + i));
        for (int j = 0; j < strlen(data_block.contents); j++) {
            if(data_block.contents[j] != '\0') data_block.contents[j];
        }
    }

        for(int i = 0; i < 8; i++) {
        data_block = getDataBlock(*(inode_list.direct_address + i));
        strcat(file_content, data_block.contents);
    }

    printf("\n");
    return file_content;
}

void mycat(char** commands) {
    char* file_name = (char *)malloc(sizeof(char) * 7); 
    char* file_content = (char *)calloc(256 * 8, sizeof(char));
    int i = 0;
    int printlen = 0;
    if(*(commands + 1) == NULL) {
        errmsg("인자가 불충분합니다.\n");
        free(file_name);
        return ;
    }
    else if(strlen(*(commands + 1)) > 7) {
        errmsg("그런 파일이 없습니다.\n");
        free(file_name);
        return ;
    }
    strcpy(file_name, *(commands + 1));
    strcpy(file_content, getFileContentsWithSourceFileName(file_name, true));
    
    if(!strcmp(file_content, "그런 파일이 없습니다.\n")) {
        errmsg("그런 파일이 없습니다.\n");
        free(file_name);
        return ;
    }

    printlen = strlen(file_content) - (7 - strlen(file_content)/256);
    for(int i = 0; i < printlen; i++) {
        if(file_content[i] != '\0') printf("%c", file_content[i]);
    }
    printf("\n");
    free(file_content);
    }

void myshowfile(char** commands) {
    int block_address;
	DataBlock data_block;
	int i = 0;
    int num1 = 0;
    int num2 = 0;
    char file_name[7] = {0};
	
	if (commands[1] == NULL) {
		errmsg("인자가 불충분합니다.\n");
		return;
	}

	for (i = 0; commands[1][i] != '\0'; i++) {
		if (commands[1][i] < '0' || commands[1][i] > '9') {
			errmsg("범위는 양수여야 합니다.\n");
			return;
		}
	}

	if (i == 0) {
		errmsg("인자가 불충분합니다.\n");
		return;
	}

    if (commands[2] == NULL) {
		errmsg("인자가 불충분합니다.\n");
		return;
	}

    for (i = 0; commands[2][i] != '\0'; i++) {
		if (commands[2][i] < '0' || commands[2][i] > '9') {
			errmsg("범위는 양수여야 합니다.\n");
			return;
		}
	}

	if (i == 0) {
		errmsg("인자가 불충분합니다.\n");
		return;
	}

    if(commands[3] == NULL) {
        errmsg("인자가 불충분합니다.\n");
        return ;
    }

    else if(strlen(commands[3]) > 7) {
        errmsg("그런 파일이 없습니다.\n"); 
        return ;
    }
    
    num1 = strtol(commands[1], NULL, 10);
    num2 = strtol(commands[2], NULL, 10);
    strcpy(file_name, commands[3]);
    char* file_content = (char *)calloc(256 * 8, sizeof(char));
    strcpy(file_content, getFileContentsWithSourceFileName(file_name, true));

    if (num1 < 0 || num2 >= strlen(file_content) || num1 > num2) {
        printf("범위가 올바르지 않습니다.\n"); //임시
        free(file_content);
        return ;
    }


    if(!strcmp(file_content, "그런 파일이 없습니다.\n")) {
        printf("그런 파일이 없습니다.\n");
        //정적 문자열이므로 free 하지 않음
        return ;
    }

    for (i = num1 - 1; i <= num2 - 1; i++) {
        if(file_content[i] == '\0') {
            printf("범위가 올바르지 않습니다.\n");
            free(file_content);
            return ;
        }
    }

    for (i = num1 - 1; i <= num2 - 1; i++) {
          printf("%c", file_content[i]);
    }

    printf("\n");
    free(file_content);
}

void myrm(char** commands) {
    if(commands[1] == NULL) {
        printf("인자가 불충분합니다.");
        return ;
    }
    else if(strlen(commands[1]) > 7) {
        printf("파일 이름이 너무 깁니다.");
        return ;
    }

    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;

    for(int i = 0; i < 8; i++) {
        inode_list = getInodeList(getAllDirectAdressWithSourceFileName(commands[1], i));
        
    }
}

void mycp(char** commands) {
    char* source_file;
    char* dest_file;
    int inode_list_address_of_dest_file;

    if(commands[1] == NULL) {
        printf("인자가 불충분합니다."); 
        return ;
    }
    else if(strlen(commands[1]) > 7) {
        printf("그런 파일이 없습니다."); 
        return ;
    }
    if(commands[2] == NULL) {
        printf("인자가 불충분합니다."); 
        return ;
    }
    else if(strlen(commands[2]) > 7) {
        printf("파일 이름이 너무 깁니다.");
        return ;
    }

    source_file = malloc(sizeof(char) * strlen(commands[1]) + 1);
    dest_file = malloc(sizeof(char) * strlen(commands[2]) + 1);
    strcpy(source_file, commands[1]);
    strcpy(dest_file, commands[2]);
    SuperBlock super_block = getSuperBlock();
	InodeList inode_list;
    DataBlock data_block;
    char* source_file_content = (char *)calloc(256 * 8, sizeof(char));
    char* sliced_source_file_content = (char*)malloc(sizeof(char) * 256 * 8 - 1);
    strcpy(source_file_content, getFileContentsWithSourceFileName(source_file, true));
    getNeededDirectAdressNumber(source_file_content);
    inode_list_address_of_dest_file = allocateInodeForNewFiles(dest_file, getNeededDirectAdressNumber(source_file_content), strlen(source_file_content));
    strncpy(sliced_source_file_content, source_file_content, strlen(source_file_content) - (7 - (strlen(source_file_content) / 256)));
    printf("%s", sliced_source_file_content);
    writeFileContents(sliced_source_file_content, inode_list_address_of_dest_file, getNeededDirectAdressNumber(source_file_content));
    free(source_file);
    free(dest_file);
    free(source_file_content);
    free(sliced_source_file_content);
}

void mycpto(char** commands) {
    char* source_file;
    char* host_dest_file;

    if(commands[1] == NULL) {
        printf("인자가 불충분합니다."); 
        return ;
    }
    else if(strlen(commands[1]) > 7) {
        printf("그런 파일이 없습니다."); 
        return ;
    }
    if(commands[2] == NULL) {
        printf("인자가 불충분합니다."); 
        return ;
    }

    source_file = malloc(sizeof(char) * strlen(commands[1]) + 1);
    host_dest_file = malloc(sizeof(char) * strlen(commands[2]) + 1);
    char* source_file_content = (char*)malloc(sizeof(char) * 256 * 8);
    char* sliced_source_file_content = (char*)malloc(sizeof(char) * 256 * 8 - 1);

    strcpy(source_file, commands[1]);
    strcpy(host_dest_file, commands[2]);
    strcpy(source_file_content, getFileContentsWithSourceFileName(source_file, true));
    FILE *host_txt;
    getNeededDirectAdressNumber(source_file_content);
    strncpy(sliced_source_file_content, source_file_content, strlen(source_file_content) - (7 - (strlen(source_file_content) / 256)));

    host_txt = fopen(host_dest_file, "w+");
    if(host_txt == NULL) {
        printf("호스트 파일을 만들거나 열 수 없습니다.\n");
        return ;
    }
    for(int i = 0; i < strlen(sliced_source_file_content); i++) {
        fputc(sliced_source_file_content[i], host_txt);
    }
    fclose(host_txt);
    free(source_file);
    free(host_dest_file);
    free(source_file_content);
    free(sliced_source_file_content);
}

void mycpfrom(char** commands) {
    char* host_source_file;
    char* dest_file;

    if(commands[1] == NULL) {
        printf("인자가 불충분합니다."); //임시
        return ;
    }
    if(commands[2] == NULL) {
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
        printf("그런 호스트 파일이 없습니다.\n");
        return ;
    }

    fseek(host_txt, 0, SEEK_END);    
    host_txt_size = ftell(host_txt);   
    buffer = malloc(host_txt_size + 1);
    memset(buffer, 0, host_txt_size + 1); 
    fseek(host_txt, 0, SEEK_SET);
    file_count = fread(buffer, host_txt_size, 1, host_txt);    

    getNeededDirectAdressNumber(buffer);
    inode_list_address_of_dest_file = allocateInodeForNewFiles(dest_file, getNeededDirectAdressNumber(buffer), strlen(buffer));
    writeFileContents(buffer, inode_list_address_of_dest_file, getNeededDirectAdressNumber(buffer));

    fclose(host_txt);   
    free(buffer);  
    free(host_source_file);
    free(dest_file);
    // inode_list = getInodeList(working_directory.my_inode_number);
    // setInodeList(working_directory.my_inode_number, 1, inode_list.birth_date, 1, 1, inode_list.reference_count + 1,  inode_list.direct_address, inode_list.single_indirect_address);
    
}
