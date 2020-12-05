#include "eint.h"
#include "irq.h"
#include"s3c2440_soc.h"

//�ж�Դ����
void key_init(void)
{
	//����GPF0(EINT0) ��GPF2(EINT2)
	GPFCON &= ~(3<<(2*0) | 3<<(2*2));
	GPFCON |= (2<<(2*0) | 2<<(2*2));

	//����GPG3(EINT11) ��GPG11(EINT19)
	GPGCON &= ~(3<<(2*3) | 3<<(2*11));
	GPGCON |= (2<<(2*3) | 2<<(2*11));

	//����EXTINTn,����˫���ش���
	EXTINT0 |= 7<<0 | 7<<8;
	EXTINT1 |= 7<<12;
	EXTINT2 |= 7<<12;

	//ʹ���ж�
	EINTMASK &= ~(1<<11 | 1<<19);

	//EINTPEND�ֱ�4-23�ĸ��жϷ�����Ҫ�������	
}

//�жϿ���������
void EINT_init(void)
{
	//SRCPND��ʾ��Щ�ж�Դ�����ж������ȴ��жϷ���д1����
	
	//INTMSKʹ���ж�
	
	//INTPND��ʾ����ִ�е��жϷ���д1����
	
	//INTOFFSET��ʾINTPND����λ��1����INTPND��SRCPND���Զ�����

	//ע���ж�
	irq_register(0,EINT0_func);
	irq_register(2,EINT2_func);
	irq_register(5,EINT4_23_func);
}

void EINT0_func(int bit)
{
//�ⲿ�ж�0
	//�жϰ����Ƿ���
	if(!(GPFDAT & 1<<0))
	{
		//����
		GPFDAT &= ~(1<<4);
	}
	else
	{
		//�ص�
		GPFDAT |= (1<<4);
	}
}

void EINT2_func(int bit)
{	
	if(!(GPFDAT & 1<<2))
	{
		GPFDAT &= ~(1<<5);
	}
	else
	{
		GPFDAT |= (1<<5);
	}
}

void EINT4_23_func(int bit)
{
	//�ⲿ�ж�4-23
	//�ⲿ�ж�11
	if(EINTPEND & 1<<11)
	{
		if(!(GPGDAT & 1<<3))
		{
			GPFDAT &= ~(1<<6);
		}
		else
		{
			GPFDAT |=  (1<<6);
		}
		//����
		EINTPEND |= 1<<11;
	}
	//�ⲿ�ж�19
	else if(EINTPEND & 1<<19)
	{
		if(!(GPGDAT & 1<<11))
		{
			GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
		}
		else
		{
			GPFDAT |= (1<<4) | (1<<5) | (1<<6);
		}
		//����
		EINTPEND |= 1<<19;
	}

}

