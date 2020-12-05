#include"uart.h"
#include"s3c2440_soc.h"
#include"led.h"
#include"init.h"
#include"eint.h"
#include"timer.h"


int main(void)
{
	unsigned char c;
	int i;
	for(i=0; i<TEVENT; i++)
	{
		t_event[i].fp= NULL;
	}
	puts("hou\r\n");
	led_init();
	timer0_init();	
	key_init();
	EINT_init();
	touchscreen_test();
	while(1);
	return 0;
}



