
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;
	unsigned char key_val;
	printf("test");
	fd = open("/dev/buttons", O_RDWR);
	if(fd < 0)
	{
		printf("can't open file...\n");
	}
	while(1)
	{
		read(fd, &key_val, 1);
		printf("irq=0x%x\n", key_val);
	}
	return 0;
}

