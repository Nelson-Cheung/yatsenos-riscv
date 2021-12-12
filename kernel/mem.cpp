#include "mem.h"
#include "constant.h"
#include "utils.h"
#include "driver.h"
#include "object.h"
#include "uart.h"
#include "timer.h"
#include "clint.h"
#include "rv64.h"
#include "queue.h"

unsigned long TEST = 0xdeadbeef;

#define CAL_N (MEMORY_SIZE >> 12)
#define CAL_A (1UL << 27)
#define CAL_B (1UL << 18)
#define CAL_C (1UL << 9)
#define CAL_X (((CAL_B + CAL_C + 1) * CAL_N + 3 * CAL_A + 2 * CAL_B + CAL_C) / (CAL_A - CAL_B - CAL_C - 1) + 1)

MemoryManager::MemoryManager()
{
    initialize();
}

unsigned long MemoryManager::init_physical_space()
{
    this->totalMemory = 0;
    this->totalMemory = MEMORY_SIZE;

    // 预留的内存
    unsigned long usedMemory = KERNEL_SIZE;

    if (this->totalMemory < usedMemory)
    {
        driver.uart.putstr("no enough memory\n");
        while (true)
            ;
    }

    // 剩余的空闲的内存
    unsigned long freeMemory = this->totalMemory - usedMemory;
    unsigned long freePages = freeMemory / PAGE_SIZE;
    unsigned long bitmapBytes = ceil(freePages, 8);

    freeMemory = this->totalMemory - ceil(usedMemory + bitmapBytes, PAGE_SIZE) * PAGE_SIZE;
    freePages = freeMemory / PAGE_SIZE;

    unsigned long physical_start_address = DRAM_BASE + ceil(usedMemory + bitmapBytes, PAGE_SIZE) * PAGE_SIZE;
    unsigned long physical_bitmap_start_address = DRAM_BASE + usedMemory;

    // int kernelVirtualBitMapStart = userPhysicalBitMapStart + ceil(userPages, 8);

    physical_address_pool.initialize(
        (char *)physical_bitmap_start_address,
        freePages,
        physical_start_address);

    // kernelVirtual.initialize(
    //     (char *)kernelVirtualBitMapStart,
    //     kernelPages,
    //     KERNEL_VIRTUAL_START);

    printf("total memory: %ld bytes ( %ld MB )\n",
           this->totalMemory,
           this->totalMemory / 1024 / 1024);

    printf("physical address pool\n"
           "    start address: 0x%lx\n"
           "    total pages: %ld ( %ld MB )\n"
           "    bitmap start address: 0x%lx\n",
           physical_start_address,
           freePages, freePages * PAGE_SIZE / 1024 / 1024,
           physical_bitmap_start_address);

    printf("Used Memory: %d ( %d MB )\n", usedMemory + bitmapBytes, (usedMemory + bitmapBytes) / 1024 / 1024);

    return ceil(usedMemory + bitmapBytes, PAGE_SIZE) * PAGE_SIZE;

    // printf("kernel virtual pool\n"
    //        "    start address: 0x%x\n"
    //        "    total pages: %d  ( %d MB ) \n"
    //        "    bit map start address: 0x%x\n",
    //        KERNEL_VIRTUAL_START,
    //        userPages, kernelPages * PAGE_SIZE / 1024 / 1024,
    //        kernelVirtualBitMapStart);
}

void MemoryManager::openPageMechanism(const pair<unsigned long, unsigned long> *address, unsigned long size)
{
    unsigned long current;

    // 为正在使用的地址建立one-one映射
    for (unsigned long i = 0; i < size; ++i)
    {
        current = address[i].first & (~0xfffUL);

        while (current < address[i].second)
        {
            connect_virtual_physical_address(l2_page_table,current, current, PTE_V | PTE_R | PTE_W | PTE_X | PTE_U);
            current += PAGE_SIZE;
        }
    }

    // ((unsigned long *)l2_page_table)[511] = (((unsigned long)l2_page_table) >> 12) << 10;
    // ((unsigned long *)l2_page_table)[511] |= PTE_V;

    // ((unsigned long *)l2_page_table)[510] = (((unsigned long)l2_page_table) >> 12) << 10;
    // ((unsigned long *)l2_page_table)[510] |= PTE_V;
    // ((unsigned long *)l2_page_table)[510] |= PTE_R;
    // ((unsigned long *)l2_page_table)[510] |= PTE_W;
    // ((unsigned long *)l2_page_table)[510] |= PTE_X;

    // printf("L2 page table address: %x\n", l2_page_table);
    // unsigned long addr = (unsigned long)&TEST;
    // l2 = (addr & PPN2_MASK) >> 30;
    // l1 = (addr & PPN1_MASK) >> 21;
    // l0 = (addr & PPN0_MASK) >> 12;

    // unsigned long mask = 0xfffffffffffUL << 10;

    // pte = ((unsigned long *)l2_page_table);
    // pte = (unsigned long *)((pte[l2] & mask) << 2);
    // pte = (unsigned long *)((pte[l1] & mask) << 2);
    // pte = (unsigned long *)((pte[l0] & mask) << 2);

    // unsigned long trans_addr = (unsigned long)pte;

    // printf("need pages: %d\n", counter);

    unsigned long satp = 0;
    satp = satp | (8UL << 60);
    satp = satp | ((unsigned long)l2_page_table >> 12);

    write_satp(satp);

    unsigned long addr = (unsigned long)&TEST;
    printf("%lx %lx %lx\n", *l2_pte_pointer(addr), *l1_pte_pointer(addr), *l0_pte_pointer(addr));

    // unsigned long paddr = addr & 0xfffUL;
}

