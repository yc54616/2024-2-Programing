#ifndef __SHELL_H__
#	define __SHELL_H__ 1

#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
#	include <stdbool.h>

#	include "data_struct.h"
#	include "fs_status_print.h"
#	include "commands2.h"

#	define string(a) #a
#	define commandIntoFunction(commandName) {#commandName, commandName} // struct g_command_list 만들기 용이하게 하기 위해 선언
// commands list
void print1(char **command); // 테스트용 함수
void echo(char **command);
void command(char **command);
void clear(char **command);
void Print_ID(char *computerId, char *userId);
void Print_WD();
void GetInput(char **inputString);
void InputIntoCommand(char *input, char **command);
int ExecuteCommand(char **command);
#endif
