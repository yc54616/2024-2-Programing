#ifndef __DATA_STRUCT_H__
#	define __DATA_STRUCT_H__ 1
typedef struct chainedDirectory {
	unsigned char my_name[7];
	unsigned char my_inode_number;

	struct chainedDirectory *parent;
} chainedDirectory; // It has parent pointing to its parent, .. goes on the end, finally we can arrives at the root.    
typedef void (*func)(char **);
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
#endif
