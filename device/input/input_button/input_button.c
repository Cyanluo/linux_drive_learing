
#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/gpio_keys.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

#include <asm/gpio.h>

struct timer_list button_timer;
struct input_dev *button_dev;

struct pin_desc{
	int irq;
	char *name;
	unsigned int pin;
	unsigned int pin_val;
};

struct pin_desc desc_pin[4]={
	{IRQ_EINT0,"s1",S3C2410_GPF0, KEY_L},
	{IRQ_EINT2,"s2",S3C2410_GPF2, KEY_S},
	{IRQ_EINT11,"s3",S3C2410_GPG3, KEY_ENTER},
	{IRQ_EINT19,"s4",S3C2410_GPG11, KEY_LEFTSHIFT},
};

struct pin_desc *irq_pd;
static irqreturn_t button_interrupt(int irq, void *dev_id)
{
	irq_pd = (struct pin_desc *)dev_id;
	mod_timer(&button_timer, jiffies + HZ/100);
	return IRQ_RETVAL(IRQ_HANDLED);
}

static void button_timer_irq(unsigned long h)
{
	int i;
	struct pin_desc *ptimer = irq_pd;
	if(!ptimer)
		return;
	i = s3c2410_gpio_getpin(ptimer->pin);

	if(i)
	{
		//最后一个参数0:松开 1:按下
		input_event(button_dev, EV_KEY, ptimer->pin_val, 0);
		input_sync(button_dev);
	}
	else
	{
		input_event(button_dev, EV_KEY, ptimer->pin_val, 1);
		input_sync(button_dev);
	}
}

static int __init buttons_init(void)
{
	int i;
	
	button_dev = input_allocate_device();
	//设置会发生哪类事件
	if(!button_dev)
	{
		return -ENOMEM;
	}
	set_bit(EV_KEY, button_dev->evbit);
	set_bit(EV_REP, button_dev->evbit);
	
	set_bit(KEY_L, button_dev->keybit);
	set_bit(KEY_S, button_dev->keybit);
	set_bit(KEY_ENTER, button_dev->keybit);
	set_bit(KEY_LEFTSHIFT, button_dev->keybit);
	//注册
	input_register_device(button_dev);
	//硬件操作
	//注册中断
	for(i=0; i<4; i++)
	{
		request_irq(desc_pin[i].irq, button_interrupt, IRQT_BOTHEDGE, desc_pin[i].name, &desc_pin[i]);
	}
	//设置定时器(用于消抖)
	init_timer(&button_timer);
	button_timer.function = button_timer_irq;
	add_timer(&button_timer);
	return 0;
}

static void __exit buttons_exit(void)
{
	int i;
	for(i=0; i<4; i++)
	{
		free_irq(desc_pin[i].irq, desc_pin[i].name);
	}
	del_timer(&button_timer);
	input_unregister_device(button_dev);
	input_free_device(button_dev);
}


module_init(buttons_init);
module_exit(buttons_exit);

MODULE_LICENSE("GPL");

