#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,83))
#include <linux/sched/mm.h>
#endif
#include <asm/cpu.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>

// 函数原型声明
phys_addr_t translate_linear_address(struct mm_struct* mm, uintptr_t va);
size_t read_physical_address(phys_addr_t pa, void* buffer, size_t size);
size_t write_physical_address(phys_addr_t pa, void* buffer, size_t size);
bool read_process_memory(pid_t pid, uintptr_t addr, void* buffer, size_t size);
bool write_process_memory(pid_t pid, uintptr_t addr, void* buffer, size_t size);

#ifdef ARCH_HAS_VALID_PHYS_ADDR_RANGE
static size_t get_high_memory(void)
{
	struct sysinfo meminfo;
	si_meminfo(&meminfo);
	return (meminfo.totalram * (meminfo.mem_unit / 1024)) << PAGE_SHIFT;
}
#define valid_phys_addr_range(addr, count) (addr + count <= get_high_memory())
#else
#define valid_phys_addr_range(addr, count) true
#endif
