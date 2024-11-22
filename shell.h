
#include "header.h"
#include "hamjoohyuk_commands.h"
#include "my_dir_commands.h"

#ifndef __SHELL_H__
#define __SHELL_H__ 1

#define commandIntoFunction(commandName) {#commandName, commandName} // struct g_command_list 만들기 용이하게 하기 위해 선언
// commands list
void print1(char **command); // 테스트용 함수
void echo(char **command);
void command(char **command);
void clear(char **command);
struct FunctionCallByString g_command_list[500] =
    {
        commandIntoFunction(print1),
        commandIntoFunction(echo),
        commandIntoFunction(clear),
        commandIntoFunction(myinode),
        commandIntoFunction(mydatablock),
        commandIntoFunction(mystatus),
        commandIntoFunction(mypwd),
        commandIntoFunction(mycd),
        commandIntoFunction(mytree),
        commandIntoFunction(mymkdir),
        commandIntoFunction(myls),
        commandIntoFunction(mytouch),
        commandIntoFunction(myrmdir),
        commandIntoFunction(mymkfs),
        commandIntoFunction(mycpfrom),
        commandIntoFunction(mycat),
        commandIntoFunction(myshowfile),
        commandIntoFunction(mycp),
        commandIntoFunction(mycpto),
    };
void Print_ID(char *computerId, char *userId);
void Print_WD();
void GetInput(char **inputString);
void InputIntoCommand(char *input, char **command);
int ExecuteCommand(char **command);
#endif
