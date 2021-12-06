.global _start

.text
_start:
    nop
    la sp, stack0
    call kernel_init
_finish:
    j _finish

.data
stack0 : .word 4096
