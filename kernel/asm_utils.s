.global supervisor_interrupt_entry
.global write_stvec
.global read_stvec
.global write_sstatus
.global read_sstatus
.global read_scause
.global write_sepc
.global read_sepc
.global write_sie
.global read_sie
.global write_mip
.global read_mip
.global read_mcause
.global write_sip
.global read_sip
.global read_mepc
.global write_mepc
.global write_satp
.global switch_to 
.global start_process
.global read_satp
.global read_mie
.global write_mie
.global read_mscratch
.global read_sscratch
.global write_mscratch
.global write_sscratch
.global read_mstatus
.global write_scause
.global save_supervisor_csr
.global restore_supervisor_csr

# extern "C" void save_supervisor_csr();
save_supervisor_csr:
    csrrw sp, mscratch, sp
    
    # 此时的sp指向supervisor栈
    addi sp, sp, -8 * 3
    csrr t0, sstatus
    sd t0, 0 * 8(sp)
    csrr t0, sepc
    sd t0, 1 * 8(sp)
    csrr t0, scause
    sd t0, 2 * 8(sp)

    addi sp, sp, -8 * 16

    sd ra, 0 * 8(sp)

    sd t0, 1 * 8(sp)
    sd t1, 2 * 8(sp)
    sd t2, 3 * 8(sp)
    sd t3, 4 * 8(sp)
    sd t4, 5 * 8(sp)
    sd t5, 6 * 8(sp)
    sd t6, 7 * 8(sp)

    sd a0, 8 * 8(sp)
    sd a1, 9 * 8(sp)
    sd a2, 10 * 8(sp)
    sd a3, 11 * 8(sp)
    sd a4, 12 * 8(sp)
    sd a5, 13 * 8(sp)
    sd a6, 14 * 8(sp)
    sd a7, 15 * 8(sp)

    csrrw sp, sscratch, sp
    csrrw sp, mscratch, sp
    ret

# extern "C" void restore_supervisor_csr();
restore_supervisor_csr:
    csrrw sp, sscratch, sp

    ld ra, 0 * 8(sp)

    ld t0, 1 * 8(sp)
    ld t1, 2 * 8(sp)
    ld t2, 3 * 8(sp)
    ld t3, 4 * 8(sp)
    ld t4, 5 * 8(sp)
    ld t5, 6 * 8(sp)
    ld t6, 7 * 8(sp)

    ld a0, 8 * 8(sp)
    ld a1, 9 * 8(sp)
    ld a2, 10 * 8(sp)
    ld a3, 11 * 8(sp)
    ld a4, 12 * 8(sp)
    ld a5, 13 * 8(sp)
    ld a6, 14 * 8(sp)
    ld a7, 15 * 8(sp)

    addi sp, sp, 8 * 16

    # 此时的sp指向supervisor栈
    ld t0, 0 * 8(sp)
    csrw sstatus, t0
    ld t0, 1 * 8(sp)
    csrw sepc, t0
    ld t0, 2 * 8(sp)
    csrw scause, t0
    addi sp, sp, 8 * 3

    csrrw sp, mscratch, sp
    csrrw sp, sscratch, sp
    ret

# extern "C" void write_scause(unsigned long)
write_scause:
    csrw scause, a0
    ret

# extern "C" unsigned long read_mstatus();
read_mstatus:
    csrr a0, mstatus
    ret
# extern "C" unsigned long read_mscratch();
read_mscratch:
    csrr a0, mscratch
    ret

# extern "C" unsigned long read_sscratch();
read_sscratch:
    csrr a0, sscratch
    ret

# extern "C" void write_mscratch(unsigned long);
write_mscratch:
    csrw mscratch, a0
    ret

# extern "C" void write_sscratch(unsigned long);
write_sscratch:
    csrw sscratch, a0
    ret

# extern "C" unsigned long read_mie();
read_mie:
    csrr a0, mie
    ret

# extern "C" void write_mie(unsigned long);
write_mie:
    csrw mie, a0
    ret

# extern "C" unsigned long read_satp();
read_satp:
    csrr a0, satp
    ret

switch_to:
    addi sp, sp, -8 * 4

    sd ra, 0 * 8(sp)
    csrr t0, sstatus
    sd t0, 1 * 8(sp)
    csrr t0, sepc
    sd t0, 2 * 8(sp)
    csrr t0, scause
    sd t0, 3 * 8(sp)

    sd sp, 0(a0)
    ld sp, 0(a1)

    ld ra, 0 * 8(sp)
    ld t0, 1 * 8(sp)
    csrw sstatus, t0
    ld t0, 2 * 8(sp)
    csrw sepc, t0
    ld t0, 3 * 8(sp)
    csrw scause, t0

    addi sp, sp, 8 * 4

    ret

start_process:   
    addi t0, sp, 8 * 32
    csrrw t0, sscratch, t0

    li t0, 0x8000000000000005
    csrw scause, t0
    ecall

    ld x1, 8 * 1 (sp)
    # sp 最后设置
    ld x3, 8 * 3 (sp)
    ld x4, 8 * 4 (sp)
    ld x5, 8 * 5 (sp)
    ld x6, 8 * 6 (sp)
    ld x7, 8 * 7 (sp)
    ld x8, 8 * 8 (sp)
    ld x9, 8 * 9 (sp)
    ld x10, 8 * 10 (sp)
    ld x11, 8 * 11 (sp)
    ld x12, 8 * 12 (sp)
    ld x13, 8 * 13 (sp)
    ld x14, 8 * 14 (sp)
    ld x15, 8 * 15 (sp)
    ld x16, 8 * 16 (sp)
    ld x17, 8 * 17 (sp)
    ld x18, 8 * 18 (sp)
    ld x19, 8 * 19 (sp)
    ld x20, 8 * 20 (sp)
    ld x21, 8 * 21 (sp)
    ld x22, 8 * 22 (sp)
    ld x23, 8 * 23 (sp)
    ld x24, 8 * 24 (sp)
    ld x25, 8 * 25 (sp)
    ld x26, 8 * 26 (sp)
    ld x27, 8 * 27 (sp)
    ld x28, 8 * 28 (sp)
    ld x29, 8 * 29 (sp)
    ld x30, 8 * 30 (sp)
    ld x31, 8 * 31 (sp)

    ld sp, 8 * 2 (sp)

    sret

