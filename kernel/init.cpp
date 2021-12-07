#include "driver.h"
#include "rv64.h"
#include "interrupt.h"

Driver driver;

extern "C" void rv64_kernel_init()
{
    driver.initialize();
    driver.clint.set_interrupt_handler((unsigned long)asm_interrupt_handler);

    driver.clint.enable_interrupt();

    // driver.clint.enable_timer_interrupt();

    unsigned long mtime, mtimecmp;
    mtime = driver.timer.read_mtime();
    mtimecmp = driver.timer.read_mtimecmp();
    driver.timer.write_mtimecmp(mtime + 20000);

    // driver.uart.putstr("######  ###  #####   #####        #     #\n");
    // driver.uart.putstr("#     #  #  #     # #     #       #     #\n");
    // driver.uart.putstr("#     #  #  #       #             #     #\n");
    // driver.uart.putstr("######   #   #####  #       ##### #     #\n");
    // driver.uart.putstr("#   #    #        # #              #   #\n");
    // driver.uart.putstr("#    #   #  #     # #     #         # #\n");
    // driver.uart.putstr("#     # ###  #####   #####           #\n");
}

extern "C" void kernel_entry()
{
    rv64_kernel_init();
}