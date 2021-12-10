#ifndef PCB_H
#define PCB_H

#include "constant.h"
#include "list.h"
#include "address_pool.h"

enum ProcessStatus
{
    CREATED,
    RUNNING,
    READY,
    BLOCKED,
    DEAD
};

struct PCB
{
    unsigned long *stack;               // 栈指针，用于调度时保存esp
    char name[MAX_PROGRAM_NAME + 1];    // 进程名
    enum ProcessStatus status;          // 进程的状态
    unsigned long priority;             // 进程优先级
    unsigned long pid;                  // 进程pid
    unsigned long total_ticks;          // 进程时间片总时间
    unsigned long ticks_after_schedule; // 进程已执行时间
    ListItem schedule_tag;              // 用于调度的进程标识
    ListItem universal_tag;             // 进程的全局标识
    unsigned long l2_page_table;        // L2页表地址
    unsigned long parent_pid;           // 父进程pid
    unsigned long return_value;         // 返回值
    // AddressPool userVirtual;  // 用户程序虚拟地址池
};

#endif