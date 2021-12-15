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

## 硬件
```
/dts-v1/;

/ {
	#address-cells = < 0x02 >;
	#size-cells = < 0x02 >;
	compatible = "riscv-virtio";
	model = "riscv-virtio,qemu";

	fw-cfg@10100000 {
		dma-coherent;
		reg = < 0x00 0x10100000 0x00 0x18 >;
		compatible = "qemu,fw-cfg-mmio";
	};

	flash@20000000 {
		bank-width = < 0x04 >;
		reg = < 0x00 0x20000000 0x00 0x2000000 0x00 0x22000000 0x00 0x2000000 >;
		compatible = "cfi-flash";
	};

	chosen {
		bootargs = [ 00 ];
		stdout-path = "/soc/uart@10000000";
	};

	memory@80000000 {
		device_type = "memory";
		reg = < 0x00 0x80000000 0x00 0x8000000 >;
	};

	cpus {
		#address-cells = < 0x01 >;
		#size-cells = < 0x00 >;
		timebase-frequency = < 0x989680 >;

		cpu@0 {
			phandle = < 0x01 >;
			device_type = "cpu";
			reg = < 0x00 >;
			status = "okay";
			compatible = "riscv";
			riscv,isa = "rv64imafdcsu";
			mmu-type = "riscv,sv48";

			interrupt-controller {
				#interrupt-cells = < 0x01 >;
				interrupt-controller;
				compatible = "riscv,cpu-intc";
				phandle = < 0x02 >;
			};
		};

		cpu-map {

			cluster0 {

				core0 {
					cpu = < 0x01 >;
				};
			};
		};
	};

	soc {
		#address-cells = < 0x02 >;
		#size-cells = < 0x02 >;
		compatible = "simple-bus";
		ranges;

		rtc@101000 {
			interrupts = < 0x0b >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x101000 0x00 0x1000 >;
			compatible = "google,goldfish-rtc";
		};

		uart@10000000 {
			interrupts = < 0x0a >;
			interrupt-parent = < 0x03 >;
			clock-frequency = < 0x384000 >;
			reg = < 0x00 0x10000000 0x00 0x100 >;
			compatible = "ns16550a";
		};

		poweroff {
			value = < 0x5555 >;
			offset = < 0x00 >;
			regmap = < 0x04 >;
			compatible = "syscon-poweroff";
		};

		reboot {
			value = < 0x7777 >;
			offset = < 0x00 >;
			regmap = < 0x04 >;
			compatible = "syscon-reboot";
		};

		test@100000 {
			phandle = < 0x04 >;
			reg = < 0x00 0x100000 0x00 0x1000 >;
			compatible = "sifive,test1\0sifive,test0\0syscon";
		};

		pci@30000000 {
			interrupt-map-mask = < 0x1800 0x00 0x00 0x07 >;
			interrupt-map = < 0x00 0x00 0x00 0x01 0x03 0x20 0x00 0x00 0x00 0x02 0x03 0x21 0x00 0x00 0x00 0x03 0x03 0x22 0x00 0x00 0x00 0x04 0x03 0x23 0x800 0x00 0x00 0x01 0x03 0x21 0x800 0x00 0x00 0x02 0x03 0x22 0x800 0x00 0x00 0x03 0x03 0x23 0x800 0x00 0x00 0x04 0x03 0x20 0x1000 0x00 0x00 0x01 0x03 0x22 0x1000 0x00 0x00 0x02 0x03 0x23 0x1000 0x00 0x00 0x03 0x03 0x20 0x1000 0x00 0x00 0x04 0x03 0x21 0x1800 0x00 0x00 0x01 0x03 0x23 0x1800 0x00 0x00 0x02 0x03 0x20 0x1800 0x00 0x00 0x03 0x03 0x21 0x1800 0x00 0x00 0x04 0x03 0x22 >;
			ranges = < 0x1000000 0x00 0x00 0x00 0x3000000 0x00 0x10000 0x2000000 0x00 0x40000000 0x00 0x40000000 0x00 0x40000000 0x3000000 0x04 0x00 0x04 0x00 0x04 0x00 >;
			reg = < 0x00 0x30000000 0x00 0x10000000 >;
			dma-coherent;
			bus-range = < 0x00 0xff >;
			linux,pci-domain = < 0x00 >;
			device_type = "pci";
			compatible = "pci-host-ecam-generic";
			#size-cells = < 0x02 >;
			#interrupt-cells = < 0x01 >;
			#address-cells = < 0x03 >;
		};

		virtio_mmio@10008000 {
			interrupts = < 0x08 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10008000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		virtio_mmio@10007000 {
			interrupts = < 0x07 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10007000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		virtio_mmio@10006000 {
			interrupts = < 0x06 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10006000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		virtio_mmio@10005000 {
			interrupts = < 0x05 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10005000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		virtio_mmio@10004000 {
			interrupts = < 0x04 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10004000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		virtio_mmio@10003000 {
			interrupts = < 0x03 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10003000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		virtio_mmio@10002000 {
			interrupts = < 0x02 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10002000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		virtio_mmio@10001000 {
			interrupts = < 0x01 >;
			interrupt-parent = < 0x03 >;
			reg = < 0x00 0x10001000 0x00 0x1000 >;
			compatible = "virtio,mmio";
		};

		plic@c000000 {
			phandle = < 0x03 >;
			riscv,ndev = < 0x35 >;
			reg = < 0x00 0xc000000 0x00 0x210000 >;
			interrupts-extended = < 0x02 0x0b 0x02 0x09 >;
			interrupt-controller;
			compatible = "sifive,plic-1.0.0\0riscv,plic0";
			#interrupt-cells = < 0x01 >;
			#address-cells = < 0x00 >;
		};

		clint@2000000 {
			interrupts-extended = < 0x02 0x03 0x02 0x07 >;
			reg = < 0x00 0x2000000 0x00 0x10000 >;
			compatible = "sifive,clint0\0riscv,clint0";
		};
	};
};
```
## 迁移进度
1. uart输出。
2. supervisor
3. 中断，时钟中断
4. 

