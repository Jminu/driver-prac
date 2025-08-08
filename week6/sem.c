#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SEM_NAME "minu semaphore"

int main(int argc, char *argv[])
{
	int *ptr;
	int sem_init = 1;
	pid_t pid;
	sem_t *sem;

	if (argc == 2 && !strcmp(argv[1], "-no-sem")) {
		sem_init = 2;
	}

	sem_unlink(SEM_NAME);
	sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, sem_init);
	if (sem == SEM_FAILED) {
		fprintf(stderr, "Failed to open semaphore\n");
		return 0;
	}

	ptr = mmap(NULL, sizeof(*ptr), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "Failed to mapping\n");
		return 1;
	}

	pid = fork();
	if (pid == 0) {
		for (int i = 0; i < 10000; i++) {
			sem_wait(sem);
			(*ptr)++;
			sem_post(sem);
			
			if (i % 1000 == 0) {
				printf("[Child] %d\n", *ptr);
			}
		}
	}
	else {
		for (int i = 0; i < 10000; i++) {
			sem_wait(sem);
			(*ptr)--;
			sem_post(sem);

			if (i % 1000 == 0) {
				printf("[Parent] %d\n", *ptr);
			}
		}

		wait(NULL);
		printf("Final value: %d\n", *ptr);

	}

	return 0;
}
