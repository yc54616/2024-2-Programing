#include <stdio.h>
#include <stdlib.h>

typedef struct _Date 
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
} Date;
typedef struct _SuperBlock
{
        char inode_list_bit[128];
        char data_block_bit[256];
} SuperBlock;
typedef struct _InodeList
{
    int type; 
    Date date;
    int size;  
} InodeList;
typedef struct _DataBlock
{
    int block;
} DataBlock;

SuperBlock s_SuperBlock;
InodeList s_InodeList[128];
DataBlock s_dataBlock[256];

void SetInitSuperBlock() { //SuperBlock을 초기값으로 설정
    FILE *super_block_txt;
    super_block_txt = fopen("super-block.txt", "w+");
    char super_block_bit[384] = {1,}; //첫 번째 비트는 root 디렉토리로 고정됨, 나머지 비트 모두 0으로 설정
    if(super_block_txt == NULL) printf("file open failed"); //test
    fputs(super_block_bit, super_block_txt);
}

void CheckTxtfilePresenceOrAbsence() { //파일 오픈 시, 파일 존재 여부 체크
    FILE *super_block_txt, *inode_list_txt, *data_block_txt;
    super_block_txt = fopen("super-block.txt", "r");
    inode_list_txt = fopen("inode-list.txt", "r");
    data_block_txt = fopen("data_block.txt", "r");
    if(super_block_txt == NULL) {
        fopen("super-block.txt", "w+");
        SetInitSuperBlock();
    }
    if(inode_list_txt == NULL) fopen("inode-list.txt", "w+"); //TODO::SetInitInodeList 함수 추가
    if(data_block_txt == NULL) fopen("data_block.txt", "w+"); //TODO::SetInitDataBlock 함수 추가
}


void GetSuperBlockFromTxtFile() {
    FILE *super_block_txt;
    super_block_txt = fopen("super-block.txt", "r");
    if(super_block_txt == NULL) printf("file open failed"); //test
    fgets(s_SuperBlock.inode_list_bit, 129, super_block_txt); //fgets(char *string, int n, FILE *stream);
    fgets(s_SuperBlock.data_block_bit, 257, super_block_txt); //256 + 1('\0' 포함)
    printf("%c", s_SuperBlock.inode_list_bit[0]); //test
    printf("%c", s_SuperBlock.data_block_bit[0]); //test
    fclose(super_block_txt);
}

void SaveSuperBlockFromTxtFile() {
    FILE *super_block_txt;
    super_block_txt = fopen("super-block.txt", "w");
    if(super_block_txt == NULL) printf("file open failed"); //test
    fputs(s_SuperBlock.inode_list_bit, super_block_txt); //fputs(const char* str, FILE* stream);
    fputs(s_SuperBlock.data_block_bit, super_block_txt);
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
    printf("!"); //임시
    SaveSuperBlockFromTxtFile();
}