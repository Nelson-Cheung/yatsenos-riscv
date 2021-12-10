#ifndef PROCESS_H
#define PROCESS_H

#include "pcb.h"
#include "list.h"

void process_exit();

class ProcessManager
{
private:
    PCB *current_running_process;
    List all_process;
    List ready_process;

public:
    void initialize();
    unsigned long create_process(const char *filename);

private:
    PCB *allocate_pcb();
    void release_pcb(PCB *pcb);
    unsigned long *create_process_l2_page_table();
    bool load_elf(const char *filename);
};

#endif