# extern "C" void write_satp(unsigned long);
write_satp:
    csrw satp, a0
    sfence.vma zero, zero
    ret

# extern "C" unsigned long read_mepc();
read_mepc:
    csrr a0, mepc
    ret

# extern "C" void write_mepc(unsigned long);
write_mepc:
    csrw mepc, a0
    ret

# extern "C" void write_sip(unsigned long);
write_sip:
    csrw sip, a0
    ret

# extern "C" unsigned long read_sip();
read_sip:
    csrr a0, sip
    ret

# extern "C" unsigned long read_mcause();
read_mcause:
    csrr a0, mcause
    ret

# extern "C" void write_mip(unsigned long);
write_mip:
    csrw mip, a0
    ret

# extern "C" unsigned long read_mip();
read_mip:
    csrr a0, mip
    ret

# extern "C" void write_sie(unsigned long);
write_sie:
    csrw sie, a0
    ret

# extern "C" unsigned long read_sie();
read_sie:
    csrr a0, sie
    ret

# extern "C" void write_sepc(unsigned long);
write_sepc:
    csrw sepc, a0
    ret
    
# extern "C" unsigned long read_sepc();
read_sepc:
    csrr a0, sepc
    ret

# extern "C" void write_stvec(long val);
write_stvec:
    csrw stvec, a0
    ret

# extern "C" long read_stvec();
read_stvec:
    csrr a0, stvec
    ret

# extern "C" void write_sstatus(long val);
write_sstatus:
    csrw sstatus, a0
    ret

# extern "C" long read_sstatus();
read_sstatus:
    csrr a0, sstatus
    ret

# extern "C" long read_scause();
read_scause:
    csrr a0, scause
    ret

supervisor_interrupt_entry:
    # 顺序十分重要
    csrrw sp, sscratch, sp
    addi sp, sp, -8 * 32

    sd ra, 8 * 1 (sp)
    sd sp, 8 * 2 (sp)
    sd gp, 8 * 3 (sp)
    sd tp, 8 * 4 (sp)
    sd t0, 8 * 5 (sp)
    sd t1, 8 * 6 (sp)
    sd t2, 8 * 7 (sp)
    sd s0, 8 * 8 (sp)
    sd s1, 8 * 9 (sp)
    sd a0, 8 * 10 (sp)
    sd a1, 8 * 11 (sp)
    sd a2, 8 * 12 (sp)
    sd a3, 8 * 13 (sp)
    sd a4, 8 * 14 (sp)
    sd a5, 8 * 15 (sp)
    sd a6, 8 * 16 (sp)
    sd a7, 8 * 17 (sp)
    sd s2, 8 * 18 (sp)
    sd s3, 8 * 19 (sp)
    sd s4, 8 * 20 (sp)
    sd s5, 8 * 21 (sp)
    sd s6, 8 * 22 (sp)
    sd s7, 8 * 23 (sp)
    sd s8, 8 * 24 (sp)
    sd s9, 8 * 25 (sp)
    sd s10, 8 * 26 (sp)
    sd s11, 8 * 27 (sp)
    sd t3, 8 * 28 (sp)
    sd t4, 8 * 29 (sp)
    sd t5, 8 * 30 (sp)
    sd t6, 8 * 31 (sp)

    call supervisor_interrupt_handler

    ld ra, 8 * 1 (sp)
    ld sp, 8 * 2 (sp)
    ld gp, 8 * 3 (sp)
    ld tp, 8 * 4 (sp)
    ld t0, 8 * 5 (sp)
    ld t1, 8 * 6 (sp)
    ld t2, 8 * 7 (sp)
    ld s0, 8 * 8 (sp)
    ld s1, 8 * 9 (sp)
    # ld a0, 8 * 10 (sp) # 返回值
    ld a1, 8 * 11 (sp)
    ld a2, 8 * 12 (sp)
    ld a3, 8 * 13 (sp)
    ld a4, 8 * 14 (sp)
    ld a5, 8 * 15 (sp)
    ld a6, 8 * 16 (sp)
    ld a7, 8 * 17 (sp)
    ld s2, 8 * 18 (sp)
    ld s3, 8 * 19 (sp)
    ld s4, 8 * 20 (sp)
    ld s5, 8 * 21 (sp)
    ld s6, 8 * 22 (sp)
    ld s7, 8 * 23 (sp)
    ld s8, 8 * 24 (sp)
    ld s9, 8 * 25 (sp)
    ld s10, 8 * 26 (sp)
    ld s11, 8 * 27 (sp)
    ld t3, 8 * 28 (sp)
    ld t4, 8 * 29 (sp)
    ld t5, 8 * 30 (sp)
    ld t6, 8 * 31 (sp)

    addi sp, sp, 8 * 32

    csrrw sp, sscratch, sp
    ecall
    sret
