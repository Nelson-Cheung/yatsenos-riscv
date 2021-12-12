#include "interrupt.h"
#include "rv64.h"
#include "driver.h"
#include "object.h"
#include "syscall.h"

extern "C" unsigned long supervisor_interrupt_handler(unsigned long a0, unsigned long a1,
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
        case 0x5: // timer
        {
            unsigned long mtime = driver.timer.read_mtime();
            driver.timer.write_mtimecmp(mtime + 0xffffff);
            driver.uart.putstr("timer interrupt\n");
            supervisor_timer_interrupt_done();
            break;
        }
        default:
            driver.uart.putstr("unhandled interrupt\n");
            while (true)
                ;
            break;
        }
    }
    else
    {
        unsigned long sepc = 0;
        switch (cause & mask)
        {
        case 0x8: // ecall from U mode
        {
            if (a0 == SYSCALL_WRITE)
            {
                unsigned long ans = do_write((const char *)a1);
                sepc = read_sepc();
                write_sepc(sepc + 4);
                return ans;
            }
            else if (a0 == SYSCALL_EXIT)
            {
                do_exit(a1);
            }
            else
            {
                printf("unhandled syscall\n");
            }
            break;
        }

        case 0x9: // ecall from S mode
            driver.uart.putstr("ecall from S mode\n");
            sepc = read_sepc() + 4;
            write_sepc(sepc);
            break;

        case 0xc: // Instruction page fault
            sepc = read_sepc();
            if (sepc == 0)
            {
                do_exit(a0);
            }
            else
            {
                printf("unhandled interrupt\n");
            }
        default:
            driver.uart.putstr("unhandled interrupt\n");
            while (true)
                ;
            break;
        }
    }
}

extern "C" long machine_interrupt_handler(unsigned long scause)
{
    unsigned long cause = read_mcause();
    unsigned long mask = 0x7fffffffffffffffUL;

    if (cause >> 63)
    {
        // interrupt 转发
        switch (cause & mask)
        {
        case 0x7: // timer
        {
            driver.timer.write_mtimecmp(~(1UL << 63));
            driver.uart.putstr("machine timer interrupt\n");
            unsigned long mip = read_mip();
            mask = 1UL << 5;
            mip = mip & (~mask);
            mip = mip | mask;
            write_mip(mip);
            break;
        }
        default:
            driver.uart.putstr("unhandled interrupt\n");
            while (true)
                ;
            break;
        }
    }
    else
    {
        unsigned long mepc = 0;
        switch (cause & mask)
        {
        case 0x9: // ecall from S mode
            driver.uart.putstr("ecall from S mode\n");
            mepc = read_mepc() + 4;
            write_mepc(mepc);
            mask = ~(1UL << 63);
            if ((scause >> 63) && ((scause & mask) == 0x05))
            {
                mask = 1UL << 5;
                unsigned long ip = read_mip();
                ip = ip & (~mask);
                write_mip(ip);

                ip = read_sip();
                ip = ip & (~mask);
                write_sip(ip);
            }
            else
            {
                driver.uart.putstr("Unhandled...\n");
            }
            break;

        default:
            driver.uart.putstr("unhandled interrupt\n");
            while (true)
                ;
            break;
        }
    }
}