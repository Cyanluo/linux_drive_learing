#include"uart.h"
#include"s3c2440_soc.h"

void uart0_init()
{
	//设置GPH2,3 为uart功能
	GPHCON &= ~((0x3<<4) | (0x3<<6));
	GPHCON |= ((0x2<<4) | (0x2<<6));

	//设置上拉
	GPHUP &= ~((1<<2) | (1<<3));

	//设置时钟:PCLK 查询方式
	UCON0 = 0x5;

	//设置8n1，8数据位，无校验位，1个停止位
	ULCON0 = 0x3;

	//设置波特率:115200bps
	UBRDIV0 = 26;
}

int putchar(int c)
{
	while(!(UTRSTAT0 & (1<<2)));
	UTXH0 = (unsigned char)c;
}

int getchar(void)
{
	while(!(UTRSTAT0 & 1));
	return URXH0;
}

int puts(const char * s)
{
	while(*s)
	{
		putchar(*s);
		s++;
	}
}

