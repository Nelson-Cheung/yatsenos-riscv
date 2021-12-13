.global _start

.text
_start:
    nop
    la sp, stack0

    # 关闭PMP
    li t0, 0x3fffffffffffff
    csrw pmpaddr0, t0
    li t0, 0x0f
    csrw pmpcfg0, t0

    # 设置machine mode中断处理程序地址
    la t0, _machine_interrupt_handler
    csrw mtvec, t0

    la t0, mstack0
    csrw mscratch, t0
    la t0, sstack0
    csrw sscratch, t0

    # 设置S mode中断处理程序地址
    la t0, _supervisor_interrupt_handler
    csrw stvec, t0

    # 将M mode的中断转发到S mode中处理
    li t0, 0x222
    csrs mideleg, t0
    li t0, 0xb1ff
    csrs medeleg, t0

    # enable interrupt
    li t0, 0x80
    csrs mstatus, t0
    li t0, 0x888
    csrs mie, t0

    # 从M mode进入S mode
    li t0, 3 << 11
    csrc mstatus, t0
    li t0, 1 << 11
    csrs mstatus, t0
    la t0, _enter_supervisor_mode
    csrw mepc, t0
    mret 

_enter_supervisor_mode:
    csrw sstatus, zero
    csrw sie, zero

    # DEBUG所用
    li t0, 1 << 18
    csrs sstatus, t0
    ########

    call kernel_entry
    
    # 设置用户态
#     la t0, _enter_user_mode
#     csrw sepc, t0
#     li t0, 1 << 8
#     csrc sstatus, t0
#     sret

# _enter_user_mode:
#     ecall

_finish:
    j _finish

_machine_interrupt_handler:
    # 不要出现load、store指令，否则会出错
    csrrw sp, mscratch, sp
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

    call machine_interrupt_handler

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
    csrrw sp, mscratch, sp

    mret

_machine_interrupt_error:
    j _machine_interrupt_error

_supervisor_interrupt_handler:
    csrr t0, sstatus
    csrr t1, sip
    csrr t2, scause
    j _supervisor_interrupt_handler