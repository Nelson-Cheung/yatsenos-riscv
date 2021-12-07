#include "driver.h"
#include "rv64.h"
#include "interrupt.h"
#include "utils.h"
#include "mem.h"

Driver driver;
MemoryManager memory_manager;

extern "C" void rv64_kernel_init()
{
    driver.initialize();
    driver.clint.set_interrupt_handler((unsigned long)asm_interrupt_handler);

    // driver.clint.enable_interrupt();

    // driver.clint.enable_timer_interrupt();

    // unsigned long mtime, mtimecmp;
    // mtime = driver.timer.read_mtime();
    // mtimecmp = driver.timer.read_mtimecmp();
    // driver.timer.write_mtimecmp(mtime + 20000);

    // driver.uart.putstr("######  ###  #####   #####        #     #\n");
    // driver.uart.putstr("#     #  #  #     # #     #       #     #\n");
    // driver.uart.putstr("#     #  #  #       #             #     #\n");
    // driver.uart.putstr("######   #   #####  #       ##### #     #\n");
    // driver.uart.putstr("#   #    #        # #              #   #\n");
    // driver.uart.putstr("#    #   #  #     # #     #         # #\n");
    // driver.uart.putstr("#     # ###  #####   #####           #\n");

    // printf("print percentage: %%\n"
    //        "print char \"N\": %c\n"
    //        "print string \"Hello World!\": %s\n"
    //        "print decimal: \"-1234\": %d\n"
    //        "print hexadecimal \"0x7abcdef0\": %x\n",
    //        'N', "Hello World!", -1234, 0x7abcdef0);

    memory_manager.initialize();
}

extern "C" void kernel_entry()
{
    rv64_kernel_init();
}