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

    // pair<unsigned long, unsigned long> mmio, kernel;

    // mmio.first = MMIO_BASE;
    // mmio.second = MMIO_END;

    // kernel.first = KERNEL_SPACE_BASE;
    // kernel.second =  kernel.first + MEMORY_SIZE;

    // pair<unsigned long, unsigned long> address_set[] = {mmio, kernel};

    // for (unsigned long i = 0; i < sizeof(address_set) / sizeof(pair<unsigned long, unsigned long>); ++i)
    // {
    //     for (unsigned long addr = address_set[i].first; addr < address_set[i].second; addr += bytes_per_l2_item)
    //     {
    //         l2 = memory_manager.l2_pte_index(addr);
    //         l2_pte = memory_manager.l2_pte_pointer(addr);
    //         process_l2_page_table[l2] = *l2_pte;
    //     }
    // }

    l2_pte = (unsigned long *)memory_manager.l2_page_table;
    for (int i = 0; i < 512; ++i)
    {
        process_l2_page_table[i] = l2_pte[i];
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

    unsigned long *page_table = (unsigned long *)pcb->l2_page_table;
    unsigned long addr = (unsigned long)&TEST;
    unsigned long index = memory_manager.l2_pte_index(addr);

    printf("%lx %lx\n", page_table, page_table[index]);

    pcb->stack = (unsigned long *)((unsigned long)pcb + PCB_SIZE - sizeof(Registers));

    Registers *registers = (Registers *)pcb->stack;
    // 通过page fault来处理
    registers->ra = 0;

    // 分配用户栈
    registers->sp = USER_SAPCE_END;
    unsigned long new_page = memory_manager.allocatePhysicalPages(1);
    if (new_page == -1UL)
    {
        return -1UL;
    }

    printf("user stack: 0x%lx\n", USER_SAPCE_END - PAGE_SIZE);

    memory_manager.connect_virtual_physical_address(pcb->l2_page_table, new_page, USER_SAPCE_END - PAGE_SIZE, PTE_V | PTE_R | PTE_W | PTE_U);
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

        printf("%d: %lx\n", i, phdr_ptr->p_vaddr);

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

            printf("flags: %lx\n", flags);

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