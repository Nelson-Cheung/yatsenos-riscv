#include "stdio.h"
#include "syscall.h"

int main()
{
    printf("你好，进程!\n");

    unsigned long pid = fork();

    if (pid == 0)
    {
        printf("子进程启动, pid: %ld\n", pid);
    }
    else if (pid > 0)
    {
        printf("父进程fork返回, pid: %ld\n", pid);
    }
    else
    {
        printf("发生错误\n");
    }

    while(true);
}