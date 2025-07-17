obj-m += kernel_driver_hook_ioctl.o

kernel_driver_hook_ioctl-objs := memory.o process.o entry.o comm.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNELDIR) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
