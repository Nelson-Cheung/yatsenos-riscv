#ifndef UTILS_H
#define UTILS_H

template<typename T, typename S>
struct pair
{
    T first;
    S second;
};

template<typename T>
void swap(T &x, T &y);

/*
 * 将一个非负整数转换为指定进制表示的字符串。
 * num: 待转换的非负整数。
 * mod: 进制。
 * numStr: 保存转换后的字符串，其中，numStr[0]保存的是num的高位数字，以此类推。
 */

void itos(char *numStr, unsigned int num, unsigned int mod);
// 将从memory开始的length个字节设置为value
void memset(void *memory, char value, int length);
// 上取整
int ceil(const int dividend, const int divisor);
// printf
int printf(const char *const fmt, ...);

void memset(void *memory, char value, unsigned long length);
#endif