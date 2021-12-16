#ifndef PROCESS_H
#define PROCESS_H

#include "pcb.h"
#include "list.h"

#define ListItem2PCB(ADDRESS, LIST_ITEM) ((PCB *)((unsigned long)(ADDRESS) - (unsigned long)&((PCB *)0)->LIST_ITEM))

class ProcessManager
{
public:
    PCB *current_running_process;
    List all_process;
    List ready_process;

public:
    void initialize();
    unsigned long create_process(const char *filename);
    void set_l2_page_table(unsigned long address);
    void schedule();
    PCB *find_process_by_pid(unsigned long pid);
    bool copy_process(PCB *parent, PCB *child);
private:
    // 分配PCB
    PCB *allocate_pcb();
    // 释放PCB
    void release_pcb(PCB *pcb);
    // 创建用户L2页表
    unsigned long *create_process_l2_page_table();
    // 加载用户进程到内存
    unsigned long load_elf(const char *filename, unsigned long l2_page_table);
};

struct Registers
{
    unsigned long zero;
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
};

extern "C" PCB *switch_to(void *current, void *next);
extern "C" void start_process(PCB *pcb);

#endif