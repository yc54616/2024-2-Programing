/*
exit을 제외한 명령어들 각각을 함수로써 저장하였다.

*/
void print1(void) // 테스트용 함수
{
    printf("1 1 1 1\n");
    return;
}
void print2(void)
{
    printf("2 2 2 2\n");
    return;
}
void clear(void)
{
    /**
 @brief clear. bash에 clear명령어 실행
 @param void
 @return void
 */ 
    system("clear");
    return;
}
