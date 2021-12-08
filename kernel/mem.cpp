#include "mem.h"
#include "constant.h"
#include "utils.h"
#include "driver.h"
#include "object.h"
#include "uart.h"
#include "timer.h"
#include "clint.h"
#include "rv64.h"

static const unsigned long TEST = 0xdeadbeef;

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

    unsigned long kernelPages = freePages / 2;
    unsigned long userPages = freePages - kernelPages;

    unsigned long kernelPhysicalStartAddress = DRAM_BASE + ceil(usedMemory + bitmapBytes, PAGE_SIZE) * PAGE_SIZE;
    unsigned long userPhysicalStartAddress = kernelPhysicalStartAddress + kernelPages * PAGE_SIZE;

    unsigned long kernelPhysicalBitMapStart = DRAM_BASE + usedMemory;
    unsigned long userPhysicalBitMapStart = kernelPhysicalBitMapStart + ceil(kernelPages, 8);
    // int kernelVirtualBitMapStart = userPhysicalBitMapStart + ceil(userPages, 8);

    kernelPhysical.initialize(
        (char *)kernelPhysicalBitMapStart,
        kernelPages,
        kernelPhysicalStartAddress);

    userPhysical.initialize(
        (char *)userPhysicalBitMapStart,
        userPages,
        userPhysicalStartAddress);

    // kernelVirtual.initialize(
    //     (char *)kernelVirtualBitMapStart,
    //     kernelPages,
    //     KERNEL_VIRTUAL_START);

    printf("total memory: %d bytes ( %d MB )\n",
           this->totalMemory,
           this->totalMemory / 1024 / 1024);

    printf("kernel pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bitmap start address: 0x%x\n",
           kernelPhysicalStartAddress,
           kernelPages, kernelPages * PAGE_SIZE / 1024 / 1024,
           kernelPhysicalBitMapStart);

    printf("user pool\n"
           "    start address: 0x%x\n"
           "    total pages: %d ( %d MB )\n"
           "    bit map start address: 0x%x\n",
           userPhysicalStartAddress,
           userPages, userPages * PAGE_SIZE / 1024 / 1024,
           userPhysicalBitMapStart);

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
    unsigned long current, new_page;
    unsigned long l2, l1, l0;
    unsigned long *pte;

    // 为正在使用的地址建立one-one映射
    for (unsigned long i = 0; i < size; ++i)
    {
        current = address[i].first & (~0xfffUL);

        while (current < address[i].second)
        {
            l2 = (current & PPN2_MASK) >> 30;
            l1 = (current & PPN1_MASK) >> 21;
            l0 = (current & PPN0_MASK) >> 12;

            pte = l2_page_table;
            if ((pte[l2] & PTE_V) == 0)
            {
                new_page = allocatePhysicalPages(AddressPoolType::KERNEL, 1);
                if (new_page == -1)
                {
                    printf("no enough space\n");
                    return;
                }
                memset((void *)new_page, 0, PAGE_SIZE);
                pte[l2] = (new_page >> 12) << 10;
                pte[l2] = pte[l2] | PTE_V;
            }

            // if (current == 0x80004000UL)
            // {
            //     printf("YES\n");
            // }

            pte = (unsigned long *)((pte[l2] >> 10) << 12);

            if ((pte[l1] & PTE_V) == 0)
            {
                new_page = allocatePhysicalPages(AddressPoolType::KERNEL, 1);
                if (new_page == -1)
                {
                    printf("no enough space\n");
                    return;
                }
                memset((void *)new_page, 0, PAGE_SIZE);
                pte[l1] = (new_page >> 12) << 10;
                pte[l1] = pte[l1] | PTE_V;
            }

            // if (current == 0x80004000UL)
            // {
            //     printf("YES\n");
            // }

            pte = (unsigned long *)((pte[l1] >> 10) << 12);
            pte[l0] = (current >> 12) << 10;
            pte[l0] = pte[l0] | PTE_V;
            pte[l0] = pte[l0] | PTE_R;
            pte[l0] = pte[l0] | PTE_W;
            pte[l0] = pte[l0] | PTE_X;

            // if (current == 0x80004000UL)
            // {
            //     printf("YES\n%d %d %d\n", l2, l1, l0);
            // }

            current += PAGE_SIZE;

            // ++l0;
            // if (l0 >= 512)
            // {
            //     l0 -= 512;
            //     ++l1;
            // }

            // if (l1 >= 512)
            // {
            //     l1 -= 512;
            //     ++l2;
            // }
        }
    }

    // printf("L2 page table address: %x\n", l2_page_table);
    // unsigned long addr = (unsigned long)&TEST;
    // l2 = (addr & PPN2_MASK) >> 30;
    // l1 = (addr & PPN1_MASK) >> 21;
    // l0 = (addr & PPN0_MASK) >> 12;

    // unsigned long mask = 0xfffffffffffUL << 10;
    
    // pte = l2_page_table;
    // pte = (unsigned long *)((pte[l2] & mask) << 2);
    // pte = (unsigned long *)((pte[l1] & mask) << 2);
    // pte = (unsigned long *)((pte[l0] & mask) << 2);

    // unsigned long trans_addr = (unsigned long)pte;

    unsigned long satp = 0;
    satp = satp | (8UL << 60);
    satp = satp | ((unsigned long)l2_page_table >> 12);

    write_satp(satp);
}

