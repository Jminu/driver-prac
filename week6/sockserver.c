#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/echo_socket"

int main()
{
	pid_t pid;
	int sockfd;
	int connfd;
	int read_bytes;
	char buf[256];
	struct sockaddr_un addr;

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		fprintf(stderr, "[Server] failed to make socket fd\n");
		return 0;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_NAME);
	unlink(addr.sun_path);

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)))
	{
		fprintf(stderr, "[Server] failed to bind\n");
		return 1;	
	}

	if (listen(sockfd, 0))
	{
		fprintf(stderr, "[Server] listen failed\n");
		return 2;
	};

	while (1)
	{
		connfd = accept(sockfd, NULL, NULL);
		if (connfd < 0)
		{
			fprintf(stderr, "[Server] failed to accept!\n");
			return 3;
		}

		printf("[Server] connect success!\n");
		pid = fork();
		if (pid == 0)
		{
			while (1)
			{
				read_bytes = read(connfd, buf, sizeof(buf));
				if (read_bytes <= 0)
				{
					break;
				}

				printf("[Server] resend to client\n");
				write(connfd, buf, sizeof(buf));
			}
			close(connfd);
		}
	}
	close(sockfd);
	return 4;
}
