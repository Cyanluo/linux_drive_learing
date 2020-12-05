
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

int main(int argc, char **argv)
{
	int fd, ret;
	struct pollfd poll_drv[1];
	unsigned char key_val;

	fd = open("/dev/buttons", O_RDWR);
	if(fd < 0)
	{
		printf("can't open file...\n");
	}
	poll_drv[0].fd = fd;
	poll_drv[0].events = POLLIN;
	while(1)
	{
		ret = poll(poll_drv, 1, 500);
		if(ret)
		{
			read(fd, &key_val, 1);
			printf("irq=0x%x\n", key_val);
		}
		else
		{
			printf("time out ....\n");
		}
	}
	return 0;
}

