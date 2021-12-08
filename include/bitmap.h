#ifndef BITMAP_H
#define BITMAP_H

class BitMap
{
public:
    // 被管理的资源个数，bitmap的总位数
    unsigned long  length;
    // bitmap的起始地址
    char *bitmap;
public:
    // 初始化
    BitMap();
    // 设置BitMap，bitmap=起始地址，length=总位数(即被管理的资源个数)
    void initialize(char *bitmap, unsigned long  length);
    // 获取第index个资源的状态，true=allocated，false=free
    bool get(unsigned long  index) const;
    // 设置第index个资源的状态，true=allocated，false=free
    void set(unsigned long  index, const bool status);
    // 分配count个连续的资源，若没有则返回-1，否则返回分配的第1个资源单元序号
    unsigned long  allocate(unsigned long  count);
    // 释放第index个资源开始的count个资源
    void release(unsigned long  index, unsigned long  count);
    // 返回Bitmap存储区域
    char *getBitmap();
    // 返回Bitmap的大小
    unsigned long  size() const;
private:
    // 禁止Bitmap之间的赋值
    BitMap(const BitMap &) {}
    void operator=(const BitMap&) {}
};

#endif