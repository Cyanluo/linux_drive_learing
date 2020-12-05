#include"touch_lcd.h"
#include"s3c2440_soc.h"
#include"irq.h"

/* ADCTSC's bits */
#define WAIT_PEN_DOWN    (0<<8)
#define WAIT_PEN_UP      (1<<8)

#define YM_ENABLE        (1<<7)
#define YM_DISABLE       (0<<7)

#define YP_ENABLE        (0<<6)
#define YP_DISABLE       (1<<6)

#define XM_ENABLE        (1<<5)
#define XM_DISABLE       (0<<5)

#define XP_ENABLE        (0<<4)
#define XP_DISABLE       (1<<4)

#define PULLUP_ENABLE    (0<<3)
#define PULLUP_DISABLE   (1<<3)

#define AUTO_PST         (1<<2)

#define WAIT_INT_MODE    (3)
#define NO_OPR_MODE      (0)

static volatile int ts_flg = 0; 

void enter_wait_pen_down_mode(void)
{
	ADCTSC = WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

void enter_wait_pen_up_mode(void)
{
	ADCTSC = WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

void enter_auto_measure_mode(void)
{
	ADCTSC = AUTO_PST | NO_OPR_MODE;
}

static void enable_ts_timer(void)
{
	ts_flg = 1;
}

static void disable_ts_timer(void)
{
	ts_flg = 0;
}

static int get_ts_timer_status(void)
{
	return ts_flg;
}

void adc_ts_init(void)
{
	ADCCON = (1<<14) | (49<<6);
	/*  ���´�����, ��ʱһ���ٷ���TC�ж�
	 *  ��ʱʱ�� = ADCDLY * �������� = ADCDLY * 1 / 12000000 = 5ms
	 */
	ADCDLY = 60000;	
}

void ts_irq_init(void)
{
	INTSUBMSK &= ~((1<<9) | (1<<10));
	SUBSRCPND = (1<<9) | (1<<10);

	//ע���жϷ�����
	irq_register(31, touchscreen_handle);	
}

void ts_timer_irq(void)
{
	if(!get_ts_timer_status())
	{
		return;
	}
	//�ɿ�
	if(ADCDAT0 & (1<<15))
	{
		disable_ts_timer();
		enter_wait_pen_down_mode();
		return;
	}
	else
	{
		enter_auto_measure_mode();
		//����adcת��
		ADCCON |= 1<<0;
	}
}

void touchscreen_init(void)
{
//	puts("touchscreen_init\n\r");
	//printf("touchscreen_init");
	//ʹ��adc
	adc_ts_init();
	//ʹ���ж�
	ts_irq_init();
	//ע�ᶨʱ��0������
	tevent_register("touchscreen", ts_timer_irq);
	//�ȴ�����
	enter_wait_pen_down_mode();

}

void ts_irq_tc(void)
{
	//�ɿ�
//	puts("ts_irq_tc\n\r");
	if(ADCDAT0 & (1<<15))
	{
//		puts("songkai\n\r");
		enter_wait_pen_down_mode();
	}
	//����
	else
	{
//		puts("anxia\n\r");
		enter_auto_measure_mode();
		//����adcת��
		ADCCON |= 1<<0;
	}
}

void ts_irq_s(void)
{
	int x,y;
	x = ADCDAT0;
	y = ADCDAT1;
	//��Ȼ�������ӡ
//	puts("ts_irq_s_1\n\r");
	if(!(ADCDAT0 & (1<<15)))
	{
		x &= 0x3ff;
		y &= 0x3ff;

		puts("x=");
		printHex(x);
		puts("   ");
		puts("y=");
		printHex(y);
		puts("\r\n");
//		printf("x = %08d, y = %08d\n\r", x, y);
		enable_ts_timer();
	}
	else
	{
		disable_ts_timer();
		enter_wait_pen_down_mode();
	}
	
	enter_wait_pen_up_mode();
}

void touchscreen_handle(int a)
{
	//�ж��ж�Դ ADC_S ADC_TC
	//ADC_TC
//	puts("handle\n\r");
	if(SUBSRCPND & (1<<9))
	{
		ts_irq_tc();
	}
	//ADC_S
	if(SUBSRCPND & (1<<10))
	{
		ts_irq_s();
	}
	//����жϱ�־λ
	SUBSRCPND = (1<<9) | (1<<10);
}

