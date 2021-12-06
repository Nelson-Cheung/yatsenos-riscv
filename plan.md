# YatSenOS RISC-V 64 迁移计划
## 已探索的部分
+ uart driver: 字符输入输出。
+ rtc driver: 实时钟。
+ interrupt driver：CLINT，特权级。
+ Sv39：分页机制。

## 待探索的部分
+ virtio driver: 磁盘输入输出设备。

## YatSenOS各部分整理。
+ 基本汇编，字符输入输出。
+ 从实模式到保护模式，IO端口编程以实现硬件交互。
+ C++的引入，中断处理。
+ printf，内核线程的实现。
+ 锁，信号量。
+ 分页机制和内存管理。
+ 系统调用，用户线程，fork/wait/exit
+ ......

## 迁移进度
1. uart输出。
2. supervisor