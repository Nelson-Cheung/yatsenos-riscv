#include "interrupt.h"
#include "rv64.h"
#include "driver.h"
#include "object.h"
#include "syscall.h"
#include "utils.h"

extern "C" unsigned long supervisor_interrupt_handler(
    unsigned long a0, unsigned long a1,
    unsigned long a2, unsigned long a3,
    unsigned long a4, unsigned long a5,
    unsigned long a6, unsigned long a7)
{
    unsigned long cause = read_scause();
    unsigned long mask = 0x7fffffffffffffffL;

    if (cause >> 63)
    {
        // 异步exception
        switch (cause & mask)
        {
        case 5: // timer
        {
            // supervisor 处理时钟中断
            printf("timer interrupt\n");
            unsigned long mtime = driver.timer.read_mtime();
            driver.timer.write_mtimecmp(mtime + TIME_INTERRUPT_DURATION);
            process_manager.schedule();
            break;
        }
        default:
            printf("unhandled interrupt\n");
            while (true)
                ;
            break;
        }
    }
    else
    {
        switch (cause & mask)
        {
        case 8: // syscall
        {
            unsigned long reg = read_sepc();
            write_sepc(reg + 4);
            driver.clint.enable_interrupt();
            reg = syscall_manager.handle_syscall(a0, a1, a2, a3, a4, a5, a6, a7);
            driver.clint.disable_interrupt();
            return reg;
            break;
        }
        case 12:
        {
            if (read_sepc() == 0)
            {
                syscall_manager.handle_syscall(SYSCALL_EXIT, a0, 0, 0, 0, 0, 0, 0);
            }
        }
        default:
            printf("unhandled interrupt\n");
            while (true)
                ;
            break;
        }
    }
}

extern "C" unsigned long machine_interrupt_handler()
{
    unsigned long mask, mcause, reg;

    mask = 1UL << 63;
    mcause = read_mcause();

    if (mcause & mask)
    {
        // interrupt
        mcause &= ~mask;
        if (mcause == 7)
        {
            // 转发timer中断到S mode处理，关闭timer中断
            printf("delegate machine timer interrupt\n");

            driver.timer.write_mtimecmp(~(1UL << 63));

            reg = read_mip();
            reg &= ~(MIP_STIP);
            reg |= MIP_STIP;
            write_mip(reg);

            reg = read_mstatus();
            if (((reg & MSTATUS_MPP) >> 11) == 1UL)
            {
                // 中断嵌套
                save_supervisor_csr();
            }
        }
        else
        {
            printf("unhandled exception\n");
            while (true)
                ;
        }
    }
    else
    {
        // exception
        mcause &= ~mask;
        if (mcause == 9) // S mode
        {
            unsigned long scause;

            scause = read_scause() & ~mask;

            if (scause == 5)
            {
                reg = read_mip();
                reg &= ~MIP_STIP;
                write_mip(reg);

                reg = read_sstatus();
                if (reg & SSTATUS_SPP)
                {
                    reg = read_sepc();
                    write_mepc(reg);
                    restore_supervisor_csr();
                    return 0;
                }
            }

            write_mepc(read_mepc() + 4);
        }
        else
        {
            printf("unhandled exception\n");
            while (true)
                ;
        }
    }
}
