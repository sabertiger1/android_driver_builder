obj-m += kerneldrv.o
kerneldrv-objs := kerneldrv.o kernel/comm.o kernel/memory.o kernel/process.o kernel/entry.o

KERNELDIR ?= /home/user/Xiaomi_Kernel_OpenSource

all:
	make -C $(KERNELDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- CC=aarch64-linux-gnu-gcc modules

clean:
	make -C $(KERNELDIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- CC=aarch64-linux-gnu-gcc clean 