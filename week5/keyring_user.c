#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl>
#include <linux/limits.h>

#define CTL_DEVICE_NAME "keyringctl"
#define IOCTL_MAGIC 'C'
#define IOCTL_ADD _IOW(IOCTL_MAGIC, 0, int)
#define IOCTL_DEL _IOW(IOCTL_MAGIC, 1, int)

int main(int argc, char *argv[]) {
	unsigned long req;
	int fd;
	int ret;

	if (argc != 3) {
		fprintf(stderr, "argument Error\n");
		exit(0);
	}

	if (!strcmp(argv[1], "add")) {
		req = IOCTL_ADD;
	}
	else if(!strcmp(argv[1], "del")) {
		req = IOCTL_DEL;
	}
	else {
		fprintf(stderr, "Usage err!\n");
		exit(0);
	}

	fd = open("/dev/", CTL_DEVICE_NAME, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "file open error!\n");
		exit(0);
	}

	ret = ioctl(fd, req, atoi(argv[2]));
	if (ret < 0) {
		printf("failed to ioctl command! : %d\n", ret);
		exit(0);
	}

	return  0;
}
