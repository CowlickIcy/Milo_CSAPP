CC = /usr/bin/gcc-9
CFLAGS = -Wall -g -O2 -Werror -std=gnu99

EXECUTABLE = csapp

SRC = ./src

CODE = ./src/memory/instruction.c ./src/disk/code.c ./src/memory/dram.c ./src/cpu/mmu.c ./src/main.c


.PHONY: csapp
csapp:
	$(CC) $(CFLAGS) -I$(SRC) $(CODE) -o $(EXECUTABLE)
	./$(EXECUTABLE)
clean:
	rm -f *.o *~$(EXECUTABLE)
