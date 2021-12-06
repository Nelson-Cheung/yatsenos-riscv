#include "driver.h"

Driver driver;

extern "C" void rv64_kernel_init()
{

    driver.initialize();
    driver.uart.putstr("Hello RISC-V!\n");

}

extern "C" void kernel_entry() {
    rv64_kernel_init();
}