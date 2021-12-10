#include "process.h"
#include "constant.h"
#include "object.h"
#include "driver.h"
#include "utils.h"
#include "rv64.h"

const int PCB_SIZE = PAGE_SIZE;              // PCB的大小，4KB。
char PCB_SET[PCB_SIZE * MAX_PROGRAM_AMOUNT]; // 存放PCB的数组，预留了MAX_PROGRAM_AMOUNT个PCB的大小空间。
bool PCB_SET_STATUS[MAX_PROGRAM_AMOUNT];     // PCB的分配状态，true表示已经分配，false表示未分配。

void process_exit()
{
    while (true)
        ;
}

void ProcessManager::initialize()
{
    all_process.initialize();
    ready_process.initialize();
    current_running_process = nullptr;

    for (int i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
    {
        PCB_SET_STATUS[i] = false;
    }
}

PCB *ProcessManager::allocate_pcb()
{
    for (unsigned long i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
    {
        if (!PCB_SET_STATUS[i])
        {
            PCB_SET_STATUS[i] = true;
            PCB *ans = (PCB *)((unsigned long)PCB_SET + PCB_SIZE * i);
            ans->pid = i;
            return ans;
        }
    }

    return nullptr;
}

void ProcessManager::release_pcb(PCB *pcb)
{
    unsigned long index = ((unsigned long)pcb - (unsigned long)PCB_SET) / PCB_SIZE;
    PCB_SET_STATUS[index] = false;
}

unsigned long *ProcessManager::create_process_l2_page_table()
{
    unsigned long new_page = memory_manager.allocatePhysicalPages(1);
    if (new_page == -1UL)
    {
        return nullptr;
    }

    unsigned long *process_l2_page_table = (unsigned long *)new_page;
    memset(process_l2_page_table, 0, PAGE_SIZE);

    unsigned long l2 = 0;
    unsigned long *l2_pte = nullptr;
    unsigned long pages_per_l2_item = 1UL << 30;

    // 复制MMIO内存
    for (unsigned long addr = MMIO_BASE; addr < MMIO_END; addr += pages_per_l2_item)
    {
        l2 = memory_manager.l2_pte_index(addr);
        l2_pte = memory_manager.l2_pte_pointer(addr);
        process_l2_page_table[l2] = *l2_pte;
    }

    // 复制KERNEL内存
    for (unsigned long addr = KERNEL_SPACE_BASE; addr < KERNEL_SAPCE_END; addr += pages_per_l2_item)
    {
        l2 = memory_manager.l2_pte_index(addr);
        l2_pte = memory_manager.l2_pte_pointer(addr);
        process_l2_page_table[l2] = *l2_pte;
    }

    return process_l2_page_table;
}

unsigned long ProcessManager::create_process(const char *filename)
{
    PCB *pcb = allocate_pcb();
    if (pcb == nullptr)
    {
        return -1UL;
    }

    memset(pcb, 0, PCB_SIZE);

    pcb->status = ProcessStatus::READY;
    pcb->total_ticks = 0;
    pcb->ticks_after_schedule = 0;
    pcb->l2_page_table = (unsigned long)create_process_l2_page_table();

    pcb->stack = (unsigned long *)((unsigned long)pcb + PCB_SIZE - sizeof(Registers));

    Registers *registers = (Registers *)pcb->stack;
    // 通过page fault来处理
    registers->ra = (unsigned long)process_exit;

    registers->sp = USER_SAPCE_END;
    unsigned long new_page = memory_manager.allocatePhysicalPages(1);
    if (new_page == -1UL)
    {
        return -1UL;
    }
    memory_manager.connect_virtual_physical_address(USER_SAPCE_END - PAGE_SIZE, new_page, PTE_V | PTE_R | PTE_W | PTE_U);


}