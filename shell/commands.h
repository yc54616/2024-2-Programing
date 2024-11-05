/* commands.h
exit을 제외한 명령어들 각각을 함수로써 저장하였다.

*/
void print1(char **command) // 테스트용 함수
{
    printf("1 1 1 1\n");
    return;
}
void echo(char **command)
{
    /**
    @brief echo. bash에 echo명령어 실행
    @param command
    @return void
    */
    int index = 1;
    while (command[index] != NULL)
    {
        printf("%s ", command[index++]);
    }
    printf("\n");
    return;
}
void command(char **command)
{
    /**
    @brief bash에 명령어를 실행한다. cmd에 command[i]를 이어붙이고 system을 통해 bash 명령어 처리하였다.
    @param void
    @return void
    */
    char cmd[50];
    strcpy(cmd, command[1]);                 // cmd = command[1]으로 한다면, cmd에 command[]의 주소가 들어가서
    for (int i = 2; command[i] != NULL; i++) // strcat(cmd, " ") 다음에 command[2]=" "로 바뀜
    {
        strcat(cmd, " ");
        strcat(cmd, command[i]);
    }
    // printf("cmd: %s\n", cmd);
    system(cmd);
    return;
}
void clear(char **command)
{
    /**
 @brief clear. bash에 clear명령어 실행
 @param command
 @return void
 */
    system("clear");
    return;
}
