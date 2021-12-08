#ifndef RV64_H
#define RV64_H

extern "C" unsigned long read_a0();

extern "C" unsigned long read_mepc();
extern "C" void write_mepc(unsigned long);

extern "C" unsigned long read_mcause();

extern "C" void write_mip(unsigned long);
extern "C" unsigned long read_mip();

extern "C" void write_stvec(unsigned long);
extern "C" unsigned long read_stvec();

extern "C" void write_sstatus(unsigned long);
extern "C" unsigned long read_sstatus();

extern "C" unsigned long read_scause();

extern "C" void write_sepc(unsigned long);
extern "C" unsigned long read_sepc();

extern "C" void write_sie(unsigned long);
extern "C" unsigned long read_sie();

extern "C" void write_sip(unsigned long);
extern "C" unsigned long read_sip();

extern "C" void supervisor_timer_interrupt_done();

extern "C" void write_satp(unsigned long);

#endif