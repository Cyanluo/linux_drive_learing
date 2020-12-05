
#include"exception.h"
#include"uart.h"

void und_deal(unsigned int cpsr, char *s)
{
	putchar(3);
	putchar('3');
	puts("\n\r");
	printHex(cpsr);
	puts("\n\r");
	puts(s);
}



