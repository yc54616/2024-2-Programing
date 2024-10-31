#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define string(a) #a
#define cmd(commandName) {#commandName, commandName} //struct commandList 만들기 용이하게 하기 위해 선언 
typedef void (*func)(void); //함수 호출을 용이하게 하기 위해 선언 
void print1(void)
{
    printf("1 1 1 1\n");
    return;
}
void print2(void)
{
    printf("2 2 2 2\n");
    return;
}
void exit1(void){   //:error: conflicting types for ‘exit’; have ‘void(void)’
    printf("exit\n");
    system("kill -INT 888");
    return;
}
void clear(void){
    system("clear");
    return;
}

struct functionCall     //
{
    char name[30];
    func command;
};


/* $ print a bc
commands[2].command("a bc")*/

int main(void)
{
    int index = 0, check;
    char input[1000];
    char *command[500];
    char *wd = "/"; // working directory
    char *computerId = "red", *id = "redmint";
    struct functionCall commandList[500] = {
        cmd(print1),
        cmd(print2),
        cmd(exit1),
        cmd(clear)
    };
    while (1)
    {                                     // command 입력 받기 반복
        printf("%s@%s:", computerId, id); // id출력
        {                                 // workig directory 출력
            printf("%s$ ", wd);
        }

        fgets(input, sizeof(input), stdin); // input 입력받기
        input[strcspn(input, "\n")] = 0;
        //        printf("1\n");
        if (input[0] == '\0') // 입력X시 continue;
            continue;

        index = 0;
        command[index++] = strtok(input, " ");           // 띄어쓰기 기준으로 나누어 저장
        while (command[index - 1] != NULL && index < 20) // input --> command[]
            command[index++] = strtok(NULL, " ");

        check = 0;
	for (int i = 0; i < 500 && commandList[i].name != NULL; i++) // 아이디어: 4장 ppt의 마지막 페이지 참조
 	{
	  if (strcmp(command[0], commandList[i].name) == 0)
            { // command exist?
                check = 1;
                // system(commandList[i]);  //==>이런식으로 함수 실행 가능?
                commandList[i].command();
                break;
            }
        }

        if (check == 0)
        {                                        // 명령어 X 시
            if (strcmp(command[0], "exit") == 0) // exit : 더 효율적으로 할 수 있는 방법?
            {                                    //exit을 명령어로 처리 시, conflicting error 발생   
                printf("exit\n");
                return 0;
            }
            printf("Command \"%s\" not found\n", command[0]);
            continue;
        }
    }
}
//shell 
