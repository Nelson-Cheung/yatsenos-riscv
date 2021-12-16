#ifndef RV64_H
#define RV64_H

#define MSTATUS_MIE (1UL << 3)
#define MSTATUS_MPIE (1UL << 7)
#define MSTATUS_MPP (3UL << 11)

#define MIP_STIP (1UL << 5)

#define MIE_MTIE (1UL << 7)

extern "C" unsigned long read_mie();
extern "C" void write_mie(unsigned long);

extern "C" unsigned long read_mepc();
extern "C" void write_mepc(unsigned long);

extern "C" unsigned long read_mcause();

extern "C" void write_mip(unsigned long);
extern "C" unsigned long read_mip();

extern "C" unsigned long read_mscratch();
extern "C" void write_mscratch(unsigned long);

extern "C" unsigned long read_mstatus();

#define SSTATUS_SIE (1UL << 1)
#define SSTATUS_SPIE (1UL << 5)
#define SSTATUS_SPP (1UL << 8)

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


extern "C" void write_satp(unsigned long);
extern "C" unsigned long read_satp();

extern "C" unsigned long read_sscratch();
extern "C" void write_sscratch(unsigned long);

extern "C" void write_scause(unsigned long);

#endif