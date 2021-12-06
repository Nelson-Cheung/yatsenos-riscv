#include "uart.h"

void UART::initialize()
{
    uart = (char *)UART_BASE;
    uart[UART_IER] = 0x00;
    uart[UART_LCR] = 1 << 7;
    uart[UART_DLL] = 0x01;
    uart[UART_DLM] = 0x00;
    uart[UART_LCR] = 0x03;
    uart[UART_FCR] = 0x07;
}

void UART::putchar(char c)
{
    while ((uart[UART_LSR] & (1 << 5)) == 0)
        ;
    uart[UART_THR] = c;
}

char UART::getchar()
{
    while ((uart[UART_LSR] & 0x01) == 0)
        ;
    return uart[UART_RHR];
}

void UART::putstr(const char *s) {
    while (*s) {
        putchar(*s);
        ++s;
    }
}