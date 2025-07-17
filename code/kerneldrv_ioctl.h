#ifndef KERNELDRV_IOCTL_H
#define KERNELDRV_IOCTL_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <sys/types.h>
#endif

struct mem_op {
    pid_t pid;
    uintptr_t addr;
    uintptr_t buffer;
    size_t size;
};

#define IOCTL_READ_MEM  _IOR('k', 1, struct mem_op)
#define IOCTL_WRITE_MEM _IOW('k', 2, struct mem_op)

#endif 