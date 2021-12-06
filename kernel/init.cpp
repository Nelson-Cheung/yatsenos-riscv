#include "driver.h"
#include "rv64.h"
#include "interrupt.h"

Driver driver;

extern "C" void rv64_kernel_init()
{
    driver.initialize();
    driver.clint.set_interrupt_handler((unsigned long)asm_interrupt_handler);

    __asm__("ecall");
    
    driver.uart.putstr("######  ###  #####   #####        #     #\n");
    driver.uart.putstr("#     #  #  #     # #     #       #     #\n");
    driver.uart.putstr("#     #  #  #       #             #     #\n");
    driver.uart.putstr("######   #   #####  #       ##### #     #\n");
    driver.uart.putstr("#   #    #        # #              #   #\n");
    driver.uart.putstr("#    #   #  #     # #     #         # #\n");
    driver.uart.putstr("#     # ###  #####   #####           #\n");
}

extern "C" void kernel_entry()
{
    rv64_kernel_init();
}