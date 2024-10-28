#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "filesystem.h"

extern SuperBlock g_SuperBlock;
extern InodeList g_InodeList[128];
extern DataBlock g_dataBlock[256];

void GetSuperBlockFromTxtFile() {
    FILE *inode_list_txt;
    inode_list_txt = fopen("inodelist.txt", "w");
    if(inode_list_txt = NULL) {
        printf("file error!");
    }

}

int main() {
    g_SuperBlock.inode_list_bit[0] = 0;
    
}