#include "key.h"



void key_config_n(int n)
{
	if (n < KEY_TOTLE_NUM) {
		// Config GPX3_2 ~ GPX3_5 as input
		set_nbit_val(GPX3CON, 4*(n+2), 4, 0x00);
	}
}

void key_config(void)
{
	int i = 0;

	for (i = 0; i < KEY_TOTLE_NUM; i++) {
		key_config_n(i);
	}
}

uchar8 key_get_bit(int n)
{
	return get_bit(GPX3DAT, n+2);
}

uint32 key_get_val(void)
{
	return get_nbit(GPX3DAT, 2, 4);
}

