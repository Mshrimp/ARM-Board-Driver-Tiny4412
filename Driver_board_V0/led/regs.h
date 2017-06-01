#ifndef	__REGS_H__
#define	__REGS_H__

#define GPIO_BASE		0x11000000

#define		GPM4CON		(*(volatile unsigned long *)(GPIO_BASE + 0x02E0))
#define		GPM4DAT		(*(volatile unsigned long *)(GPIO_BASE + 0x02E4))


#endif	/* __REGS_H__ */

