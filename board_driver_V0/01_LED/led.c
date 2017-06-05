#include "led.h"

#define	LED_TOTLE_NUM	4

int led_config_n(int n)
{
	if (n < LED_TOTLE_NUM) {
		// Config CPM4_0 ~ GPM4_3 as output
		set_nbit_val(GPM4CON, 4*n, 4, 0x01);
		return 0;
	} else {
		return -1;
	}
}

void led_config(void)
{
	int i = 0;

	for (i = 0; i < LED_TOTLE_NUM; i++) {
		led_config_n(i);
	}
}

void led_on_all(void)
{
	
	set_nbit_val(GPM4DAT, 0, LED_TOTLE_NUM, 0x0);
}

void led_off_all(void)
{
	
	set_nbit_val(GPM4DAT, 0, LED_TOTLE_NUM, 0xF);
}

void led_on(int n)
{
	clr_bit(GPM4DAT, n);
}

void led_off(int n)
{
	set_bit(GPM4DAT, n);
}

void led_set_val(uint32 val)
{
	set_nbit_val(GPM4DAT, 0, LED_TOTLE_NUM, val);
}

uint32	led_get_val(void)
{
	get_nbit(GPM4DAT, 0, 4);
}

