#include"uart.h"
#include"s3c2440_soc.h"
#include"led.h"
#include"init.h"

unsigned char test_char1;
unsigned char test_char2=0;
unsigned char test_char3='a';
unsigned char test_char4='A';
const int test_char5=1;


int main(void)
{
	char a = 'd';
	unsigned char c;
	while(1)
	{

		printf("aa%c", a);
		putchar(test_char3);
		test_char3++;
		delay(1000);
		putchar(test_char4);
		test_char4++;
		delay(1000);
	}
	return 0;
}



