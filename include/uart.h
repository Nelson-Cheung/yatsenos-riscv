#ifndef UART_H
#define UART_H

#define UART_BASE 0x10000000

#define UART_RHR 0
#define UART_THR 0
#define UART_IER 1
#define UART_FCR 2
#define UART_ISR 2
#define UART_LCR 3
#define UART_LSR 5

#define UART_DLL 0
#define UART_DLM 1

class UART {
    private:
        char *uart;

    public:
        void initialize();
        void putchar(char c);
        char getchar();
};

#endif