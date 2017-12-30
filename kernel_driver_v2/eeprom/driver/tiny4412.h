#ifndef	__TINY4412_H__
#define	__TINY4412_H__



#define	GPIO_BASE			0x11400000
#define	GPA1CON_ADDR		(GPIO_BASE + 0x20)
#define	GPA1DAT_ADDR		(GPIO_BASE + 0x24)




#define	set_gpio_output(addr, bit)		\
	(*(addr) = ((*(addr) & (~((1<<4)-1) << ((bit) * 4))) | (0x1 << ((bit) * 4))))

#define	set_gpio_input(addr, bit)		\
	(*(addr) = (*(addr) & (~((1<<4)-1) << ((bit) * 4))))

#define	set_gpio_high(addr, bit)	(*(addr) |= (0x1 << (bit)))

#define	set_gpio_low(addr, bit)		(*(addr) &= ~(0x1 << (bit)))

#define get_gpio_value(addr, bit)	(*(addr) & (0x1 << (bit)))

#endif
