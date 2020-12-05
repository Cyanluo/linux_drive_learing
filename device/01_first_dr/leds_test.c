#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

//file <dev> <on|off>
//<dev> /dev/led1 | 2 | 3
//file ./ledsdrv
//eg
//./ledsdrv /dev/led1 on

void usage(void)
{
	printf("usage\n");
	printf("file <dev> <on|off>\n");
	printf("<dev> /dev/led1 | 2 | 3\n");
	printf("file ./ledsdrv\n");
	printf("eg\n");
	printf("./ledsdrv /dev/led1 on\n");
}

int main(int argc, char **argv)
{
	int fd,val = 1;
	char *filename;
	filename = argv[1];
	fd = open(filename, O_RDWR);
	if(fd<0)
	{
		printf("can't open %s\n", filename);
		return 0;
	}
	if(argc != 3)
	{
		usage();
		return 0;
	}
	if(!strcmp(argv[2], "on"))
	{
		//printf("ce shi on\n");
		val = 1;
	}
	else if(!strcmp(argv[2], "off"))
	{
		val = 0;
	}
	else
	{
		usage();
		return 0;
	}
	write(fd, &val, 4);
	return 0;
}



