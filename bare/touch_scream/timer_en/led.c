
#include"led.h"
#include "s3c2440_soc.h"

void led_timer0(void)
{
	static int count = 0;
	static int led_fg = 0;
	int temp;
	count++;
	if(count<50)
	{
		return;
	}
	count = 0;

	led_fg++;
	if(led_fg>7)
		led_fg = 0;
	temp = ~led_fg;
	temp &= 7;
	GPFDAT &= ~(7<<4);
	GPFDAT |= temp<<4;
}

void led_init(void)
{
	//ÉèÖÃGPF4¡¢5¡¢6
	GPFCON &= ~((3<<(4*2)) | (3<<(5*2)) | (3<<(6*2)));
	GPFCON |= (1<<(4*2)) | (1<<(5*2)) | (1<<(6*2));
/*	GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
	delay(1000);*/
	GPFDAT |= (1<<4) | (1<<5) | (1<<6);
//	delay(1000);
	tevent_register("led", led_timer0);
}

void delay(int z)
{
	int x, y;
	for(x=z; x>0; x--)
		for(y=200;y>0;y--);
}




