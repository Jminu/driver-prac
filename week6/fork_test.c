#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

char ppid_char[32];
char *const ps_argv[]= {
	"/bin/ps", // ps_argv[0]
	"-fx", // ps_argv[1]
	NULL, // ps_argv[2]
};

int run_ps() {
	int status;
	pid_t pid;

	printf("process list > \n");

	pid = fork();

	/*Parent process*/
	if (pid == 0) {
		execve(ps_argv[0], ps_argv, environ);
	}
	waitpid(pid, &status, 0);
}

int main() {
	int status;
	pid_t pid;
	pid = fork();

	if (pid == 0) {
		/*parent*/
		printf("[Child] pid-%d ppid-%d\n", getpid(), getppid());
		sleep(1);
		printf("[Child] exit\n");

		return 3;
	}
	else {
		/*child*/
		printf("[Parent] pid-%d\n", getpid());
		run_ps();

		sleep(2);
		run_ps();

		waitpid(pid, &status, 0);
		printf("[Parent] child exit : %d\n", WEXITSTATUS(status));

		run_ps();
		printf("[Parent] exit\n");
	}

	return 0;
}

