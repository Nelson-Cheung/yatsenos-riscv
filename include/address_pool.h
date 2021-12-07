#ifndef ADDRESS_POOL_H
#define ADDRESS_POOL_H

#include "bitmap.h"

class AddressPool
{
public:
    BitMap resources;
    unsigned long startAddress;

public:
    AddressPool();
    // 初始化地址池
    void initialize(char *bitmap, const int length, unsigned long startAddress);
    // 从地址池中分配count个连续页，成功则返回第一个页的地址，失败则返回-1
    unsigned long allocate(const int count);
    // 释放若干页的空间
    void release(unsigned long address, const int amount);
};

#endif