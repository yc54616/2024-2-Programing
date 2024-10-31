/*shell_system.h
쉘 시스템을 구성하는 헤더파일, 전처리기, 구조체, 함수들이다.
자세한 설명은 shell.c를 참조하면 된다.
commands.h를 포함한다. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "commands.h"
#define string(a) #a
#define commandIntoFunction(commandName) {#commandName, commandName} // struct g_command_list 만들기 용이하게 하기 위해 선언

typedef void (*func)(char **); // 함수 호출을 용이하게 하기 위해 선언: param이 필요한 것들은 추가로 typedef하기

struct FunctionCallByString
{    /**
    @brief 함수 호출을 입력받은 문자열을 통해 하기 위해 사용한다.
    name은 명령어(함수)의 이름(문자열), command는 명령어(함수) 호출이다.
    name == #command
    */
    char name[30];
    func command;
};

// commands list
struct FunctionCallByString g_command_list[500] =
    {
        commandIntoFunction(print1),
        commandIntoFunction(echo),
        commandIntoFunction(clear)};

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
void Print_WD(char *wd)
{
    /**
 @brief WorkingDirectory출력; 로직이 추가되면 수정하기
 @param 로직이 추가되면 수정하기
 @return void
 */
    printf("%s$ ", wd);
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