## 局限性
1. 假设内核不超过1MB
2. 不支持中断嵌套


## 问题
1. 从M mode进入S mode前需要关闭PMP，否则会发生illegal instruction
2. timer无法转发到S mode，转发的方法是在M mode中设置mtimecmp为最大值，然后设置mip的stip，mret后即可转发到S mode。S mode处理完成后，通过ecall进入M mode，在M mode中clear stip，从而完成整个时钟中断的处理。
3. 对于Sv39，页地址应该是4KB对齐的。

## Sv39
1. Instruction fetch addresses and load and store effective addresses, which are 64 bits, must have bits 63–39 all equal to bit 38, or else a page-fault exception will occur.
2. A page-fault exception is raised if the physical address is insufficiently aligned.
3. 假设内存有$N$个页表，用于管理的3级页表的数量总和最大值为$M$，则
   $$
    M\le\frac{N+M}{512}+1+\frac{\frac{N+M}{512}+1}{512}+1+\frac{\frac{\frac{N+M}{512}+1}{512}+1}{512}+1
   $$
   解得
   $$
   M\le\frac{(2^{18}+2^{9}+1)N+3\cdot 2^{27}+2\cdot 2^{18}+2^{9}}{2^{27}-2^{18}-2^{9}-1}+1
   $$
4. 对所有内存建立one-to-one映射，在内核态下，虚拟地址等同于物理地址。而且只能这样做

## 进程的实现
1. ELF文件加载中，只需将Program Header中的LOAD segment加载即可。
2. 进程的内存分布如下
```
========================================================================================================================
    Start addr    |   Offset   |     End addr     |  Size   | VM area description
========================================================================================================================
                  |            |                  |         |
 0000000000000000 |    0       | 0000003fffffffff |  256 GB | user-space virtual memory, different per mm
__________________|____________|__________________|_________|___________________________________________________________
                  |            |                  |         |
 0000004000000000 | +256    GB | ffffffbfffffffff | ~16M TB | ... huge, almost 64 bits wide hole of non-canonical
                  |            |                  |         |     virtual memory addresses up to the -256 GB
                  |            |                  |         |     starting offset of kernel mappings.
__________________|____________|__________________|_________|___________________________________________________________
                                                            |
                                                            | Kernel-space virtual memory, shared between all processes:
____________________________________________________________|___________________________________________________________
                  |            |                  |         |
 ffffffc000000000 | -256    GB | ffffffc7ffffffff |   32 GB | kasan
 ffffffcefee00000 | -196    GB | ffffffcefeffffff |    2 MB | fixmap
 ffffffceff000000 | -196    GB | ffffffceffffffff |   16 MB | PCI io
 ffffffcf00000000 | -196    GB | ffffffcfffffffff |    4 GB | vmemmap
 ffffffd000000000 | -192    GB | ffffffdfffffffff |   64 GB | vmalloc/ioremap space
 ffffffe000000000 | -128    GB | ffffffff7fffffff |  124 GB | direct mapping of all physical memory
__________________|____________|__________________|_________|____________________________________________________________
                                                            |
                                                            |
____________________________________________________________|____________________________________________________________
                  |            |                  |         |
 ffffffff00000000 |   -4    GB | ffffffff7fffffff |    2 GB | modules, BPF
 ffffffff80000000 |   -2    GB | ffffffffffffffff |    2 GB | kernel
__________________|____________|__________________|_________|____________________________________________________________    
```

3. 内存划分
```
[0x0000000000000000, 0x0000004000000000) 
user，包含代码段、数据段、堆、栈、命令行参数和环境变量
[0xffffffff80000000, 0xffffffffffffffff] 
kernel，用于进程间共享
```
4. 创建进程：PCB、L2页表、用户栈、程序加载
5. 坑：使用GDB调试，进入用户进程后，需要`delete`所有的断点，然后再执行，否则会报如下错误
```
GDB Cannot insert breakpoint, Cannot access memory at address XXX
```
6. debug技巧：sstatus.sum = 1, 所有内存页的映射U=1，这样既可以在User mode中访问Supervisor mode的页。
7.

# 重要假设
1. 假设时钟中断处理的时间小于时钟中断的间隔
2. 中断嵌套无法处理