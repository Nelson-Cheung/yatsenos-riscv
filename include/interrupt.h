#ifndef INTERRUPT_H
#define INTERRUPT_H

extern "C" long asm_interrupt_handler();
// 中断是不可重入的，因为没有和恢复CSR
extern "C" long supervisor_interrupt_handler();

#endif