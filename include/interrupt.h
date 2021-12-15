#ifndef INTERRUPT_H
#define INTERRUPT_H

extern "C" long asm_interrupt_handler();

// 中断是不可重入的，因为没有和恢复CSR
extern "C" unsigned long supervisor_interrupt_handler(
    unsigned long a0, unsigned long a1,
    unsigned long a2, unsigned long a3,
    unsigned long a4, unsigned long a5,
    unsigned long a6, unsigned long a7);

// M mode interrupt 无法转发到 S mode
extern "C" unsigned long machine_interrupt_handler();

extern "C" void save_supervisor_csr();
extern "C" void restore_supervisor_csr();

#endif