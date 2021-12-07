#ifndef INTERRUPT_H
#define INTERRUPT_H

extern "C" long asm_interrupt_handler();
// 中断是不可重入的，因为没有和恢复CSR
extern "C" long supervisor_interrupt_handler();
// M mode interrupt 无法转发到 S mode
extern "C" long machine_interrupt_handler(unsigned long scause);

class InterruptManager
{
public:
    void handle_timer_interrupt();
};
#endif