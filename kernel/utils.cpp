#include "utils.h"

template<typename T>
void swap(T &x, T &y) {
    T z = x;
    x = y;
    y = z;
}


void itos(char *numStr, unsigned int num, unsigned int mod) {
    

    // 只能转换2~26进制的整数
    if (mod < 2 || mod > 26 || num < 0) {
        return;
    }

    unsigned int length, temp;

    // 进制转换
    length = 0;
    while(num) {
        temp = num % mod;
        num /= mod;
        numStr[length] = temp > 9 ? temp - 10 + 'A' : temp + '0';
        ++length;
    }

    // 特别处理num=0的情况
    if(!length) {
        numStr[0] = '0';
        ++length;
    }

    // 将字符串倒转，使得numStr[0]保存的是num的高位数字
    for(int i = 0, j = length - 1; i < j; ++i, --j) {
        swap(numStr[i], numStr[j]);
    }

    numStr[length] = '\0';
}

void memset(void *memory, char value, int length)
{
    for (int i = 0; i < length; ++i)
    {
        ((char *)memory)[i] = value;
    }
}

int ceil(const int dividend, const int divisor) {
    return (dividend + divisor - 1) / divisor;
}

#include "driver.h"
#include "object.h"
#include "stdarg.h"

int printf_add_to_buffer(char *buffer, char c, int &idx, const int BUF_LEN)
{
    buffer[idx] = c;
    ++idx;

    if (idx == BUF_LEN)
    {
        buffer[idx] = '\0';
        driver.uart.putstr(buffer);
        idx = 0;
    }

    return 0;
}

int printf(const char *const fmt, ...)
{
    const int BUF_LEN = 32;

    char buffer[BUF_LEN + 1];
    char number[33];

    int idx;
    va_list ap;

    va_start(ap, fmt);
    idx = 0;
    // counter = 0;

    for (int i = 0; fmt[i]; ++i)
    {
        if (fmt[i] != '%')
        {
            // counter += printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN);
            printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN);
        }
        else
        {
            i++;
            if (fmt[i] == '\0')
            {
                break;
            }

            switch (fmt[i])
            {
            case '%':
                // counter += printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN);
                printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN);
                break;

            case 'c':
                // counter += printf_add_to_buffer(buffer, va_arg(ap, char), idx, BUF_LEN);
                printf_add_to_buffer(buffer, va_arg(ap, int), idx, BUF_LEN);
                break;

            case 's':
                buffer[idx] = '\0';
                idx = 0;
                // counter += stdio.print(buffer);
                // counter += stdio.print(va_arg(ap, const char *));
                driver.uart.putstr(buffer);
                driver.uart.putstr(va_arg(ap, const char *));
                break;

            case 'd':
            case 'x':
                int temp = va_arg(ap, int);

                if (temp < 0 && fmt[i] == 'd')
                {
                    // counter += printf_add_to_buffer(buffer, '-', idx, BUF_LEN);
                    printf_add_to_buffer(buffer, '-', idx, BUF_LEN);
                    temp = -temp;
                }

                itos(number, temp, (fmt[i] == 'd' ? 10 : 16));

                for (int j = 0; number[j]; ++j)
                {
                    // counter += printf_add_to_buffer(buffer, number[j], idx, BUF_LEN);
                    printf_add_to_buffer(buffer, number[j], idx, BUF_LEN);
                }
                break;
            }
        }
    }

    buffer[idx] = '\0';
    // counter += stdio.print(buffer);
    driver.uart.putstr(buffer);

    return 0;
}