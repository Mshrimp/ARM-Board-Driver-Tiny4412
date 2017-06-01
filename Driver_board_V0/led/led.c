#include "regs.h"
#include "led.h"

void led_init(void)
{
	GPM4CON	&= ~0xFFFF;
	GPM4CON |= 0x1111;
	GPM4DAT |= 0xF;
}

void led_on_all(void)
{
	GPM4DAT &= ~0xF;
}

void led_off_all(void)
{
	GPM4DAT |= 0xF;
}

void led_on(int bit)
{
	GPM4DAT &= ~(1 << bit);
}

void led_off(int bit)
{
	GPM4DAT |= (1 << bit);
}

unsigned int get_led_status(void)
{
	return (GPM4DAT & 0x0F);
}

