
#include"init.h"
#include "s3c2440_soc.h"

void set_nor_tacc(int val)
{
	BANKCON0 &= val<<8;
}

void sdram_init(void)
{
	BWSCON = 0x22000000;

	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	REFRESH  = 0x8404f5;

	BANKSIZE = 0xb1;

	MRSRB6   = 0x20;
	MRSRB7   = 0x20;
}

