#ifndef PROCESS_H
#define PROCESS_H

#include "pcb.h"
#include "list.h"

void process_exit();

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

#endif