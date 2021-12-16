#include "syscall.h"

int main()
{
    write("你好，进程!\n");

    unsigned long pid = fork();
    if (pid == 0)
    {
        write("子进程启动");
    }
    else if (pid > 0)
    {
        write("父进程fork返回");
    }
    else
    {
        write("error\n");
    }

    while (true)
    {
    }
}