#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include "kerneldrv_ioctl.h"
// #include <linux/miscdevice.h> // 移除miscdevice相关

#define DEVICE_NAME "kerneldrv"
#define IOCTL_READ_MEM  _IOR('k', 1, struct mem_op)
#define IOCTL_WRITE_MEM _IOW('k', 2, struct mem_op)

// struct mem_op 已在头文件定义，无需重复定义

static long kerneldrv_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct mem_op op;
    struct task_struct *task;
    void *kbuf = NULL;
    int ret = 0;

    if (copy_from_user(&op, (void __user *)arg, sizeof(op))) {
        pr_err("[kerneldrv] copy_from_user 结构体失败\n");
        return -EFAULT;
    }

    if (op.addr == 0 || op.size == 0) {
        pr_warn("[kerneldrv] 警告: 收到无效参数 ioctl, pid=%d, addr=0x%lx, size=%zu, 直接返回\n", op.pid, (unsigned long)op.addr, op.size);
        return 0;
    }

    pr_info("[kerneldrv] ioctl cmd=0x%x, pid=%d, addr=0x%lx, size=%zu\n", cmd, op.pid, (unsigned long)op.addr, op.size);

    task = pid_task(find_vpid(op.pid), PIDTYPE_PID);
    if (!task) {
        pr_err("[kerneldrv] 找不到进程: pid=%d\n", op.pid);
        return -ESRCH;
    }

    kbuf = kmalloc(op.size, GFP_KERNEL);
    if (!kbuf) {
        pr_err("[kerneldrv] 分配内存失败, size=%zu\n", op.size);
        return -ENOMEM;
    }

    if (cmd == IOCTL_READ_MEM) {
        int n = access_process_vm(task, (unsigned long)op.addr, kbuf, op.size, 0);
        pr_info("[kerneldrv] 读内存: pid=%d, addr=0x%lx, size=%zu, n=%d\n", op.pid, (unsigned long)op.addr, op.size, n);
        if (n > 0)
            ret = copy_to_user((void __user *)op.buffer, kbuf, op.size) ? -EFAULT : 0;
        else {
            pr_err("[kerneldrv] 读内存失败, n=%d\n", n);
            ret = -EFAULT;
        }
    } else if (cmd == IOCTL_WRITE_MEM) {
        pr_info("[kerneldrv] 请求写内存: pid=%d, addr=0x%lx, size=%zu, buffer=0x%lx\n", op.pid, (unsigned long)op.addr, op.size, (unsigned long)op.buffer);
        if (copy_from_user(kbuf, (void __user *)op.buffer, op.size)) {
            pr_err("[kerneldrv] 写内存 copy_from_user 失败, buffer=0x%lx, size=%zu\n", (unsigned long)op.buffer, op.size);
            ret = -EFAULT;
        } else {
            int n = access_process_vm(task, (unsigned long)op.addr, kbuf, op.size, 1);
            pr_info("[kerneldrv] 写内存: pid=%d, addr=0x%lx, size=%zu, n=%d\n", op.pid, (unsigned long)op.addr, op.size, n);
            if (n <= 0) {
                pr_err("[kerneldrv] 写内存失败, n=%d\n", n);
            }
            ret = (n > 0) ? 0 : -EFAULT;
        }
    } else {
        pr_err("[kerneldrv] 未知命令: 0x%x\n", cmd);
        ret = -EINVAL;
    }
    kfree(kbuf);
    return ret;
}

static int major = 0;
static struct class *kerneldrv_class = NULL;
static struct device *kerneldrv_device = NULL;

static int kerneldrv_open(struct inode *inode, struct file *file) { return 0; }
static int kerneldrv_release(struct inode *inode, struct file *file) { return 0; }

static const struct file_operations kerneldrv_fops = {
    .owner = THIS_MODULE,
    .open = kerneldrv_open,
    .release = kerneldrv_release,
    .unlocked_ioctl = kerneldrv_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = kerneldrv_ioctl,
#endif
};

static int __init kerneldrv_init(void) {
    printk("=== [kerneldrv] kerneldrv_init called ===\n");
    major = register_chrdev(0, DEVICE_NAME, &kerneldrv_fops);
    if (major < 0) {
        printk("=== [kerneldrv] register_chrdev failed: %d ===\n", major);
        return major;
    }
    kerneldrv_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(kerneldrv_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk("=== [kerneldrv] class_create failed ===\n");
        return PTR_ERR(kerneldrv_class);
    }
    kerneldrv_device = device_create(kerneldrv_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(kerneldrv_device)) {
        class_destroy(kerneldrv_class);
        unregister_chrdev(major, DEVICE_NAME);
        printk("=== [kerneldrv] device_create failed ===\n");
        return PTR_ERR(kerneldrv_device);
    }
    printk("=== [kerneldrv] loaded, device: /dev/%s ===\n", DEVICE_NAME);
    return 0;
}

static void __exit kerneldrv_exit(void) {
    printk("=== [kerneldrv] kerneldrv_exit called ===\n");
    device_destroy(kerneldrv_class, MKDEV(major, 0));
    class_destroy(kerneldrv_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk("[kerneldrv] unloaded\n");
}

module_init(kerneldrv_init);
module_exit(kerneldrv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AI适配");
MODULE_DESCRIPTION("Android kernel memory rw driver for Peace Elite cheat"); 