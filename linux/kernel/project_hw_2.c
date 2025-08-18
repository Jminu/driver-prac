#include <linux/kernel.h>
#include <linux/syscalls.h>
#define MAX_SIZE 64

static int array[MAX_SIZE]; // 256bytes

SYSCALL_DEFINE2(set_array, int, idx, int, param)
{
	printk(KERN_INFO, "set [%d] into index [%d]\n", param, idx);
	array[idx] = param;

	return idx;
}

SYSCALL_DEFINE1(get_array, int, idx)
{
	printk(KERN_INFO, "get value from index [%d]", idx);
	int item;
	item = array[idx];

	return item;
}
