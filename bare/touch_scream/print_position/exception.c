
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

void swi_deal(volatile unsigned int *p)
{
	unsigned int val;
	val = *p & ~0xff000000;
	puts("swi expection occur\r\n");
	puts("the val=");
	printHex(val);
	puts("\r\n");
}


