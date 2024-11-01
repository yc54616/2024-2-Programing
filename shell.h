#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "fs_status_print.h"

#ifndef __SHELL__
#	define __SHELL__
#	define string(a) #a
#	define commandIntoFunction(commandName) {#commandName, commandName} // struct g_command_list 만들기 용이하게 하기 위해 선언
typedef void (*func)(char **); // 함수 호출을 용이하게 하기 위해 선언: param이 필요한 것들은 추가로 typedef하기
struct FunctionCallByString
{
    /**
    @brief 함수 호출을 입력받은 문자열을 통해 하기 위해 사용한다.
    name은 명령어(함수)의 이름(문자열), command는 명령어(함수) 호출이다.
    name == #command
    */
    char name[30];
    func command;
};
// commands list
void print1(char **command); // 테스트용 함수
void echo(char **command);
void command(char **command);
void clear(char **command);
void Print_ID(char *computerId, char *userId);
void Print_WD(char *wd);
void GetInput(char **inputString);
void InputIntoCommand(char *input, char **command);
int ExecuteCommand(char **command);
#endif
