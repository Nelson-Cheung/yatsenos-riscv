#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL_NUMBER 32UL

#define SYSCALL_WRITE 0UL
#define SYSCALL_EXIT 1UL

unsigned long write(const char *s);
unsigned long do_write(const char *s);

void exit(long status);
void do_exit(long status);

#endif