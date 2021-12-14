#include "interrupt.h"
#include "rv64.h"
#include "driver.h"
#include "object.h"
#include "syscall.h"
#include "utils.h"

extern "C" unsigned long supervisor_interrupt_handler()
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
            // supervisor 处理时钟中断
            printf("timer interrupt\n");
            unsigned long mtime = driver.timer.read_mtime();
            driver.timer.write_mtimecmp(mtime + 0xffffffUL);
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
        printf("unhandled exception\n");
    }
}

// extern "C" long machine_interrupt_handler(unsigned long scause)
// {
//     unsigned long cause = read_mcause();
//     unsigned long mask = 0x7fffffffffffffffUL;

//     if (cause >> 63)
//     {
//         // interrupt 转发
//         switch (cause & mask)
//         {
//         case 0x7: // timer
//         {
//             unsigned long sstatus = read_sstatus();
//             unsigned long sie = read_sie();

//             driver.uart.putstr("machine timer interrupt\n");
//             if ((sstatus & (1UL << 1)) && (sie & (1UL << 5)))
//             {
//                 driver.uart.putstr("delegate machine timer interrupt\n");
//                 driver.timer.write_mtimecmp(~(1UL << 63));

//                 unsigned long mip = read_mip();
//                 mask = 1UL << 5;
//                 mip = mip & (~mask);
//                 mip = mip | mask;
//                 write_mip(mip);

//                 unsigned long mie = read_mie();

//             }
//             else
//             {
//                 driver.uart.putstr("ignore machine timer interrupt\n");
//                 unsigned long mtime = driver.timer.read_mtime();
//                 driver.timer.write_mtimecmp(mtime + 0xffffffUL);
//             }

//             break;
//         }
//         default:
//             driver.uart.putstr("unhandled machine interrupt\n");
//             while (true)
//                 ;
//             break;
//         }
//     }
//     else
//     {
//         unsigned long mepc = 0;
//         switch (cause & mask)
//         {
//         case 0x9: // ecall from S mode
//             driver.uart.putstr("ecall from S mode\n");
//             mepc = read_mepc() + 4;
//             write_mepc(mepc);
//             mask = ~(1UL << 63);
//             if ((scause >> 63) && ((scause & mask) == 0x05))
//             {
//                 mask = 1UL << 5;
//                 unsigned long ip = read_mip();
//                 ip = ip & (~mask);
//                 write_mip(ip);
//             }
//             else
//             {
//                 driver.uart.putstr("Unhandled...\n");
//             }
//             break;

//         default:
//             driver.uart.putstr("unhandled machine interrupt\n");
//             while (true)
//                 ;
//             break;
//         }
//     }
// }

long test()
{
    return -1;
}

extern "C" unsigned long machine_interrupt_handler(
    unsigned long a0, unsigned long a1,
    unsigned long a2, unsigned long a3,
    unsigned long a4, unsigned long a5,
    unsigned long a6, unsigned long a7)
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
            reg &= ~MIP_STIP;
            reg |= MIP_STIP;
            write_mip(reg);
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
        if (mcause == 9)
        {
            unsigned long scause;

            scause = read_scause() & ~mask;
            
            if (scause == 5)
            {
                reg = read_mip();
                reg &= ~MIP_STIP;
                write_mip(reg);

                write_mepc(read_mepc() + 4);
            }
        }
        else
        {
            printf("unhandled exception\n");
            while (true)
                ;
        }
    }
}
