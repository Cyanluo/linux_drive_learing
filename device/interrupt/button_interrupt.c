#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/poll.h>


volatile unsigned long *gpfcon ;
volatile unsigned long *gpfdat ;
volatile unsigned long *gpgcon ;
volatile unsigned long *gpgdat;

struct pin_desc{
	unsigned int pin;
	unsigned int pin_val;
};

struct pin_desc desc_pin[4]={
	{S3C2410_GPF0, 0x01},
	{S3C2410_GPF2, 0x02},
	{S3C2410_GPG3, 0x03},
	{S3C2410_GPG11, 0x04},
};

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

/* 中断事件标志, 中断服务程序将它置1，second_drv_read将它清0 */
static volatile int ev_press = 0;

static struct class *second_drv_class;
static struct class_device	*second_drv_class_dev;
static unsigned char key_val;

static irqreturn_t button_interrupt(int irq, void *dev_id)
{
	unsigned int i;
	struct pin_desc *pbutton = (struct pin_desc *)dev_id;
	i = s3c2410_gpio_getpin(pbutton->pin);
	if(i)
	{
		key_val = 0x80 | pbutton->pin_val;
	}
	else
	{
		key_val = pbutton->pin_val;
	}

	ev_press = 1;                  /* 表示中断发生了 */
    wake_up_interruptible(&button_waitq);   /* 唤醒休眠的进程 */
	return IRQ_RETVAL(IRQ_HANDLED);
}


static int second_drv_open(struct inode *inode, struct file *file)
{
	request_irq(IRQ_EINT0, button_interrupt, IRQT_BOTHEDGE, "s1", &desc_pin[0]);
	request_irq(IRQ_EINT2, button_interrupt, IRQT_BOTHEDGE, "s2", &desc_pin[1]);
	request_irq(IRQ_EINT11, button_interrupt, IRQT_BOTHEDGE, "s3", &desc_pin[2]);
	request_irq(IRQ_EINT19, button_interrupt, IRQT_BOTHEDGE, "s4", &desc_pin[3]);
	return 0;
}
ssize_t
second_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	if(size != 1)
		return -EINVAL;

	wait_event_interruptible(button_waitq, ev_press);
	copy_to_user(buf, &key_val, 1);

	ev_press = 0;
	return 1;
}

int
button_release(struct inode *inode, struct file *file)
{
	free_irq(IRQ_EINT0,  &desc_pin[0]);
	free_irq(IRQ_EINT2,  &desc_pin[1]);
	free_irq(IRQ_EINT11, &desc_pin[2]);
	free_irq(IRQ_EINT19, &desc_pin[3]);
	return 0;
}

static struct file_operations second_drv_fops = {
    .owner   =  THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open    =  second_drv_open,     
	.read	 =	second_drv_read,	   
	.release =  button_release,
};

int major;
static int button_init(void)
{
	major = register_chrdev(0, "button", &second_drv_fops);
	
	second_drv_class = class_create(THIS_MODULE, "button");;
	second_drv_class_dev = class_device_create(second_drv_class, NULL, MKDEV(major, 0), NULL, "buttons");

	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon+1;
	
	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon+1;

	return 0;
}

void button_exit(void)
{
	unregister_chrdev(major, "button");
	class_device_unregister(second_drv_class_dev, MKDEV(major, 0));
	class_destroy(second_drv_class);	
	iounmap(gpfcon);
	iounmap(gpgcon);
}

module_init(button_init);
module_exit(button_exit);
MODULE_LICENSE("GPL");

