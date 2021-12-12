VPATH = ../include ../lib

PREFIX = riscv64-unknown-linux-gnu
CXX = $(PREFIX)-g++
AS = $(PREFIX)-as
LD = $(PREFIX)-ld

INCLUDE = ../include/
CXX_FLAGS = -g -Wall -nostdlib -O0 -I$(INCLUDE) -fno-builtin -ffreestanding -fno-pic -mcmodel=medany -c
TEXT = 0x200001000

AS = $(PREFIX)-as
LD = $(PREFIX)-ld

TARGET = 
OBJ = proc_zero.cpp


all: $(OBJ)

lib_utils.o: lib_utils.s
	$(AS) -g $< -o $@

syscall.o: syscall.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

gen_binary.out: gen_binary.cpp
	g++ $< -o $@

zero.o: zero.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

proc_zero.elf: zero.o syscall.o lib_utils.o
	$(LD) $^ -e main -Ttext $(TEXT) -o $@

proc_zero.cpp: proc_zero.elf gen_binary.out
	./gen_binary.out $< $@ zero
	

clean :
	rm -fr *.o *.elf proc_*.cpp