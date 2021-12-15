#ifndef OBJECT_H
#define OBJECT_H

#include "driver.h"
#include "mem.h"
#include "syscall_manager.h"

extern class Driver driver;
extern MemoryManager memory_manager;
extern SystemCallManager syscall_manager;
extern ProcessManager process_manager;

#endif