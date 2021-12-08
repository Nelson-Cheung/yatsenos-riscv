#include "bitmap.h"
#include "utils.h"

BitMap::BitMap()
{
}

void BitMap::initialize(char *bitmap, unsigned long length)
{
    this->bitmap = bitmap;
    this->length = length;

    unsigned long bytes = ceil(length, 8);

    for (unsigned long i = 0; i < bytes; ++i)
    {
        bitmap[i] = 0;
    }
}

bool BitMap::get(unsigned long index) const
{
    unsigned long pos = index / 8;
    unsigned long offset = index % 8;

    return (bitmap[pos] & (1 << offset));
}

void BitMap::set(unsigned long index, const bool status)
{
    unsigned long pos = index / 8;
    unsigned long offset = index % 8;

    // 清0
    bitmap[pos] = bitmap[pos] & (~(1 << offset));

    // 置1
    if (status)
    {
        bitmap[pos] = bitmap[pos] | (1 << offset);
    }
}

unsigned long BitMap::allocate(unsigned long count)
{
    if (count == 0)
        return -1;

    unsigned long index, empty, start;

    index = 0;
    while (index < length)
    {
        // 越过已经分配的资源
        while (index < length && get(index))
            ++index;

        // 不存在连续的count个资源
        if (index == length)
            return -1;

        // 找到1个未分配的资源
        // 检查是否存在从index开始的连续count个资源
        empty = 0;
        start = index;
        while ((index < length) && (!get(index)) && (empty < count))
        {
            ++empty;
            ++index;
        }

        // 存在连续的count个资源
        if (empty == count)
        {
            for (unsigned long i = 0; i < count; ++i)
            {
                set(start + i, true);
            }

            return start;
        }
    }

    return -1;
}

void BitMap::release(unsigned long index, unsigned long count)
{
    for (unsigned long i = 0; i < count; ++i)
    {
        set(index + i, false);
    }
}

char *BitMap::getBitmap()
{
    return (char *)bitmap;
}

unsigned long BitMap::size() const
{
    return length;
}