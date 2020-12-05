#include"uart.h"
#include"s3c2440_soc.h"

void uart0_init()
{
	//����GPH2,3 Ϊuart����
	GPHCON &= ~((0x3<<4) | (0x3<<6));
	GPHCON |= ((0x2<<4) | (0x2<<6));

	//��������
	GPHUP &= ~((1<<2) | (1<<3));

	//����ʱ��:PCLK ��ѯ��ʽ
	UCON0 = 0x5;

	//����8n1��8����λ����У��λ��1��ֹͣλ
	ULCON0 = 0x3;

	//���ò�����:115200bps
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

void printHex(int val)
{
	unsigned char dat[8];
	int i;
	for(i=0;i<8;i++)
	{
		dat[i] = val & 0xf;
		val >>= 4;
	}
	puts("0X");
	for(i=7;i>=0;i--)
	{
		if(dat[i]>=0 &&dat[i]<=9)
		{
			putchar(dat[i] + '0');
		}
		else if(dat[i]>=0xa && dat[i]<=0xf)
		{
			putchar(dat[i]-0xa + 'A');
		}
	}
}

void print1(void)
{
	puts("11111\r\n");
}

void print2(void)
{
	puts("2222\r\n");
}


