# 2024-2-Programing


현재 gcc하는 법:  
gcc -o main1 my_dir_commands.c my_file_commands.c system/data_struct.c system/io_stream.c shell.c

error found:
1.
디렉토리와 이름이 같은 파일을 만들 수 없음
혹은 디렉토리와 이름이 같으면 표시되지 않는듯

2.
[/ ]$ mytree
.
├──
├──
├── 2192
│   ├──
│   ├──
│   └── 93
│       ├──
│       └──
├── 24
│   ├──
│   └──
├── 293
│   ├──
│   └──
├── 93
│   ├──
│   └──
├── aks
│   ├──
│   └──
└── asd
    ├──
    └──

[/ ]$ mypwd
/

[/ ]$ mytouch my_touch_commands_
*** stack smashing detected ***: terminated
Aborted (core dumped)


파일들의 include, defnie, struct, union, function 

:commands2.c 
-include:stdio, stdlib, string, data_struct.h, file_system.h, io_stream.h
-mytouch, mymkdir, myls, mypwd, mycd, mytree

:data_struct.c 
-include: stdio, stdlib, string, file_system.h, io_stream.h
-findDirectoryNameToInode, getExistence, _mycd, copyWorkingDirectory, clearVWD, cd
-struct chainedDirectory, FunctionCallByString

:io_stream.c 
-include: stdbool, file_system.h
-findEmptyInode, findEmptyDataBlock, writeDirectoryDataBlock, setBit, initFilesystem, 
setFilesystem, setSuperBlock, setInodeList, setDataBlock, getSuperBlock, 
getInodeList, getDataBlock


:shell.c 
-include: stdio, stdlib, string, stdbool, data_struct.h, fs_status_


:fs_status_print.c
-include: stdio, stdlib, file_system.h, io_stream.h
-define: errmsg
-myinode, mydatablock, mystatus


:file_system.h
-define: FILENAME, SIZE_BOOTBLOCK etc
-union: Byte, SuperBlock, InodeList, DataBlock


:get_file_contents/c
-include: stdio, stdlib, time, stdbool, string, file_syste,h, io_stream.h
-getFileContentsWithSourceFileName

:hamjoohyuk_commands.c
-include: file_system, io_stream
-myshowfile
-getFileContentsWithSourecFileName
-WriteFileContents


계획: 
1. header.h파일에 include 파일 모으기
2. my_directory_commands.c & my_file_commands.c 로 분류 

총 io_stream, data_struct, file_system, 
my_directory_commands, my_file_commands, 
header.h
shell 파일로 구성할 것

