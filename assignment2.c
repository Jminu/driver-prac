#include <stdio.h>
#include <unistd.h>
#define __NR_set_array 454
#define __NR_get_array 455

int main()
{
	int var = 10;
	int idx = 5;

	syscall(__NR_set_array, idx, var);
	printf("set item %d to %d index\n", var, idx);

	int item = syscall(__NR_get_array, idx);
	printf("get item : %d\n", item);

	return 0;
}
