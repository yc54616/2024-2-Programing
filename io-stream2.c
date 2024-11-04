#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "filesystem.h"

typedef struct {
    char inode_list_bit[128];
    char data_block_bit[256];
} ConvertedSuperBlock;

SuperBlock s_SuperBlock;
ConvertedSuperBlock s_ConvertedSuperBlock;
InodeList s_InodeList[128];
DataBlock s_dataBlock[256];

void SetConvertedSuperBlockStructure() {
    for(int i = 0; i < 16; i++) {
        s_ConvertedSuperBlock.inode_list_bit[i * 8] = s_SuperBlock.inode_list[i].b0?'1':'0';
        s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 1] = s_SuperBlock.inode_list[i].b1?'1':'0';
        s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 2] = s_SuperBlock.inode_list[i].b2?'1':'0';
        s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 3] = s_SuperBlock.inode_list[i].b3?'1':'0';
        s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 4] = s_SuperBlock.inode_list[i].b4?'1':'0';
        s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 5] = s_SuperBlock.inode_list[i].b5?'1':'0';
        s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 6] = s_SuperBlock.inode_list[i].b6?'1':'0';
        s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 7] = s_SuperBlock.inode_list[i].b7?'1':'0';
    }
    for(int i = 0; i < 32; i ++) {
        s_ConvertedSuperBlock.data_block_bit[i * 8] = s_SuperBlock.data_block[i].b0?'1':'0';
        s_ConvertedSuperBlock.data_block_bit[(i * 8) + 1] = s_SuperBlock.data_block[i].b1?'1':'0';
        s_ConvertedSuperBlock.data_block_bit[(i * 8) + 2] = s_SuperBlock.data_block[i].b2?'1':'0';
        s_ConvertedSuperBlock.data_block_bit[(i * 8) + 3] = s_SuperBlock.data_block[i].b3?'1':'0';
        s_ConvertedSuperBlock.data_block_bit[(i * 8) + 4] = s_SuperBlock.data_block[i].b4?'1':'0';
        s_ConvertedSuperBlock.data_block_bit[(i * 8) + 5] = s_SuperBlock.data_block[i].b5?'1':'0';
        s_ConvertedSuperBlock.data_block_bit[(i * 8) + 6] = s_SuperBlock.data_block[i].b6?'1':'0';
        s_ConvertedSuperBlock.data_block_bit[(i * 8) + 7] = s_SuperBlock.data_block[i].b7?'1':'0';
    }
}

void SetOriginalSuperBlockStructure() {
    for(int i = 0; i < 16; i++) {
        s_SuperBlock.inode_list[i].b0 = s_ConvertedSuperBlock.inode_list_bit[i * 8];
        s_SuperBlock.inode_list[i].b1 = s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 1];
        s_SuperBlock.inode_list[i].b2 = s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 2];
        s_SuperBlock.inode_list[i].b3 = s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 3];
        s_SuperBlock.inode_list[i].b4 = s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 4];
        s_SuperBlock.inode_list[i].b5 = s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 5];
        s_SuperBlock.inode_list[i].b6 = s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 6];
        s_SuperBlock.inode_list[i].b7 = s_ConvertedSuperBlock.inode_list_bit[(i * 8) + 7];
    }
    for(int i = 0; i < 32; i ++) {
        s_SuperBlock.data_block[i].b0 = s_ConvertedSuperBlock.data_block_bit[i * 8];
        s_SuperBlock.data_block[i].b1 = s_ConvertedSuperBlock.data_block_bit[(i * 8) + 1];
        s_SuperBlock.data_block[i].b2 = s_ConvertedSuperBlock.data_block_bit[(i * 8) + 2];
        s_SuperBlock.data_block[i].b3 = s_ConvertedSuperBlock.data_block_bit[(i * 8) + 3];
        s_SuperBlock.data_block[i].b4 = s_ConvertedSuperBlock.data_block_bit[(i * 8) + 4];
        s_SuperBlock.data_block[i].b5 = s_ConvertedSuperBlock.data_block_bit[(i * 8) + 5];
        s_SuperBlock.data_block[i].b6 = s_ConvertedSuperBlock.data_block_bit[(i * 8) + 6];
        s_SuperBlock.data_block[i].b7 = s_ConvertedSuperBlock.data_block_bit[(i * 8) + 7];
    }
}

void SetInitSuperBlock() { //SuperBlock을 초기값으로 설정
    FILE *super_block_txt;
    super_block_txt = fopen("super-block.txt", "w");
    char super_block_bit[384] = {1,};
    //for(int i = 1; i < 384; i++) super_block_bit[i] = '0';
    if(super_block_txt == NULL) printf("file open failed"); //test
    fputs(super_block_bit, super_block_txt);
}

void CheckTxtfilePresenceOrAbsence() { //파일 오픈 시, 파일 존재 여부 체크
    FILE *super_block_txt, *inode_list_txt, *data_block_txt;
    super_block_txt = fopen("super-block.txt", "r");
    inode_list_txt = fopen("inode-list.txt", "r");
    data_block_txt = fopen("data-block.txt", "r");
    if(super_block_txt == NULL) {
        SetInitSuperBlock();
    }
    if(inode_list_txt == NULL) fopen("inode-list.txt", "w"); //TODO::SetInitInodeList 함수 추가
    if(data_block_txt == NULL) fopen("data-block.txt", "w"); //TODO::SetInitDataBlock 함수 추가
}


void GetSuperBlockFromTxtFile() {
    FILE *super_block_txt;
    super_block_txt = fopen("super-block.txt", "r");
    char full_txt_file[384] = {0,}; 
    if(super_block_txt == NULL) printf("file open failed"); //test
    fgets(full_txt_file, 385, super_block_txt); //fgets(char *string, int n, FILE *stream);, //384 + 1('\0' 포함)
    fclose(super_block_txt);
    for(int i = 0; i < 128; i++) {
        s_ConvertedSuperBlock.inode_list_bit[i] = full_txt_file[i];
    }
    for(int i = 0; i < 256; i++) {
        s_ConvertedSuperBlock.data_block_bit[i] = full_txt_file[i + 128];
    }
    printf("%c", s_ConvertedSuperBlock.inode_list_bit[0]); //test
    printf("%c", s_ConvertedSuperBlock.data_block_bit[0]); //test
}

void SaveSuperBlockFromTxtFile() {
    FILE *super_block_txt;
    super_block_txt = fopen("super-block.txt", "w");
    if(super_block_txt == NULL) printf("file open failed"); //test
    fputs(s_ConvertedSuperBlock.inode_list_bit, super_block_txt); //fputs(const char* str, FILE* stream);
    //fputs(s_ConvertedSuperBlock.data_block_bit, super_block_txt);
    fclose(super_block_txt);
}

void GetInodeListFromTxtFile() { //TODO::전체적인 구조 확정 이후 완성할 예정
    FILE *inode_list_txt;
    inode_list_txt = fopen("inode-list.txt", "r");
    if(inode_list_txt == NULL) printf("file open failed");
}

int main() { //for test
    CheckTxtfilePresenceOrAbsence();
    GetSuperBlockFromTxtFile();
    printf("!"); //for test
    SetOriginalSuperBlockStructure();
    for(int i = 0; i < 48; i++) printf("%u", s_SuperBlock.inode_list[i].b0); //test
    //명령어 수행
    SetConvertedSuperBlockStructure();
    SaveSuperBlockFromTxtFile();
}