#include"uart.h"
#include"s3c2440_soc.h"
#include"led.h"
#include"init.h"

int main(void)
{
	unsigned char c;
	uart0_init();
	c = getchar();
	putchar(c);
	while(1)
	{
		if(c >='0' && c<= '7')
		{
			set_nor_tacc(c - '0');
		}
		else
		{
			puts("Please enter number 0-7\n");
			return 0;
		}
		led_init();
	}
	return 0;
}



