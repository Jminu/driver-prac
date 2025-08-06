#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t pid;
	int pipe_p_to_c[2];
	int pipe_c_to_p[2];
	char buf[256];

	pipe(pipe_p_to_c);
	pipe(pipe_c_to_p);

	pid = fork();
	if (pid == 0) //child
	{
		close(pipe_p_to_c[1]);
		close(pipe_c_to_p[0]);

		const char *msg = "I'm child";
		write(pipe_c_to_p[1], msg, strlen(msg) + 1);
		printf("[Child]: send msg %s\n", msg);
		sleep(1);

		read(pipe_p_to_c[0], buf, 256);
		printf("[Child]: receive msg %s\n", buf);
		sleep(1);

		close(pipe_p_to_c[0]);
		close(pipe_c_to_p[1]);
	}
	else //parent
	{
		close(pipe_p_to_c[0]);
		close(pipe_c_to_p[1]);

		const char *msg = "I'm parent";
		write(pipe_p_to_c[1], msg, strlen(msg) + 1);
		printf("[Parent]: send msg : %s\n", msg);
		sleep(1);
		
		read(pipe_c_to_p[0], buf, 256);
		printf("[Parent]: receive msg : %s\n", buf);
		sleep(1);

		close(pipe_p_to_c[1]);
		close(pipe_c_to_p[0]);

		waitpid(pid, NULL, 0);
		printf("[Parent]: child is exit\n");
	}
	
	return 0;
}

