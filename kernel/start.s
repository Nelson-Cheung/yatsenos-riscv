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

    # 设置S mode中断处理程序地址
    la t0, _supervisor_interrupt_handler
    csrw stvec, t0

    # 将M mode的中断转发到S mode中处理
    li t0, 0x222
    csrw mideleg, t0
    li t0, 0xb3ff
    csrw medeleg, t0

    # 从M mode进入S mode
    li t0, 3 << 11
    csrc mstatus, t0
    li t0, 1 << 11
    csrs mstatus, t0
    la t0, _enter_supervisor_mode
    csrw mepc, t0
    mret 

_enter_supervisor_mode:
    call kernel_entry

_finish:
    j _finish

_machine_interrupt_handler:
    csrr t0, mstatus
    csrr t1, mip
    csrr t2, mcause
    j _machine_interrupt_handler

_supervisor_interrupt_handler:
    csrr t0, sstatus
    csrr t1, sip
    csrr t2, scause
    j _supervisor_interrupt_handler

.data
stack0 : .word 4096
