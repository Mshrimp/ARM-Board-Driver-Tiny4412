
#include "chip.h"


#define	set_gpio_output(addr, bit)		\
	(*(addr) = ((*(addr) & (~((1<<4)-1) << ((bit) * 4))) | (0x1 << ((bit) * 4))))

#define	set_gpio_input(addr, bit)		\
	(*(addr) = (*(addr) & (~((1<<4)-1) << ((bit) * 4))))

#define	set_gpio_high(addr, bit)	(*(addr) |= (0x1 << (bit)))

#define	set_gpio_low(addr, bit)		(*(addr) &= ~(0x1 << (bit)))

#define get_gpio_value(addr, bit)	(*(addr) & (0x1 << (bit)))
