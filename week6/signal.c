#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sigint_handler(int signum)
{
	printf("Ctrl + C pressed!\n");
	exit(0);
}

void sigusr1_handler(int signum)
{
	printf("SIGUSR1 occured!\n");
}

int main()
{
	pid_t pid;
	pid = fork();
	if (pid == 0) { // child
		kill(getppid(), SIGSTOP);
		sleep(1);

		kill(getpid(), SIGCONT);
		sleep(1);
	}
	else { // parent
		if (signal(SIGINT, sigint_handler) < 0) {
			fprintf(stderr, "failed to register handler\n");
			return 1;
		}
		
		if (signal(SIGUSR1, sigusr1_handler) < 0) {
			fprintf(stderr, "failed to register handler\n");
			return 1;
		}

		wait(NULL);
		while (1) {
			sleep(1);
		}
	}
	return 0;
}
