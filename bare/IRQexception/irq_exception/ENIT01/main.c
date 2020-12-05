#include"uart.h"
#include"s3c2440_soc.h"
#include"led.h"
#include"init.h"
#include"eint.h"


int main(void)
{
	unsigned char c;
	led_init();
	key_init();
	EINT_init();
	while(1)
	{
		
	}
	return 0;
}



