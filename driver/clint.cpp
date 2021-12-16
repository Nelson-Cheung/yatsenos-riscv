#include "clint.h"
#include "rv64.h"
#include "driver.h"
#include "object.h"

void CLINT::set_interrupt_handler(unsigned long address)
{
    write_stvec(address);
}

void CLINT::enable_interrupt()
{
    unsigned long sstauts = read_sstatus();
    unsigned long mask = 1UL << 1;
    sstauts = sstauts & (~mask);
    sstauts = sstauts | mask;
    write_sstatus(sstauts);

    // unsigned long sie = read_sie();
    // mask = (1UL << 9) | (1UL << 5) | (1UL << 1);
    // sie = sie & (~mask);
    // sie = sie | mask;
    // write_sie(sie);
}

// void enable_software_interrupt();
void CLINT::enable_timer_interrupt()
{
    unsigned long sie = read_sie();
    unsigned long mask = 1UL << 5;
    sie = sie & (~mask);
    sie = sie | mask;
    write_sie(sie);
}

// void enable_external_interrupt();

void CLINT::disable_interrupt()
{
    unsigned long sstauts = read_sstatus();
    unsigned long mask = 1UL << 1;
    sstauts = sstauts & (~mask);
    write_sstatus(sstauts);

    // unsigned long sie = read_sie();
    // mask = (1UL << 9) | (1UL << 5) | (1UL << 1);
    // sie = sie & (~mask);
    // write_sie(sie);
}

// void disable_software_interrupt();
void CLINT::disable_timer_interrupt()
{
    unsigned long sie = read_sie();
    unsigned long mask = 1UL << 5;
    sie = sie & (~mask);
    write_sie(sie);
}
// void disable_external_interrupt();

extern "C" void machine_interrupt_redirect()
{
    unsigned long mcause = read_mcause();
    unsigned long mask = ~(1UL << 63);

    if (mcause >> 63)
    {
        switch (mcause & mask)
        {
        case 0x7: // machine timer interrupt
        {

            driver.timer.write_mtimecmp(~(0UL));
            unsigned long mip = read_mip();
            mask = 1UL << 5;
            mip = mask & (~mask);
            mip = mip | mask;
            write_mip(mip);
            break;
        }

        default:
            driver.uart.putstr("can not direct machine interrupt\n");
            while (true)
                ;
            break;
        }
    }
    else
    {
        driver.uart.putstr("can not direct machine exception\n");
        while (true)
            ;
    }
}