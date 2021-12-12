.global asm_interrupt_handler
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
.global supervisor_timer_interrupt_done
.global write_satp
.global switch_to 
.global start_process

switch_to:
    addi sp, sp, -8
    sd ra, 0(sp)
    
    sd sp, 0(a0)
    ld sp, 0(a1)

    ld ra, 0(sp)
    addi sp, sp, 8

    mv a0, a1

    ret

start_process:
    # 切换页表
    li t0, 0xfffff00000000000
    csrr t1, satp
    and t0, t1, t0
    ld t1, 0(sp)
    srli t1, t1, 12
    or t0, t1, t0
    csrw satp, t0

    # 设置进程进入点
    ld t0, 8(sp)
    csrw sepc, t0

    addi sp, sp, 8 * 2

    mv t0, sp
    addi t0, t0, 8 * 32
    sd t0, 0(a0)

    # 设置用户态
    csrs sstatus, zero

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

    ld x2, 8 * 2 (sp)

    sret

# extern "C" void write_satp(unsigned long);
write_satp:
    csrw satp, a0
    ret

# extern "C" unsigned long read_mepc();
read_mepc:
    csrr a0, mepc
    ret

# extern "C" void write_mepc(unsigned long);
write_mepc:
    csrw mepc, a0
    ret

# extern "C" void supervisor_timer_interrupt_done();
supervisor_timer_interrupt_done:
    addi sp, sp, -8
    sd ra, 0(sp)

    csrr a0, scause
    ecall

    ld ra, 0(sp)
    addi sp, sp, 8
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

asm_interrupt_handler:
    call supervisor_interrupt_handler
    sret


