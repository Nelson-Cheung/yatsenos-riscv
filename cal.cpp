#include <iostream>
using namespace std;

#define N ((128UL << 20) >> 12)
#define CAL_A (1UL << 27)
#define CAL_B (1UL << 18)
#define CAL_C (1UL << 9)
#define CAL_X (((CAL_B + CAL_C + 1) * N + 3 * CAL_A + 2 * CAL_B + CAL_C) / (CAL_A - CAL_B - CAL_C - 1) + 1)

int main()
{
    int a[CAL_X];
    // unsigned long N = (128UL << 20) >> 12;
    // unsigned long a = 1UL << 27;
    // unsigned long b = 1UL << 18;
    // unsigned long c = 1UL << 9;

    // unsigned long x = ((b + c + 1) * N + 3 * a + 2 * b + c) / (a - b - c - 1) + 1;

    // a = (N + x) / 512 + 1;
    // b = a / 512 + 1;
    // c = b / 512 + 1;

    // cout << a << " " << b << " " << c << " " << endl;

    // a = N / 512;
    // b = a / 512 + 1;
    // c = b / 512 + 1;

    // cout << a << " " << b << " " << c << " " << endl;

    // unsigned long addr = (N << 12) - 4096UL;
    // a = (addr & (0x1ffUL << 30)) >> 30;
    // b = (addr & (0x1ffUL << 21)) >> 21;
    // c = (addr & (0x1ffUL << 12)) >> 12;

    // cout << a << " " << b << " " << c << " " << endl;

    cout << sizeof(a) / sizeof(int)<< endl;
}