#include "linux/sched/signal.h"
#include "linux/types.h"
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/pid.h>
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,83))
#include <linux/sched/mm.h>
#endif
#define ARC_PATH_MAX 256

#include <linux/fs.h>    // For file and d_path
#include <linux/path.h>  // For struct path
#include <linux/dcache.h>// For d_path
#ifndef ARC_PATH_MAX
#define ARC_PATH_MAX PATH_MAX
#endif

// 函数原型声明
pid_t get_process_pid(char *comm);
size_t get_module_base(pid_t pid, const char* name);