void MemoryManager::connect_virtual_physical_address(unsigned long l2_page_table, unsigned long paddr, unsigned long vaddr, unsigned long flags)
{
    unsigned long l2, l1, l0, new_page;
    unsigned long *pte = nullptr;

    l2 = (vaddr & PPN2_MASK) >> 30;
    l1 = (vaddr & PPN1_MASK) >> 21;
    l0 = (vaddr & PPN0_MASK) >> 12;

    if (vaddr == 0x3fffff000UL)
    {
        printf("%ld %ld %ld\n", l2, l1, l0);
    }

    pte = ((unsigned long *)l2_page_table);
    if ((pte[l2] & PTE_V) == 0)
    {
        new_page = allocatePhysicalPages(1);
        if (new_page == -1UL)
        {
            printf("no enough space\n");
            return;
        }
        memset((void *)new_page, 0, PAGE_SIZE);
        pte[l2] = (new_page >> 12) << 10;
        pte[l2] = pte[l2] | PTE_V;
    }

    pte = (unsigned long *)((pte[l2] >> 10) << 12);

    if ((pte[l1] & PTE_V) == 0)
    {
        new_page = allocatePhysicalPages(1);
        if (new_page == -1UL)
        {
            printf("no enough space\n");
            return;
        }
        memset((void *)new_page, 0, PAGE_SIZE);
        pte[l1] = (new_page >> 12) << 10;
        pte[l1] = pte[l1] | PTE_V;
    }

    pte = (unsigned long *)((pte[l1] >> 10) << 12);
    pte[l0] = (paddr >> 12) << 10;
    pte[l0] = pte[l0] | flags;
}

void MemoryManager::initialize()
{
    init_physical_space();
    l2_page_table = allocatePhysicalPages(1);
    memset((void *)l2_page_table, 0, PAGE_SIZE);

    pair<unsigned long, unsigned long> clint, uart, kernel;

    clint.first = CLINT_BASE;
    clint.second = clint.first + CLINT_SIZE;

    uart.first = UART_BASE;
    uart.second = uart.first + UART_SIZE;

    kernel.first = DRAM_BASE;
    kernel.second = kernel.first + MEMORY_SIZE;

    pair<unsigned long, unsigned long> address[] = {clint, uart, kernel};
    openPageMechanism(address, sizeof(address) / sizeof(pair<unsigned long, unsigned long>));
}

unsigned long MemoryManager::allocatePhysicalPages(unsigned long amount)
{
    unsigned long start = -1;

    start = physical_address_pool.allocate(amount);

    return (start == -1UL) ? 0 : start;
}

void MemoryManager::releasePhysicalPages(unsigned long paddr, unsigned long amount)
{
    physical_address_pool.release(paddr, amount);
}

unsigned long MemoryManager::getTotalMemory()
{
    return this->totalMemory;
}

unsigned long MemoryManager::l2_pte_index(unsigned long virtual_address)
{
    return (virtual_address & PPN2_MASK) >> 30;
}

unsigned long MemoryManager::l1_pte_index(unsigned long virtual_address)
{
    return (virtual_address & PPN1_MASK) >> 21;
}

unsigned long MemoryManager::l0_pte_index(unsigned long virtual_address)
{
    return (virtual_address & PPN0_MASK) >> 12;
}

unsigned long *MemoryManager::l2_pte_pointer(unsigned long virtual_address)
{
    unsigned long l2 = (virtual_address & PPN2_MASK) >> 30;
    return (unsigned long *)(l2_page_table + sizeof(unsigned long) * l2);
}

unsigned long *MemoryManager::l1_pte_pointer(unsigned long virtual_address)
{
    unsigned long *pte = l2_pte_pointer(virtual_address);
    unsigned long l1_page_table = ((*pte) & PTE_PPN_MASK) << 2;
    unsigned long l1 = (virtual_address & PPN1_MASK) >> 21;
    return (unsigned long *)(l1_page_table + sizeof(unsigned long) * l1);
}

unsigned long *MemoryManager::l0_pte_pointer(unsigned long virtual_address)
{
    unsigned long *pte = l1_pte_pointer(virtual_address);
    unsigned long l0_page_table = ((*pte) & PTE_PPN_MASK) << 2;
    unsigned long l0 = (virtual_address & PPN0_MASK) >> 12;
    return (unsigned long *)(l0_page_table + sizeof(unsigned long) * l0);
}