
#include"init.h"
#include "s3c2440_soc.h"

void set_nor_tacc(int val)
{
	BANKCON0 &= val<<8;
}

