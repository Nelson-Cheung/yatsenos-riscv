#include "syscall.h"
#include "syscall_manager.h"
#include "utils.h"
#include "object.h"
#include "process.h"

unsigned long SystemCallManager::handle_syscall(unsigned long a0, unsigned long a1,
                                                unsigned long a2, unsigned long a3,
                                                unsigned long a4, unsigned long a5,
                                                unsigned long a6, unsigned long a7)
{
    switch (a0)
    {
    case SYSCALL_TEST:
        do_test();
        break;

    case SYSCALL_WRITE:
        do_write((const char *)a1);
        break;

    case SYSCALL_EXIT:
        do_exit(a1);
        break;

    case SYSCALL_FORK:
        return do_fork();
        break;

    default:
        printf("unhandled interrrupt\n");
        while (true)
            ;
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
    printf("process %ld exit\n", process_manager.current_running_process->pid);
    process_manager.current_running_process->status = ProcessStatus::DEAD;
    process_manager.schedule();
}

void SystemCallManager::do_test()
{
    process_manager.schedule();
}

unsigned long SystemCallManager::do_fork()
{
    unsigned long pid = process_manager.create_process(nullptr);
    if (pid == -1UL)
    {
        return -1UL;
    }

    PCB *child = process_manager.find_process_by_pid(pid);
    bool flag = process_manager.copy_process(process_manager.current_running_process, child);

    if (!flag)
    {
        child->status = ProcessStatus::DEAD;
        return -1UL;
    }

    return pid;
}