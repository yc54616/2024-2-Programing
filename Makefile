CC=gcc
CFLAGS=-Wall -g
SOURCE=my_dir_commands.c system/data_struct.c system/io_stream.c shell.c hamjoohyuk_commands.c
OBJ=$(SOURCE:.c=.o)
EXE=fs

all: $(EXE)
	echo 'execution file "fs" made'
	rm -rf $(OBJ)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(OBJ) $(EXE)

.PHONY: clean
