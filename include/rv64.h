#ifndef RV64_H
#define RV64_H

extern "C" void write_stvec(unsigned long val);
extern "C" unsigned long read_stvec();

extern "C" void write_sstatus(unsigned long val);
extern "C" unsigned long read_sstatus();

extern "C" unsigned long read_scause();

#endif