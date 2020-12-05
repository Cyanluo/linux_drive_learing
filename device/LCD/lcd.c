#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include <asm/mach/map.h>
#include <asm/arch/regs-lcd.h>
#include <asm/arch/regs-gpio.h>
#include <asm/arch/fb.h>

static struct fb_info *s3c_lcd;
static volatile unsigned long *gpgcon;
static volatile unsigned long *gpbcon;
static volatile unsigned long *gpbdat;
static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;

struct lcd_reg{
	unsigned long	lcdcon1;
	unsigned long	lcdcon2;
	unsigned long	lcdcon3;
	unsigned long	lcdcon4;
	unsigned long	lcdcon5;
    unsigned long	lcdsaddr1;
    unsigned long	lcdsaddr2;
    unsigned long	lcdsaddr3;
    unsigned long	redlut;
    unsigned long	greenlut;
    unsigned long	bluelut;
    unsigned long	reserved[9];
    unsigned long	dithmode;
    unsigned long	tpal;
    unsigned long	lcdintpnd;
    unsigned long	lcdsrcpnd;
    unsigned long	lcdintmsk;
    unsigned long	lpcsel;
};

static volatile struct lcd_reg *lcd_reg;
static u32 pallet[16];
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}
static int s3cfb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	unsigned int val;
	if(regno>16)
		return 1;
			val  = chan_to_field(red,   &info->var.red);
			val |= chan_to_field(green, &info->var.green);
			val |= chan_to_field(blue,  &info->var.blue);

			pallet[regno] = val;
	return 0;
}

static struct fb_ops s3cfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= s3cfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

static int  lcd_init(void)
{
	s3c_lcd = framebuffer_alloc(0, NULL);
	//设置
	//fix:固定参数  fb.h
	strcpy(s3c_lcd->fix.id , "mylcd");
	s3c_lcd->fix.smem_len = 480*272*16/8;
	s3c_lcd->fix.type = FB_TYPE_PACKED_PIXELS;
	s3c_lcd->fix.visual = FB_VISUAL_TRUECOLOR;//真彩
	s3c_lcd->fix.line_length = 480*2;

	//var:可变参数
	s3c_lcd->var.xres = 480;
	s3c_lcd->var.yres = 272
	s3c_lcd->var.xres_virtual= 480;
	s3c_lcd->var.yres_virtual= 272;
	s3c_lcd->var.bits_per_pixel = 16;
	//RGB:565
	s3c_lcd->var.red.length = 5;
	s3c_lcd->var.red.offset = 11;

	s3c_lcd->var.green.length = 6;
	s3c_lcd->var.green.offset = 5;

	s3c_lcd->var.blue.length = 5;
	s3c_lcd->var.blue.offset =	0;

	s3c_lcd->var.activate =FB_ACTIVATE_NOW; // 16

	s3c_lcd->fbops = &s3cfb_ops;
	//s3c_lcd->screen_base = ;显存的虚拟地址
	s3c_lcd->screen_size = 480*272*16/8;
	s3c_lcd->pseudo_palette = pallet;

	//硬件设置
	//GPC:数据  GPD:数据  GPG4:LCD_PWREN  GPGB0:KEYBOARD 背光控制 
	gpccon = ioremap(0x56000020, 4);
	gpgcon = ioremap(0x56000060, 4);
	gpdcon = ioremap(0x56000030, 4);
	gpbcon = ioremap(0x56000010, 8);
	gpbdat = gpbcon + 1;

	//配置为LCD功能
	*gpccon = 0xaaaaaaaa;  
	*gpdcon = 0xaaaaaaaa;
	//关闭背光GPB0
	*gpbcon &= ~(3);//配置为输出引脚
	*gpbcon |= 1;
	*gpbdat &= ~(1);
	//配置GPG4:LCD_PWREN
	*gpgcon |= 3<<(4*2);

	//配置各寄存器
	lcd_reg = ioremap(0X4D000000, sizeof(struct lcd_reg));

	//16bpp
	lcd_reg->lcdcon1 = 4<<8 | 3<<5 | 12<<1;
	lcd_reg->lcdcon2 = 1<<24 | 271<<14 | 1<<6 | 9<<0;
	lcd_reg->lcdcon3 = 1<<19 | 479<<8 | 1<<0;
	lcd_reg->lcdcon4 = 40;
	lcd_reg->lcdcon5 = (1<<11) | (1<<9) | (1<<8) | (1<<0);
	// 以下函数会分配s3c_lcd->fix.smem_start
	s3c_lcd->screen_base = dma_alloc_writecombine(NULL, s3c_lcd->fix.smem_len, &s3c_lcd->fix.smem_start, GFP_KERNEL);
	//去掉起始地址的最高和最低位存放在寄存器的低30位
	lcd_reg->lcdsaddr1 = (s3c_lcd->fix.smem_start >> 1) & ~(3<<30);
	//结束地址的[1:21]放到寄存器的[20:0]
	lcd_reg->lcdsaddr2 = ((s3c_lcd->fix.smem_start + s3c_lcd->fix.smem_len) >> 1) & 0x1fffff;
	//一行的长度单位两个字节
	lcd_reg->lcdsaddr3 = 480*16/16;

	//使能LCD
	lcd_reg->lcdcon1 |= 1;
	lcd_reg->lcdcon5 |= 1<<3;
	*gpbdat |= 1;//使能背光

	//注册
	register_framebuffer(s3c_lcd);
	return 0;
}

static void __exit lcd_exit(void)
{
	unregister_framebuffer(s3c_lcd);
	lcd_reg->lcdcon1 &= ~(1<<0);
	lcd_reg->lcdcon5 &= ~(1<<3);
	*gpbdat &= ~(1<<0);
	dma_free_writecombine(NULL, s3c_lcd->fix.smem_len, s3c_lcd->screen_base, s3c_lcd->fix.smem_start);
	iounmap(gpccon);
	iounmap(gpgcon);
	iounmap(gpdcon);
	iounmap(gpbcon);
	iounmap(lcd_reg);
	framebuffer_release(s3c_lcd);

}

module_init(lcd_init);
module_exit(lcd_exit);

MODULE_LICENSE("GPL");

