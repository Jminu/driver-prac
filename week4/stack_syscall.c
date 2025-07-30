#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_new_push_syscall 452
#define __NR_new_pop_syscall 453

int main(int argc, char *argv[]) {
	long long data;
	long long result;

	if(argc == 3 && strcmp(argv[1], "push") == 0) {
		data = atoi(argv[2]);
		result = syscall(__NR_new_push_syscall, data);

		if(result == -1) {
			perror("max stack\n");
			exit(1);
		}
	}
	else if(argc == 2 && strcmp(argv[1], "pop") == 0) {
		result = syscall(__NR_new_pop_syscall);

		if(result != -1) {
			printf("[popped item] : %ll \n", result);
			perror("empty stack\n");
			exit(1);
		}
		else {
			perror("empty stack\n");
			exit(1);
		}
	}
	else {
		perror("input Error\n");
		exit(1);
	}

	return 0;
}

