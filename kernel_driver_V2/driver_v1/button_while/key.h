
#ifndef	__KEY_H__
#define	__KEY_H__


#define	KEY_BASE_ADDR		0x11000000

#define	KEY_CON_ADDR		(KEY_BASE_ADDR + 0x06C0)
#define	KEY_DAT_ADDR		(KEY_BASE_ADDR + 0x06C4)


#define	GPIO_INPUT			0x0
#define	GPIO_OUTPUT			0x1


#define	GPIO_OUTPUT_HIGH	0x1
#define	GPIO_OUTPUT_LOW		0x0
#define	GPIO_INT			0xF


#define	KEY_TOTLE			4


#endif
