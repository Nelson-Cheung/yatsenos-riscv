#ifndef MEMORY_H
#define MEMORY_H

#include "address_pool.h"
#include "utils.h"

#define PPN2_MASK (0x1ffUL << 30)
#define PPN1_MASK (0x1ffUL << 21)
#define PPN0_MASK (0x1ffUL << 12)

#define PTE_PPN_MASK (0xfffffffffffUL << 10)

#define PTE_V (1UL << 0)
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
    unsigned long allocate_physical_pages(unsigned long count);

    // 释放从paddr开始的count个物理页
    void release_physical_pages(unsigned long startAddress, unsigned long count);

    // 获取内存总容量
    unsigned long get_total_memory();

    // 开启分页机制
    void open_page_mechanism(const pair<unsigned long, unsigned long> *address, unsigned long size);

    // 建立虚拟地址到物理地址的联系
    void connect_virtual_physical_address(unsigned long l2_page_table, unsigned long paddr, unsigned long vaddr, unsigned long flags);

    // 计算Sv39pte
    unsigned long l2_pte_index(unsigned long virtual_address);
    unsigned long l1_pte_index(unsigned long virtual_address);
    unsigned long l0_pte_index(unsigned long virtual_address);

    unsigned long *l2_pte_pointer(unsigned long virtual_address);
    unsigned long *l1_pte_pointer(unsigned long virtual_address);
    unsigned long *l0_pte_pointer(unsigned long virtual_address);
    unsigned long vaddr2paddr(unsigned long virtual_address);

private:
    unsigned long init_physical_space();
};

#endif