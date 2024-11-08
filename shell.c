/*
라이선스 문구 ==> ?
*/

/*shell.c
 :터미널에서 사용자 입력을 받고, 이에 해당하는 명령어(함수)를 실행하는 코드이다.
 exit입력을 받기 전까지 코드가 실행된다. 입력을 받으면 g_command_list와 입력 문자열을
 비교하여 해당 명령어(함수)를 실행한다.

*/
/*입력과 실행
:typedef void (*func)(void)는 명령어 이름을 통해 함수를 호출하기 위해 정의하였다.(motive: 4장 ppt의 마지막 페이지 참조)
struct functionCall은 함수 이름(문자열)과 함수 이름(함수 호출)으로 구성된다. 함수 이름(문자열)을 입력된 문자열과 비교하여
함수 호출을 할 수 있다. #define cmd()는 이 구조체에 함수 이름을 넣기 용이하도록 선언하였다.
*/

/*명령어를 추가하는 방법
1. 함수 작성 - parameter: char** command로 통일해야함 (typdef 보기)
2. FunctionCallByString에 함수이름 추가하기
*/
/*to-do: how to deal with parameters?
 $ print a bc
commands[2].command("a bc")
*/

#include "shell.h"

/* definitions of global variable */
chainedDirectory *working_directory; // It makes a stack.
int depth_working_directory = 0; // It refers to the depth which is the number how many directories are there to reach here from '/'
/* example
 * /home/yhj/advanced_programming/project => 4
 * / => 0
 */

// commands
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
    @param void
    @return void
    */
    system("clear");
    return;
}

// commands list
struct FunctionCallByString g_command_list[500] =
    {
        commandIntoFunction(print1),
        commandIntoFunction(echo),
        commandIntoFunction(clear),
        commandIntoFunction(command),
    	commandIntoFunction(myinode),
    	commandIntoFunction(mydatablock),
    	commandIntoFunction(mystatus),
	commandIntoFunction(mypwd),
	commandIntoFunction(mycd),
	commandIntoFunction(mytree),
    commandIntoFunction(myls),
    commandIntoFunction(mymkdir)
    };

// funcions for shell system
// main에서 실행되는 순으로 정렬하였다
void Print_ID(char *computerId, char *userId)
{
    /**
     @brief 기기 및 사용자 ID 출력
     @param 기기ID, 사용자 ID
     @return void
     */
    printf("%s@%s:", computerId, userId);
}
void Print_WD()
{
    /**
 @brief WorkingDirectory출력; 로직이 추가되면 수정하기
 @param 로직이 추가되면 수정하기
 @return void
 */
    chainedDirectory *virtual_working_directory = working_directory;
    char (*linked_directories)[8];
    char c;
    int i;
    
    /* Coping real w.d into virtual w.d */
    linked_directories = (char (*)[8])malloc(sizeof(char (*)[8]) * depth_working_directory);
    for (i = 0; i < depth_working_directory; i++) {
        strncpy(*(linked_directories + i), virtual_working_directory -> my_name, 7);
	(*(linked_directories + i))[7] = '\0';
        virtual_working_directory = virtual_working_directory -> parent; // exploring
    }
    /* Now, the array consists of directories in descending order.
     * ex)  /as/df/gh
     *    => gh df as
     * index  0  1  2
     * So, we need to read this from backward.
     */
    for (i = depth_working_directory - 1; i >= 0; i--)
	    printf("/%s", *(linked_directories + i));
    if (depth_working_directory == 0)
	    printf("/");
    printf(" ");
    free(linked_directories);
}

void GetInput(char **inputString)
{
    /**
     @brief 문자열 입력받기 및 개행문자 제거(없으면 \n이 다음 반복 시에 영향을 준다)
     @param 입력받은 문자열의 주소를 저장할 char double pointer
     @return void
     */
    char input[1000];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0; // 개행문자 제거
    *inputString = input;            // 주소 넘겨주기
    return;
}
void InputIntoCommand(char *input, char **command)
{
    /**
     @brief input을 띄어쓰기 별로 나누어 command 배열에 저장
     @example input="cat hello.c" ==> command[0]="cat", command[1]="hello.c"
     @param input: 입력받은 문자열, command: 문자열을 띄어쓰기로 나누어 저장할 포인터배열
     @return void
     */
    int index = 0;
    command[index++] = strtok(input, " ");
    while (command[index - 1] != NULL && index < 20)
        command[index++] = strtok(NULL, " ");
    return;
}

int ExecuteCommand(char **command)
{
    /**
     @brief 해당 명령어 존재 여부 확인 및 실행
     @param 입력받은 명령어
     @return 실행 결과(성공:1, 실패:0)
     */
    int check = 0;
    for (int i = 0; i < 500 && g_command_list[i].name != NULL; i++)
        if (strcmp(command[0], g_command_list[i].name) == 0)
        {
            check = 1;
            g_command_list[i].command(command);
            break;
        }
    return check;
}

int main(void)
{
    // 선언들
    working_directory = (chainedDirectory *) malloc(sizeof(chainedDirectory));
    working_directory -> my_name[0] = '\0';
    working_directory -> my_inode_number = 1;
    working_directory -> parent = working_directory;
    depth_working_directory = 0;
    int index = 0;
    bool execution_result; // 명령어 실행 성공 여부
    char *inputString;
    char *command[500];                              // 배열의 한 칸이 char*으로, 하나의 단어를 지칭
    char *computer_id = "red", *user_id = "redmint"; // 컴퓨터 및 사용자 ID

    // 실행코드
    while (1)
    { // {1.ID및 WD출력   2.command 입력받기 실행하기} 반복
	/* 정상화 */
	printf("[");
        //Print_ID(computer_id, user_id);
        Print_WD();
	/* 의 신 */
	printf("]$ ");

        GetInput(&inputString);
        if (inputString[0] == '\0') // 입력값이 없을 경우 continue;
            continue;

        InputIntoCommand(inputString, command); // @example: input="cat hello.c"
                                                //==> command[0]="cat", command[1]="hello.c"

        execution_result = ExecuteCommand(command); // 명령어 실행 및 실행 결과 저장 1:성공, 0:실패

        if (execution_result == 0) // exit 명령어 처리 및 Command not found
        {
            if (strcmp(command[0], "exit") == 0)
                return 0; // 프로그램 종료
            printf("Command \"%s\" not found\n", command[0]);
            continue;
        }
	printf("\n");
    }
}
// shell
