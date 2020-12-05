#include"irq.h"
#include"s3c2440_soc.h"
#include"timer.h"

timer_desc t_event[TEVENT];

void timer0_handle(int irq)
{
//	puts("timer0\r\n");
	int i;
	for(i = 0; i < TEVENT; i++)
	{
		if(t_event[i].fp)
		{
			t_event[i].fp();
		}
	}
}

void timer0_init(void)
{
   /*
	*设置时钟频率
	*Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
	*                     50000000 / (49 + 1) / 16 = 62500
	*                         16us 一次计数
	*/
	TCFG0 = 49; //prescaler value = 24
	TCFG1 &= ~0xf; 
	TCFG1 |= 3<<0; //divider value = 16
	TCNTB0 = 625; //设置计数值10ms中断一次
	/* 加载初值, 启动timer0 */
	TCON |= (1<<1);   /* Update from TCNTB0 & TCMPB0 */
    TCON |= 1<<3; //auto reload
    TCON &= ~(1<<1);
	TCON |= 1<<0; //启动定时器
	//注册中断函数
	irq_register(10, timer0_handle);
}

int tevent_register(char * name, ptimer fp)
{
	int i;
	for(i = 0; i < TEVENT; i++)
	{
//		puts("tevent_regsiter\r\n");
//		printHex(t_event[i].fp);
//		puts("\r\n");
		if(!t_event[i].fp)
		{
			t_event[i].fp = fp;
			t_event[i].name = name;
//			printHex(t_event[i].fp);
			return 0;
		}
	}
	return -1;
}
/*
void tevent_unregister(char *name)
{
	int i;
	for(i = 0; i<TEVENT ; i++)
	{
		if(!strcmp(t_event[i].name, name))
		{
			t_event[i].name = NULL;
			t_event[i].fp 	=  NULL;
		}
	}
}
*/

