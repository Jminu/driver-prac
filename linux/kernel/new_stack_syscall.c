#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <stdlib.h>

#define MAX_SIZE 256

static int top = -1;
static long long *sp = (long long*)malloc(sizeof(long long) * MAX_SIZE);

SYSCALL_DEFINE1(new_push_syscall, int, item)
{
	if(top == MAX_SIZE - 1) {
		return -1;
	}

	sp[++top] = item;
	return 0;
}

SYSCALL_DEFINE0(new_pop_syscall)
{
	if(top == 0) {
		return -1;
	}

	long long item = sp[top--];
	return item;	
}
