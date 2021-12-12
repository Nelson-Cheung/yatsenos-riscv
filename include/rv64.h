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

extern "C" unsigned long read_a0();
extern "C" unsigned long read_a1();
extern "C" unsigned long read_a2();
extern "C" unsigned long read_a3();
extern "C" unsigned long read_a4();
extern "C" unsigned long read_a5();
extern "C" unsigned long read_a6();
extern "C" unsigned long read_a7();

struct Registers
{
    unsigned long zero;
    unsigned long ra;
    unsigned long sp;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
};

#include "pcb.h"
extern "C" PCB *switch_to(void *current, void *next);
extern "C" void start_process(PCB *pcb);

#endif