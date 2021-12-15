#include "syscall.h"
#include "syscall_manager.h"
#include "utils.h"

unsigned long SystemCallManager::handle_syscall(unsigned long a0, unsigned long a1,
                                                unsigned long a2, unsigned long a3,
                                                unsigned long a4, unsigned long a5,
                                                unsigned long a6, unsigned long a7)
{
    switch (a0)
    {
    case SYSCALL_WRITE:
        do_write((const char *)a1);
        break;
    
    case SYSCALL_EXIT:
        do_exit(a1);
        break;

    default:
        printf("unhandled interrrupt\n");
        break;
    }
}

unsigned long SystemCallManager::do_write(const char *s)
{
    printf("%s", s);
    return 0;
}

void SystemCallManager::do_exit(long status)
{
    printf("process exit\n");
    while (true)
        ;
}