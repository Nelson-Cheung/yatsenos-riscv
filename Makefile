VPATH = driver include kernel

PREFIX = riscv64-unknown-linux-gnu
CXX = $(PREFIX)-g++
INCLUDE = include
CXX_FLAGS = -c -g -Wall -nostdlib -O0 -I$(INCLUDE) -fno-builtin -ffreestanding -fno-pic


AS = $(PREFIX)-as
LD = $(PREFIX)-ld

TARGET = kernel.elf
OBJ = start.o
OBJ += driver.o uart.o init.o 

$(TARGET) : $(OBJ)
	$(LD) $^ -Ttext 0x80000000 -e _start -o $@

driver.o : driver.cpp driver.h
	$(CXX) $(CXX_FLAGS) $< -o $@

uart.o : uart.cpp uart.h
	$(CXX) $(CXX_FLAGS) $< -o $@

init.o : init.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

start.o : start.s 
	$(AS) -g $< -o $@

debug : $(TARGET)
	qemu-system-riscv64 -machine virt -kernel $(TARGET) -bios none -nographic -s -S

run : $(TARGET)
	qemu-system-riscv64 -machine virt -kernel $(TARGET) -bios none -nographic

clean :
	rm -fr *.o *.elf