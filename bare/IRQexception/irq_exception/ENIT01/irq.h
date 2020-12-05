
#ifndef _IRQ_H_
#define _IRQ_H_

typedef void(*irq_func)(int);
void irq_register(int irq, irq_func fp);
extern irq_func irq_f[32];

#endif