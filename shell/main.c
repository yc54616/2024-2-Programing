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

/*to-do: how to deal with parameters?
 $ print a bc
commands[2].command("a bc")
*/
#include "shell_system.h"

int main(void)
{
    // 선언들
    int index = 0;
    bool execution_result; // 명령어 실행 성공 여부
    char *inputString;
    char *command[500];                            // 배열의 한 칸이 char*으로, 하나의 단어를 지칭
    char *root_directory = "/";                     // root directory
    char *computer_id = "red", *user_id = "redmint"; // 컴퓨터 및 사용자 ID

    // 실행코드
    while (1)
    { // {1.ID및 WD출력   2.command 입력받기 실행하기} 반복
        Print_ID(computer_id, user_id);
        Print_WD(root_directory);

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
    }
}
// shell

