#include "driver.h"
#include "rv64.h"
#include "interrupt.h"
#include "utils.h"
#include "mem.h"
#include "constant.h"
#include "process.h"
#include "syscall_manager.h"
#include "syscall.h"

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
    printf("set up driver for clint, timer, uart.\n");

    driver.clint.set_interrupt_handler((unsigned long)supervisor_interrupt_entry);
    printf("set up supervisor interrupt handler.\n");

    memory_manager.initialize();
    printf("set up physical memory management and Sv39 paging.\n");

    process_manager.initialize();
    printf("set up process management.\n");
}

#include "user_process.h"

extern "C" void kernel_entry()
{
    rv64_kernel_init();
    printf("kernel initialization done.\n"
           "Welcome to RISC-V 64!\n");

    printf(

        "######  ###  #####   #####        #     #\n"
        "#     #  #  #     # #     #       #     #\n"
        "#     #  #  #       #             #     #\n"
        "######   #   #####  #       ##### #     #\n"
        "#   #    #        # #              #   #\n"
        "#    #   #  #     # #     #         # #\n"
        "#     # ###  #####   #####           #\n"

    );

    unsigned long pid = process_manager.create_process((const char *)zero);
    if (pid)
    {
        return;
    }

    printf("create 0 process.\n");

    driver.clint.enable_interrupt();
    driver.clint.enable_timer_interrupt();
    unsigned long mtime;
    mtime = driver.timer.read_mtime();
    driver.timer.write_mtimecmp(mtime + 0x2ffffffUL);
    printf("enable timer interrupt.\n");

    PCB *pcb = ListItem2PCB(process_manager.ready_process.front(), schedule_tag);
    PCB none;

    process_manager.ready_process.pop_front();
    pcb->status = ProcessStatus::RUNNING;
    process_manager.set_l2_page_table(pcb->l2_page_table);

    process_manager.current_running_process = pcb;

    printf("start 0 process.\n");
    switch_to(&none, pcb);
}