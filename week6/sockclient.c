#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/echo_socket"

int main()
{
	int sockfd;
	char buf[256];
	char str[256];
	struct sockaddr_un addr;
	int recv_bytes = 0;

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		fprintf(stderr, "[Client] failed to make socket\n");
		return 0;
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_NAME);

	if (connect(sockfd,(struct sockaddr*)&addr, sizeof(addr)))
	{
		fprintf(stderr, "[Client] failed to connect!\n");
		return 1;
	}

	while (1)
	{
		printf("[Client] Input: ");
		scanf("%s", str);

		write(sockfd, str, sizeof(str));
		recv_bytes = read(sockfd, buf, sizeof(buf));
		if (recv_bytes <= 0)
		{
			printf("[Client] Sever is shutdown!\n");
			break;
		}
		
		printf("[Client] return message: %s\n", buf);
	}

	close(sockfd);
	printf("[Client] client is exit!\n");

	return 0;
}
