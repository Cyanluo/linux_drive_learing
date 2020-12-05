
#include"irq.h"
#include"s3c2440_soc.h"


irq_func irq_f[32];


void irq_register(int irq, irq_func fp)
{
	irq_f[irq] = fp;
	INTMSK &= ~(1<<irq);
}


void IRQ(void)
{
	unsigned int bit;
	bit = INTOFFSET;
	irq_f[bit](bit);
	//ÇåÁã
	INTPND |= 1<<bit;
	SRCPND |= 1<<bit;
}

