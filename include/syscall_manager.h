#ifndef SYSCALL_MANAGER_H
#define SYSCALL_MANAGER_H

class SystemCallManager
{
public:
    unsigned long handle_syscall(unsigned long a0, unsigned long a1,
                                 unsigned long a2, unsigned long a3,
                                 unsigned long a4, unsigned long a5,
                                 unsigned long a6, unsigned long a7);

private:
    unsigned long do_write(const char *s);
    void do_exit(long status);
    void do_test();
    void do_fork();
    void do_wait();
};

#endif