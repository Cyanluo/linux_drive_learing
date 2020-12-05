
#include"led.h"
#include "s3c2440_soc.h"

void led_init(void)
{
	//…Ë÷√GPF4°¢5°¢6
	GPFCON &= ~((3<<(4*2)) | (3<<(5*2)) | (3<<(6*2)));
	GPFCON |= (1<<(4*2)) | (1<<(5*2)) | (1<<(6*2));
/*	GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
	delay(1000);*/
	GPFDAT |= (1<<4) | (1<<5) | (1<<6);
//	delay(1000);
}

void delay(int z)
{
	int x, y;
	for(x=z; x>0; x--)
		for(y=200;y>0;y--);
}


