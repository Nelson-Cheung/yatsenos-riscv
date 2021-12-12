#include "syscall.h"
#include "utils.h"
#include "object.h"

unsigned long do_write(const char *s)
{
    // 这里使用printf("%s\n", s)有问题，会触发page fault，原因暂时未知
    // 可能和可变参数机制有关？
    driver.uart.putstr(s);
    return 0;
}

void do_exit(long status)
{
    driver.uart.putstr("process exit\n");
    while (true)
        ;
}