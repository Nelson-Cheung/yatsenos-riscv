#include "driver.h"

extern "C" void kernel_init() {
    Driver driver;
    driver.initialize();
    
    driver.uart.putchar('H');
    driver.uart.putchar('e');
    driver.uart.putchar('l');
    driver.uart.putchar('l');
    driver.uart.putchar('o');
    driver.uart.putchar(' ');
    driver.uart.putchar('R');
    driver.uart.putchar('I');
    driver.uart.putchar('S');
    driver.uart.putchar('C');
    driver.uart.putchar('-');
    driver.uart.putchar('V');
    driver.uart.putchar('!');
    driver.uart.putchar('\n');

    while(true);
}