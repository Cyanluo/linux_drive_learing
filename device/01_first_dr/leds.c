#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

volatile unsigned long *base_bufaddr;
static struct class *ledsdrv_class;
static struct class_devices *ledsdrv_class_devs[4];

#define DRV_NAME "leds"
#define addr_calc(x) (((x) - 0x56000000)/4)
#define GPFCON (*(volatile unsigned long *)(base_bufaddr + addr_calc(0x56000050)))
#define GPFDAT (*(volatile unsigned long *)(base_bufaddr + addr_calc(0x56000054)))



static int leds_drv_open(struct inode *inode, struct file *file)
{
	int minor;
	//获取副设备号
	minor = MINOR(inode->i_rdev);
	switch(minor)
	{
		case 0:
			{
				//led 1 2 3 输出
				//printk("ce shi dev leds open\n");
				GPFCON &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
				GPFCON |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
				break;
			}
		case 1:
			{
				//led 1 输出
				GPFCON &= ~(0x3<<(4*2));
				GPFCON |= (0x1<<(4*2));
				break;
			}
		case 2:
			{
				//led 2 输出
				GPFCON &= ~(0x3<<(5*2));
				GPFCON |= (0x1<<(5*2));
				break;
			}
		case 3:
			{
				//led 3 输出
				GPFCON &= ~(0x3<<(5*3));
				GPFCON |= (0x1<<(5*3));
				break;
			}
	}
	return 0;
}

static ssize_t leds_drv_write(struct file *file, const char __user *buf, size_t count, loff_t ppos)
{
	int minor;
	int val;
	minor = MINOR(file->f_dentry->d_inode->i_rdev);
	copy_from_user(&val, buf, count); 
	switch(minor)
	{
		case 0:
		{
			if(val == 1)
			{
				//printk("ce shi dev leds write\n");
				//开灯
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				//关灯
				GPFDAT |= (1<<4) | (1<<5) | (1<<6);
			}					
			break;
		}
		case 1:
		{
			if(val == 1)
			{
				//开灯
				GPFDAT &= ~(1<<4);
			}
			else
			{
				//关灯
				GPFDAT |= (1<<4);
			}					
			break;
		}
		case 2:
		{
			if(val == 1)
			{
				//开灯
				GPFDAT &= ~(1<<5);
			}
			else
			{
				//关灯
				GPFDAT |= (1<<5);
			}					
			break;
		}
		case 3:
		{
			if(val == 1)
			{
				//开灯
				GPFDAT &= ~(1<<6);
			}
			else
			{
				//关灯
				GPFDAT |=  (1<<6);
			}					
			break;	
		}
	}
	return 0;
}

static struct file_operations leds_drv_fops ={
	.open  = leds_drv_open, 
	.write = leds_drv_write,
	.owner = THIS_MODULE,
};

int major;
int leds_drv_init(void)
{
	int minor = 0;
	//注册
	major = register_chrdev(0, DRV_NAME, &leds_drv_fops);

	ledsdrv_class = class_create(THIS_MODULE, "ledsdrv");
	ledsdrv_class_devs[0] = class_device_create(ledsdrv_class, NULL, MKDEV(major, minor), NULL, "leds");
	
	for(minor = 1 ; minor<4 ; minor++)
	{
		ledsdrv_class_devs[minor] = class_device_create(ledsdrv_class, NULL, MKDEV(major, minor), NULL, "led%d", minor);	
	}

	base_bufaddr = (volatile unsigned long *)ioremap(0x56000000, 0x100000);
	//printk("base_bufaddr=%ld\n", *base_bufaddr);
	
	
	return 0;
}

void leds_drv_exit(void)
{
	int minor;
	unregister_chrdev(major, DRV_NAME);
	for(minor = 0; minor<4; minor++)
	{
		class_device_unregister(ledsdrv_class_devs[minor]);
	}
	class_destroy(ledsdrv_class);
	iounmap(base_bufaddr);
}

module_init(leds_drv_init);
module_exit(leds_drv_exit);
MODULE_LICENSE("GPL");

