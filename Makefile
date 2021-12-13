VPATH = driver include kernel user

PREFIX = riscv64-unknown-linux-gnu
CXX = $(PREFIX)-g++
INCLUDE = include
CXX_FLAGS = -c -g -Wall -nostdlib -I$(INCLUDE) -mcmodel=medany -ffreestanding -nostartfiles


AS = $(PREFIX)-as
LD = $(PREFIX)-ld

TARGET = kernel.elf
OBJ = start.o
OBJ += driver.o uart.o init.o asm_utils.o interrupt.o clint.o timer.o utils.o \
address_pool.o bitmap.o mem.o \
list.o process.o proc_zero.o \
syscall_manager.o

$(TARGET) : $(OBJ)
	$(LD) $^ -Ttext 0x80000000 -e _start -o $@
	# $(LD) $^ -T kernel.ld -o $@

syscall_manager.o: syscall_manager.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

list.o: list.cpp list.h
	$(CXX) $(CXX_FLAGS) $< -o $@

process.o: process.cpp process.h pcb.h
	$(CXX) $(CXX_FLAGS) $< -o $@

proc_zero.o: proc_zero.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@

address_pool.o: address_pool.cpp address_pool.h
	$(CXX) $(CXX_FLAGS) $< -o $@

bitmap.o: bitmap.cpp bitmap.h
	$(CXX) $(CXX_FLAGS) $< -o $@

mem.o: mem.cpp mem.h
	$(CXX) $(CXX_FLAGS) $< -o $@
	
utils.o: utils.cpp utils.h
	$(CXX) $(CXX_FLAGS) $< -o $@

timer.o: timer.cpp timer.h
	$(CXX) $(CXX_FLAGS) $< -o $@

clint.o: clint.cpp clint.h
	$(CXX) $(CXX_FLAGS) $< -o $@

interrupt.o: interrupt.cpp interrupt.h
	$(CXX) $(CXX_FLAGS) $< -o $@

asm_utils.o: asm_utils.s
	$(AS) -g $< -o $@

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
	clear
	qemu-system-riscv64 -machine virt -kernel $(TARGET) -bios none -nographic

clean :
	rm -fr *.o *.elf