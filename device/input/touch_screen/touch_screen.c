#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <asm/plat-s3c24xx/ts.h>

#include <asm/arch/regs-adc.h>
#include <asm/arch/regs-gpio.h>

struct s3c24x0_adc_io{
	unsigned long adccon;
	unsigned long adctsc;
	unsigned long adcdly;
	unsigned long adcdat0;
	unsigned long adcdat1;
	unsigned long adcupdn;
};

static volatile struct s3c24x0_adc_io *adc_io;
static struct input_dev *ts_dev;
struct timer_list timer;
int x[4], y[4];

void enter_wait_pen_down(void)
{
	adc_io->adctsc = 0xd3;
}

void enter_wait_pen_up(void)
{
	adc_io->adctsc = 0x1d3;
}

void enter_auto_measure(void)
{
	adc_io->adctsc = 0x4;
}

int ts_filter(int x[], int y[])
{
#define FILTER_ERR 10
	int avr_x, avr_y, det_x, det_y;
	avr_x = (x[0] + x[1])/2;
	avr_y = (y[0] + y[1])/2;
	det_x = (x[2] > avr_x) ? (x[2] - avr_x) : (avr_x - x[2]);
	det_y = (y[2] > avr_y) ? (y[2] - avr_y) : (avr_y - y[2]);
	if(det_x >= FILTER_ERR || det_y >= FILTER_ERR)
		return 0;
	avr_x = (x[1] + x[2])/2;
	avr_y = (y[1] + y[2])/2;
	det_x = (x[3] > avr_x) ? (x[3] - avr_x) : (avr_x - x[3]);
	det_y = (y[3] > avr_y) ? (y[3] - avr_y) : (avr_y - y[3]);
	if(det_x >= FILTER_ERR || det_y >= FILTER_ERR)
		return 0;

	return 1;
}

static void ts_timer_irq(unsigned long p)
{
	if(adc_io->adcdat0 & (1<<15))
	{
		input_report_abs(ts_dev, ABS_PRESSURE, 0);
 		input_report_key(ts_dev, BTN_TOUCH, 0);
		input_sync(ts_dev);
		enter_wait_pen_down();
	}
	else 
	{
		enter_auto_measure();
		//启动ADC
		adc_io->adccon |= 1<<0;
	}
}

static irqreturn_t tc_irq(int irq, void *dev_id)
{
	//松开
	if(adc_io->adcdat0 & (1<<15))
	{
//		printk("pen_up\n");
		input_report_abs(ts_dev, ABS_PRESSURE, 0);
 		input_report_key(ts_dev, BTN_TOUCH, 0);
		input_sync(ts_dev);
		enter_wait_pen_down();
	}
	//按下
	else 
	{
//		printk("pen_down\n");
		enter_auto_measure();
		//启动ADC
		adc_io->adccon |= 1<<0;
	}
	return IRQ_HANDLED;
}

static irqreturn_t adc_irq(int irq, void *dev_id)
{
	static int count = 0;
	int i;
	int x1 = adc_io->adcdat0;
	int y1 = adc_io->adcdat1;
	//松开
	if(adc_io->adcdat0 & (1<<15))
	{
		input_report_abs(ts_dev, ABS_PRESSURE, 0);
 		input_report_key(ts_dev, BTN_TOUCH, 0);
		input_sync(ts_dev);
 		
		enter_wait_pen_down();
	}
	//按下
	else 
	{
		x[count] = x1 & 0x3ff;
		y[count] = y1 & 0x3ff;
		count++;
		//优化:计算四次取平均值
		if(count>=4)
		{
			count = 0;
			x1 = 0;
			y1 = 0;
			if(ts_filter(x, y))
			{
				for(i=0; i<4; i++)
				{
					x1+=x[i];
					y1+=y[i]; 
				}
				//printk("x=%d, y=%d \n", x1/4, y1/4);
				input_report_abs(ts_dev, ABS_X, x1/4);
 				input_report_abs(ts_dev, ABS_Y, y1/4);
				input_report_abs(ts_dev, ABS_PRESSURE, 1);
 				input_report_key(ts_dev, BTN_TOUCH, 1);			
 				input_sync(ts_dev);
			}
			enter_wait_pen_up();
			mod_timer(&timer, jiffies + HZ/100);
		}
		else
		{
			enter_auto_measure();
			//启动ADC
			adc_io->adccon |= 1<<0;
		}
	}	
	return IRQ_HANDLED;
}
static int  ts_init(void)
{
	struct clk *adc_clk;
	//分配input_dev
	ts_dev = input_allocate_device();
	if (!ts_dev) {
		printk(KERN_ERR "Unable to allocate the input device !!\n");
		return -ENOMEM;
	}
	//设置
	//按键事件
	set_bit(EV_KEY, ts_dev->evbit);
	//绝对位移事件
	set_bit(EV_ABS, ts_dev->evbit);
	set_bit(BTN_TOUCH, ts_dev->keybit);
	input_set_abs_params(ts_dev, ABS_X, 0, 0x3FF, 0, 0);
	input_set_abs_params(ts_dev, ABS_Y, 0, 0x3FF, 0, 0);
	input_set_abs_params(ts_dev, ABS_PRESSURE, 0, 1, 0, 0);
	//注册
	input_register_device(ts_dev);

	//硬件操作
	//使能时钟
	adc_clk = clk_get(NULL, "adc");
	if (!adc_clk) {
		printk(KERN_ERR "failed to get adc clock source\n");
		return -ENOENT;
	}
	clk_enable(adc_clk);
	//地址映射
	adc_io = ioremap(0x58000000, sizeof(struct s3c24x0_adc_io));
	//初始化ADC
	adc_io->adccon = 1<<14 | 49<<6;
	adc_io->adcdly = 60000;
	//注册中断
	//触摸中断
	if(request_irq(IRQ_TC, tc_irq, IRQF_SAMPLE_RANDOM, "s3c_tc_irq", NULL))
	{
		printk(KERN_ERR "s3c2410_ts.c: Could not allocate ts IRQ_TC !\n");
		return -EIO;
	}
	//adc中断
	if (request_irq(IRQ_ADC, adc_irq, IRQF_SAMPLE_RANDOM ,
		"s3c_adc_irq", NULL)) {
		printk(KERN_ERR "s3c2410_ts.c: Could not allocate ts IRQ_ADC !\n");
		return -EIO;
	}
	//定时器
	init_timer(&timer);
	timer.function = ts_timer_irq;
	add_timer(&timer);
	//进入等待中断状态ADCTSC=0xd3;
	enter_wait_pen_down();
	return 0;
}

static void __exit ts_exit(void)
{
	free_irq(IRQ_TC, NULL);
	free_irq(IRQ_ADC, NULL);
	iounmap(adc_io);
	input_unregister_device(ts_dev);
	input_free_device(ts_dev);
	del_timer(&timer);
}


module_init(ts_init);
module_exit(ts_exit);

MODULE_LICENSE("GPL");



