#include"uart.h"
#include"s3c2440_soc.h"

int main(void)
{
	unsigned char c;
	uart0_init();
	puts("Hellow World\r\n");
	while(1)
	{
		c = getchar();
		if(c == '\n')
		{
			putchar('\r');
		}
		if(c == '\r')
		{
			putchar('\n');
		}
		putchar(c);
	}
	return 0;
}



