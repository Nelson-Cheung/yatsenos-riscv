#include "process.h"
#include "constant.h"
#include "object.h"
#include "driver.h"
#include "utils.h"
#include "rv64.h"
#include <elf.h>

const unsigned long PCB_SIZE = PAGE_SIZE;    // PCB的大小，4KB。
char PCB_SET[PCB_SIZE * MAX_PROGRAM_AMOUNT]; // 存放PCB的数组，预留了MAX_PROGRAM_AMOUNT个PCB的大小空间。
bool PCB_SET_STATUS[MAX_PROGRAM_AMOUNT];     // PCB的分配状态，true表示已经分配，false表示未分配。
extern unsigned long TEST;

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

    for (unsigned long i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
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
            memset(ans, 0, PCB_SIZE);
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
    unsigned long bytes_per_l2_item = 1UL << 30;

    pair<unsigned long, unsigned long> mmio, kernel;

    mmio.first = MMIO_BASE;
    mmio.second = MMIO_END;

    kernel.first = KERNEL_SPACE_BASE;
    kernel.second = KERNEL_SAPCE_END;

    pair<unsigned long, unsigned long> address_set[] = {mmio, kernel};

    for (unsigned long i = 0; i < sizeof(address_set) / sizeof(pair<unsigned long, unsigned long>); ++i)
    {
        for (unsigned long addr = address_set[i].first; addr < address_set[i].second; addr += bytes_per_l2_item)
        {
            l2 = memory_manager.l2_pte_index(addr);
            l2_pte = memory_manager.l2_pte_pointer(addr);
            process_l2_page_table[l2] = *l2_pte;
        }
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

    pcb->status = ProcessStatus::READY;
    pcb->total_ticks = 0;
    pcb->ticks_after_schedule = 0;
    pcb->l2_page_table = (unsigned long)create_process_l2_page_table();

    unsigned long *page_table = (unsigned long *)pcb->l2_page_table;
    unsigned long addr = (unsigned long)&TEST;
    unsigned long index = memory_manager.l2_pte_index(addr);

    // printf("%lx %lx\n", page_table, page_table[index]);

    pcb->stack = (unsigned long *)((unsigned long)pcb + PCB_SIZE - sizeof(Registers));

    Registers *registers = (Registers *)pcb->stack;
    memset(registers, 0, sizeof(Registers));

    // 通过page fault来处理
    registers->ra = 0;

    // 分配用户栈
    registers->sp = USER_SAPCE_END;
    unsigned long new_page = memory_manager.allocatePhysicalPages(1);
    if (new_page == -1UL)
    {
        return -1UL;
    }

    // printf("user stack: 0x%lx\n", USER_SAPCE_END - PAGE_SIZE);

    // printf("user stack: %lx\n", USER_SAPCE_END - PAGE_SIZE);
    memory_manager.connect_virtual_physical_address(pcb->l2_page_table, new_page, USER_SAPCE_END - PAGE_SIZE, PTE_V | PTE_R | PTE_W | PTE_U);
    // unsigned long pte = ((unsigned long *)(pcb->l2_page_table))[511];
    // memory_manager.connect_virtual_physical_address(memory_manager.l2_page_table, new_page, USER_SAPCE_END - PAGE_SIZE, PTE_V | PTE_R | PTE_W | PTE_U);

    // 加载程序到内存
    unsigned long entry = load_elf(filename, pcb->l2_page_table);

    if (entry == -1UL)
    {
        return -1UL;
    }

    pcb->stack -= 4;
    pcb->stack[0] = (unsigned long)start_process; // ra
    pcb->stack[1] = (1UL << 18) | (1UL << 5);     // sstatus
    pcb->stack[2] = entry;                        // sepc
    pcb->stack[3] = 0;                            // scause

    all_process.push_back(&(pcb->universal_tag));
    ready_process.push_back(&(pcb->schedule_tag));

    return pcb->pid;
}

unsigned long ProcessManager::load_elf(const char *filename, unsigned long l2_page_table)
{
    if (!filename)
    {
        return 0;
    }

    // 没有实现文件系统
    Elf64_Ehdr *ehdr_ptr = (Elf64_Ehdr *)filename;
    Elf64_Phdr *phdr_ptr = nullptr;
    unsigned long new_page, flags;
    const char *current;

    for (int i = 0; i < ehdr_ptr->e_phnum; ++i)
    {
        phdr_ptr = (Elf64_Phdr *)(filename + ehdr_ptr->e_phoff + i * ehdr_ptr->e_phentsize);

        if (phdr_ptr->p_type != PT_LOAD)
        {
            continue;
        }

        // printf("%d: %lx\n", i, phdr_ptr->p_vaddr);

        current = filename + phdr_ptr->p_offset;
        for (unsigned long offset = 0; offset < phdr_ptr->p_filesz; offset += PAGE_SIZE)
        {
            new_page = memory_manager.allocatePhysicalPages(1);
            if (new_page == -1UL)
            {
                return -1UL;
            }
            memset((void *)new_page, 0, PAGE_SIZE);

            if ((phdr_ptr->p_filesz - offset) >= PAGE_SIZE)
            {
                memcpy((void *)(current + offset), (void *)new_page, PAGE_SIZE);
            }
            else
            {
                memcpy((void *)(current + offset), (void *)new_page, phdr_ptr->p_filesz - offset);
            }

            flags = PTE_V | PTE_U;

            if (phdr_ptr->p_flags & PF_X)
            {
                flags |= PTE_X;
            }

            if (phdr_ptr->p_flags & PF_R)
            {
                flags |= PTE_R;
            }

            if (phdr_ptr->p_flags & PF_W)
            {
                flags |= PTE_W;
            }

            // printf("flags: %lx\n", flags);
            printf("%lx %lx\n", new_page, phdr_ptr->p_vaddr + offset);
            memory_manager.connect_virtual_physical_address(l2_page_table, new_page, phdr_ptr->p_vaddr + offset, flags);
        }
    }

    return ehdr_ptr->e_entry;
}

void ProcessManager::set_l2_page_table(unsigned long address)
{

    unsigned long reg = read_satp() & 0xfffff00000000000UL;
    reg |= (address >> 12);
    write_satp(reg);
}

void ProcessManager::schedule()
{
    if (ready_process.size() < 1)
    {
        return;
    }

    // time sharing
    ++current_running_process->total_ticks;
    ++current_running_process->ticks_after_schedule;

    // if (current_running_process->total_ticks <= 5)
    // {
    //     return;
    // }

    if (current_running_process->status == ProcessStatus::RUNNING)
    {
        ready_process.push_back(&(current_running_process->schedule_tag));
        current_running_process->ticks_after_schedule = 0;
        current_running_process->status = ProcessStatus::READY;
    }
    else if (current_running_process->status == ProcessStatus::DEAD)
    {
        // 回收
    }

    ListItem *item = ready_process.front();
    PCB *next = ListItem2PCB(item, schedule_tag);
    next->status = ProcessStatus::RUNNING;

    PCB *current = current_running_process;
    current_running_process = next;

    ready_process.pop_front();

    set_l2_page_table(next->l2_page_table);

    printf("schedule %lx to %lx\n", current, next);

    switch_to(current, next);
}

PCB *ProcessManager::find_process_by_pid(unsigned long pid)
{
    ListItem *item = all_process.front();
    PCB *pcb = nullptr;

    while (item)
    {
        pcb = ListItem2PCB(item, universal_tag);
        if (pcb->pid == pid)
        {
            return pcb;
        }
        item = item->next;
    }

    return nullptr;
}

bool ProcessManager::copy_process(PCB *parent, PCB *child)
{
    // 这里和 create_process 联系紧密
    // 复制PCB
    Registers *child_registers = (Registers *)((unsigned long)child + PAGE_SIZE - sizeof(Registers));
    Registers *parent_registers = (Registers *)((unsigned long)parent + PAGE_SIZE - sizeof(Registers));
    memcpy(parent_registers, child_registers, sizeof(Registers));

    child_registers->a0 = 0;
    child_registers->sp = read_sscratch();

    // child 会通过start_process来启动
    child->stack = (unsigned long *)child_registers;
    child->stack -= 4;
    child->stack[0] = (unsigned long)start_process; // ra
    child->stack[1] = read_sstatus();               // sstatus
    child->stack[2] = read_sepc();                  // sepc
    child->stack[3] = 0;                            // scause

    child->status = ProcessStatus::READY;
    child->parent_pid = parent->pid;
    child->priority = parent->priority;
    child->ticks_after_schedule = parent->ticks_after_schedule;
    child->total_ticks = parent->total_ticks;

    // unsigned long *parent_l2_page_table = (unsigned long *)parent->l2_page_table;
    // unsigned long bytes_per_l2_pte = 1UL << 30;
    unsigned long *src, *dst;

    unsigned long *pte = nullptr;

    for (unsigned long address = USER_SAPCE_BASE; address < USER_SAPCE_END; address += PAGE_SIZE)
    {
        pte = memory_manager.
        if (!(parent_l2_page_table[i] & PTE_V))
        {
            continue;
        }

        src = (unsigned long *)memory_manager.vaddr2paddr(address);
        dst = (unsigned long *)memory_manager.allocatePhysicalPages(1);
        if (!dst)
        {
            return false;
        }
        memcpy(src, dst, PAGE_SIZE);

        pte = memory_manager.l0_pte_pointer(address);
        memory_manager.connect_virtual_physical_address(child->l2_page_table, (unsigned long)dst, address, (*pte) & 0x3ff);
    }

    return true;
}