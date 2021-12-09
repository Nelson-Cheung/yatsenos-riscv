#ifndef MEMORY_H
#define MEMORY_H

#include "address_pool.h"
#include "utils.h"

#define PPN2_MASK (0x1ffUL << 30)
#define PPN1_MASK (0x1ffUL << 21)
#define PPN0_MASK (0x1ffUL << 12)

#define PTE_V 1UL
#define PTE_R (1UL << 1)
#define PTE_W (1UL << 2)
#define PTE_X (1UL << 3)
#define PTE_U (1UL << 4)

#define SATP_MODE (0xfUL << 60)

enum AddressPoolType
{
    USER,
    KERNEL
};

class MemoryManager
{
public:
    // 可管理的内存容量
    unsigned long totalMemory;
    // 物理地址池
    AddressPool physical_address_pool;
    // 内核虚拟地址池
    AddressPool kernelVirtual;
    // L2页表
    unsigned long l2_page_table;

public:
    MemoryManager();

    // 初始化地址池
    void initialize();

    // 从type类型的物理地址池中分配count个连续的页
    // 成功，返回起始地址；失败，返回0
    unsigned long allocatePhysicalPages(unsigned long count);

    // 释放从paddr开始的count个物理页
    void releasePhysicalPages(unsigned long startAddress, unsigned long count);

    // 获取内存总容量
    unsigned long getTotalMemory();

    // 开启分页机制
    void openPageMechanism(const pair<unsigned long, unsigned long> *address, unsigned long size);

    // 页内存分配
    unsigned long allocatePages(enum AddressPoolType type, unsigned long count);

    // 虚拟页分配
    unsigned long allocateVirtualPages(enum AddressPoolType type, unsigned long count);

    // 建立虚拟页到物理页的联系
    bool connectPhysicalVirtualPage(unsigned long virtualAddress, unsigned long physicalPageAddress);

    // 计算virtualAddress的页目录项的虚拟地址
    unsigned long toPDE(unsigned long virtualAddress);

    // 计算virtualAddress的页表项的虚拟地址
    unsigned long toPTE(unsigned long virtualAddress);

    // 页内存释放
    void releasePages(enum AddressPoolType type, unsigned long virtualAddress, unsigned long count);

    // 找到虚拟地址对应的物理地址
    unsigned long vaddr2paddr(unsigned long vaddr);

    // 释放虚拟页
    void releaseVirtualPages(enum AddressPoolType type, unsigned long vaddr, unsigned long count);

private:
    unsigned long init_physical_space();
};

#endif