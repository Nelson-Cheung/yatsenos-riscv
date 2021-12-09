#include "address_pool.h"
#include "constant.h"

AddressPool::AddressPool()
{
}

// 设置地址池BitMap
void AddressPool::initialize(char *bitmap, unsigned long  length, unsigned long startAddress)
{
    resources.initialize(bitmap, length);
    this->startAddress = startAddress;
}

// 从地址池中分配count个连续页
unsigned long AddressPool::allocate(unsigned long  count)
{
    unsigned long start = resources.allocate(count);
    return (start == -1UL) ? -1UL : (start * PAGE_SIZE + startAddress);
}

// 释放若干页的空间
void AddressPool::release(unsigned long address, unsigned long  amount)
{
    resources.release((address - startAddress) / PAGE_SIZE, amount);
}