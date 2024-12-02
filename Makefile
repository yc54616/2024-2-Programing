CC=gcc
CFLAGS=-Wall -g
SOURCE=my_dir_commands.c system/data_struct.c system/io_stream.c shell.c hamjoohyuk_commands.c
OBJ=$(SOURCE:.c=.o)
EXE=myfs_shell

all: $(EXE)
	echo 'execution file "myfs_shell" made'
	rm -rf $(OBJ)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -g -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(OBJ) $(EXE)

.PHONY: clean