void MemoryManager::initialize()
{
    unsigned long used = init_physical_space();
    l2_page_table = (unsigned long *)allocatePhysicalPages(AddressPoolType::KERNEL, 1);
    memset(l2_page_table, 0, PAGE_SIZE);

    pair<unsigned long, unsigned long> clint, uart, kernel;

    clint.first = CLINT_BASE;
    clint.second = clint.first + CLINT_SIZE;

    uart.first = UART_BASE;
    uart.second = uart.first + UART_SIZE;

    kernel.first = DRAM_BASE;
    kernel.second = kernel.first + used;

    pair<unsigned long, unsigned long> address[] = {clint, uart, kernel};
    openPageMechanism(address, 3);
}

unsigned long MemoryManager::allocatePhysicalPages(enum AddressPoolType type, unsigned long count)
{
    unsigned long start = -1;

    if (type == AddressPoolType::KERNEL)
    {
        start = kernelPhysical.allocate(count);
    }
    else if (type == AddressPoolType::USER)
    {
        start = userPhysical.allocate(count);
    }

    return (start == -1) ? 0 : start;
}

void MemoryManager::releasePhysicalPages(enum AddressPoolType type, unsigned long paddr, unsigned long count)
{
    if (type == AddressPoolType::KERNEL)
    {
        kernelPhysical.release(paddr, count);
    }
    else if (type == AddressPoolType::USER)
    {

        userPhysical.release(paddr, count);
    }
}

unsigned long MemoryManager::getTotalMemory()
{
    return this->totalMemory;
}

// void MemoryManager::openPageMechanism()
// {

// }

// int MemoryManager::allocatePages(enum AddressPoolType type, const int count)
// {
//     // 第一步：从虚拟地址池中分配若干虚拟页
//     int virtualAddress = allocateVirtualPages(type, count);
//     if (!virtualAddress)
//     {
//         return 0;
//     }

//     bool flag;
//     int physicalPageAddress;
//     int vaddress = virtualAddress;

//     // 依次为每一个虚拟页指定物理页
//     for (int i = 0; i < count; ++i, vaddress += PAGE_SIZE)
//     {
//         flag = false;
//         // 第二步：从物理地址池中分配一个物理页
//         physicalPageAddress = allocatePhysicalPages(type, 1);
//         if (physicalPageAddress)
//         {
//             //printf("allocate physical page 0x%x\n", physicalPageAddress);

//             // 第三步：为虚拟页建立页目录项和页表项，使虚拟页内的地址经过分页机制变换到物理页内。
//             flag = connectPhysicalVirtualPage(vaddress, physicalPageAddress);
//         }
//         else
//         {
//             flag = false;
//         }

//         // 分配失败，释放前面已经分配的虚拟页和物理页表
//         if (!flag)
//         {
//             // 前i个页表已经指定了物理页
//             releasePages(type, virtualAddress, i);
//             // 剩余的页表未指定物理页
//             releaseVirtualPages(type, virtualAddress + i * PAGE_SIZE, count - i);
//             return 0;
//         }
//     }

//     return virtualAddress;
// }

// int MemoryManager::allocateVirtualPages(enum AddressPoolType type, const int count)
// {
//     int start = -1;

//     if (type == AddressPoolType::KERNEL)
//     {
//         start = kernelVirtual.allocate(count);
//     }

//     return (start == -1) ? 0 : start;
// }

// bool MemoryManager::connectPhysicalVirtualPage(const int virtualAddress, const int physicalPageAddress)
// {
//     // 计算虚拟地址对应的页目录项和页表项
//     int *pde = (int *)toPDE(virtualAddress);
//     int *pte = (int *)toPTE(virtualAddress);

//     // 页目录项无对应的页表，先分配一个页表
//     if(!(*pde & 0x00000001))
//     {
//         // 从内核物理地址空间中分配一个页表
//         int page = allocatePhysicalPages(AddressPoolType::KERNEL, 1);
//         if (!page)
//             return false;

//         // 使页目录项指向页表
//         *pde = page | 0x7;
//         // 初始化页表
//         char *pagePtr = (char *)(((int)pte) & 0xfffff000);
//         memset(pagePtr, 0, PAGE_SIZE);
//     }

//     // 使页表项指向物理页
//     *pte = physicalPageAddress | 0x7;

//     return true;
// }

// int MemoryManager::toPDE(const int virtualAddress)
// {
//     return (0xfffff000 + (((virtualAddress & 0xffc00000) >> 22) * 4));
// }

// int MemoryManager::toPTE(const int virtualAddress)
// {
//     return (0xffc00000 + ((virtualAddress & 0xffc00000) >> 10) + (((virtualAddress & 0x003ff000) >> 12) * 4));
// }

// void MemoryManager::releasePages(enum AddressPoolType type, const int virtualAddress, const int count)
// {
//     int vaddr = virtualAddress;
//     int *pte;
//     for (int i = 0; i < count; ++i, vaddr += PAGE_SIZE)
//     {
//         // 第一步，对每一个虚拟页，释放为其分配的物理页
//         releasePhysicalPages(type, vaddr2paddr(vaddr), 1);

//         // 设置页表项为不存在，防止释放后被再次使用
//         pte = (int *)toPTE(vaddr);
//         *pte = 0;
//     }

//     // 第二步，释放虚拟页
//     releaseVirtualPages(type, virtualAddress, count);
// }

// int MemoryManager::vaddr2paddr(int vaddr)
// {
//     int *pte = (int *)toPTE(vaddr);
//     int page = (*pte) & 0xfffff000;
//     int offset = vaddr & 0xfff;
//     return (page + offset);
// }

// void MemoryManager::releaseVirtualPages(enum AddressPoolType type, const int vaddr, const int count)
// {
//     if (type == AddressPoolType::KERNEL)
//     {
//         kernelVirtual.release(vaddr, count);
//     }
// }