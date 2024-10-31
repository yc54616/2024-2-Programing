/*
exit을 제외한 명령어들 각각을 함수로써 저장하였다.

*/
void print1(char** command) // 테스트용 함수
{
    printf("1 1 1 1\n");
    return;
}
void echo(char** command)
{
    int index = 1;
    while(command[index]!=NULL)
        printf("%s ", command[index]);
    printf("\n");
    return;
}
void clear(char** command)
{
    /**
 @brief clear. bash에 clear명령어 실행
 @param void
 @return void
 */ 
    system("clear");
    return;
}
