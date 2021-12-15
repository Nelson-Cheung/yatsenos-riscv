#include "syscall.h"

extern "C" unsigned long start_syscall(unsigned long number, unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long a5, unsigned long a6);

unsigned long write(const char *s)
{
    return start_syscall(SYSCALL_WRITE, (unsigned long)s, 0, 0, 0, 0, 0);
}

void exit(long status)
{
    start_syscall(SYSCALL_EXIT, (unsigned long)status, 0, 0, 0, 0, 0);
}

void test()
{
    start_syscall(SYSCALL_TEST, 0, 0, 0, 0, 0, 0);
}