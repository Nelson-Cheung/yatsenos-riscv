VPATH = 

PREFIX = riscv64-unknown-linux-gnu
CXX = $(PREFIX)-g++
INCLUDE = 
CXX_FLAGS = -g -Wall -nostdlib -O0 -I$(INCLUDE) -fno-builtin -ffreestanding -fno-pic -mcmodel=medany


AS = $(PREFIX)-as
LD = $(PREFIX)-ld

TARGET = 
OBJ = gen_binary.out proc_zero.cpp


all: $(OBJ)

gen_binary.out: gen_binary.cpp
	g++ $< -o $@

proc_zero.cpp: zero.cpp gen_binary.out
	$(CXX) $(CXX_FLAGS) zero.cpp -e main -o zero.elf
	./gen_binary.out zero.elf proc_zero.cpp zero
	

clean :
	rm -fr *.o *.elf