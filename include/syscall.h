#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL_NUMBER 32UL

#define SYSCALL_TEST 0UL
#define SYSCALL_WRITE 1UL
#define SYSCALL_EXIT 2UL
#define SYSCALL_FORK 3UL
#define SYSCALL_WAIT 4UL

void test();
unsigned long write(const char *s);
void exit(long status);
unsigned long fork();

#endif