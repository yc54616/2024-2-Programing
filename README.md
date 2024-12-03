# 2024-2-Programing

- make 
- gcc -o main my_dir_commands.c system/data_struct.c system/io_stream.c shell.c hamjoohyuk_commands.c

> commands2.c 
> - #include : stdio, stdlib, string, data_struct.h, file_system.h, io_stream.h
> - mytouch, mymkdir, myls, mypwd, mycd, mytree

> data_struct.c 
> - #include : stdio, stdlib, string, file_system.h, io_stream.h
> - findDirectoryNameToInode, getExistence, _mycd, copyWorkingDirectory, clearVWD, cd
> - struct chainedDirectory, FunctionCallByString

> io_stream.c 
> - #include : stdbool, file_system.h
> - findEmptyInode, findEmptyDataBlock, writeDirectoryDataBlock, setBit, initFilesystem, 
> - setFilesystem, setSuperBlock, setInodeList, setDataBlock, getSuperBlock, 
> - getInodeList, getDataBlock

> shell.c 
> - #include : stdio, stdlib, string, stdbool, data_struct.h, fs_status_

> fs_status_print.c
> - #include : stdio, stdlib, file_system.h, io_stream.h
> - #define : errmsg
> - myinode, mydatablock, mystatus

> file_system.h
> - #define : FILENAME, SIZE_BOOTBLOCK etc
> - union : Byte, SuperBlock, InodeList, DataBlock

> get_file_contents/c
> - #include : stdio, stdlib, time, stdbool, string, file_syste,h, io_stream.h
> - getFileContentsWithSourceFileName

> hamjoohyuk_commands.c
> - #include : file_system, io_stream
> - myshowfile
> - getFileContentsWithSourecFileName
> - WriteFileContents
