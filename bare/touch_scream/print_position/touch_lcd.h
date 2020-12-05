#ifndef _TOUCH_SCREEN_H_
#define _TOUCH_SCREEN_H_


void enter_auto_measure_mode(void);
void enter_wait_pen_down_mode(void);
void enter_wait_pen_up_mode(void);
void touchscreen_handle(int);
void adc_ts_init(void);
void ts_irq_init(void);
void touchscreen_init(void);
void ts_irq_tc(void);
void ts_irq_s(void);

#endif