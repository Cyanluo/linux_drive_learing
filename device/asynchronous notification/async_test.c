#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>

int fd;

void async_handle(int signum)
{
	unsigned char key_val;
	read(fd, &key_val, 1);
	printf("key_val=0x%x\n", key_val);
}

int main(int argc, char **argv)
{
	int Oflag;

	signal(SIGIO, async_handle);
	fd = open("/dev/buttons", O_RDWR);
	if(fd < 0)
	{
		printf("can't open file...\n");
	}

	fcntl(fd, F_SETOWN, getpid());
	Oflag = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, Oflag | FASYNC);
	
	while(1)
	{
		sleep(10000);		
	}
	return 0;
}

