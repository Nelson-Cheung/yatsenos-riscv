#include "driver.h"
#include "rv64.h"
#include "interrupt.h"
#include "utils.h"
#include "mem.h"
#include "constant.h"

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

    memory_manager.initialize();
}

extern "C" void kernel_entry()
{
    rv64_kernel_init();

    unsigned int n0 = ceil(MEMORY_SIZE, PAGE_SIZE);
    unsigned int n1 = ceil(n0, 512);
    unsigned int n2 = ceil(n1, 512);
    unsigned int n3 = ceil(n2, 512);

    printf("%d %d %d %d\n", n0, n1, n2, n3);
    
    return;
    printf("initialization finish\n");

    printf("######  ###  #####   #####        #     #\n");
    printf("#     #  #  #     # #     #       #     #\n");
    printf("#     #  #  #       #             #     #\n");
    printf("######   #   #####  #       ##### #     #\n");
    printf("#   #    #        # #              #   #\n");
    printf("#    #   #  #     # #     #         # #\n");
    printf("#     # ###  #####   #####           #\n");

    printf("print percentage: %%\n"
           "print char \"N\": %c\n"
           "print string \"Hello World!\": %s\n"
           "print decimal: \"-1234\": %d\n"
           "print hexadecimal \"0x7abcdef0\": %x\n"
           "print long decimal: \"-122147483647\": %ld\n"
           "pritn long hexadecimal \"0x123456789a\": %lx\n",
           'N', "Hello World!", -1234, 0x7abcdef0, -122147483647, 0x123456789a);
}