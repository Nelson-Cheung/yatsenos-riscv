.global start_syscall

start_syscall:
    addi sp, sp, -8
    sd ra, 0(sp)
    ecall
    ld ra, 0(sp)
    addi sp, sp, 8
    ret
    