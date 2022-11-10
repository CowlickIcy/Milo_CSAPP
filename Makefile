CC = /usr/bin/gcc-9
CFLAGS = -Wall -g -O2 -Werror -std=gnu99 -Wno-unused-function 

EXECUTABLE = csapp

SRC = ./src

# debug
COMMON = $(SRC)/common/print.c $(SRC)/common/convert.c

# hardware
CPU = $(SRC)/hardware/cpu/mmu.c $(SRC)/hardware/cpu/isa.c 
MEMORY = $(SRC)/hardware/memory/dram.c

# main
# TESTHARDWARE = $(SRC)/tes



.PHONY: hardware
hardware:
	$(CC) $(CFLAGS) -I$(SRC) $(COMMON) $(CPU) $(MEMORY) -o $(EXECUTABLE)
	./$(EXECUTABLE)
clean:
	rm -f *.o *~$(EXECUTABLE)
