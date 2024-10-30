#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

void CheckTxtfilePresenceOrAbsence() {

}


void GetSuperBlockFromTxtFile() {
    FILE *super_block_txt;
    super_block_txt = fopen("super-block.txt", "r");
    if(super_block_txt == NULL) printf("file open failed"); //test
    fgets(s_SuperBlock.inode_list_bit, 129, super_block_txt); //fgets(char *string, int n, FILE *stream);
    fgets(s_SuperBlock.data_block_bit, 257, super_block_txt); //256 + 1('\0' included)
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

void GetInodeListFromTxtFile() {
    FILE *inode_list_txt;
    inode_list_txt = fopen("inode-list.txt", "r");
    if(inode_list_txt == NULL) printf("file open failed");
}

int main() {
    GetSuperBlockFromTxtFile();
    printf("!"); //test
    SaveSuperBlockFromTxtFile();
}