#ifndef _TIMER_H_
#define _TIMER_H_

#define TEVENT 15
#define NULL ((void *)0)

typedef void(*ptimer)(void);
typedef struct timer_desc{
	char   *name;
	ptimer fp ;
}timer_desc;
extern timer_desc t_event[TEVENT];

#endif

