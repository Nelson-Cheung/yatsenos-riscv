#include "driver.h"
#include "rv64.h"
#include "interrupt.h"
#include "utils.h"
#include "mem.h"
#include "constant.h"
#include "process.h"
#include "syscall_manager.h"

unsigned long stack0[4096];
unsigned long mstack0[4096];
unsigned long sstack0[4096];

Driver driver;
MemoryManager memory_manager;
ProcessManager process_manager;
SystemCallManager syscall_manager;

extern "C" void rv64_kernel_init()
{
    driver.initialize();
    driver.clint.set_interrupt_handler((unsigned long)asm_interrupt_handler);

    // printf("%lx\n", &driver);
    // printf("%lx\n", &memory_manager);
    // printf("%lx\n", &process_manager);

    memory_manager.initialize();
}

extern unsigned char zero[];

#include "syscall.h"

extern "C" void kernel_entry()
{
    rv64_kernel_init();

    printf(

        "######  ###  #####   #####        #     #\n"
        "#     #  #  #     # #     #       #     #\n"
        "#     #  #  #       #             #     #\n"
        "######   #   #####  #       ##### #     #\n"
        "#   #    #        # #              #   #\n"
        "#    #   #  #     # #     #         # #\n"
        "#     # ###  #####   #####           #\n"

    );

    process_manager.initialize();
    unsigned long pid = process_manager.create_process((const char *)zero);
    // printf("zero process: %ld\n", pid);

    if (pid)
    {
        return;
    }

    pid = process_manager.create_process((const char *)zero);
    printf("next process: %ld, process list size: %d\n", pid, process_manager.ready_process.size());
    if (pid == -1UL)
    {
        return;
    }

    pid = process_manager.create_process((const char *)zero);
    printf("next process: %ld, process list size: %d\n", pid, process_manager.ready_process.size());
    if (pid == -1UL)
    {
        return;
    }

    // while(true);

    driver.clint.enable_interrupt();
    driver.clint.enable_timer_interrupt();
    unsigned long mtime, mtimecmp;
    mtime = driver.timer.read_mtime();
    driver.timer.write_mtimecmp(mtime + 0x2ffffffUL);
    driver.clint.disable_interrupt();
    // while(true);

    PCB *pcb = ListItem2PCB(process_manager.ready_process.front(), schedule_tag);
    PCB none;

    process_manager.ready_process.pop_front();
    pcb->status = ProcessStatus::RUNNING;
    process_manager.set_l2_page_table(pcb->l2_page_table);
    process_manager.current_running_process = pcb;
    switch_to(&none, pcb);

    return;

    printf("initialization finish\n");

    printf("print percentage: %%\n"
           "print char \"N\": %c\n"
           "print string \"Hello World!\": %s\n"
           "print decimal: \"-1234\": %d\n"
           "print hexadecimal \"0x7abcdef0\": %x\n"
           "print long decimal: \"-122147483647\": %ld\n"
           "pritn long hexadecimal \"0x123456789a\": %lx\n",
           'N', "Hello World!", -1234, 0x7abcdef0, -122147483647, 0x123456789a);

    return;
}