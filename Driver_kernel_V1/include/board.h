#ifndef	__BOARD_H__
#define	__BOARD_H__


#include <asm/io.h>

#define	readb(a)		__raw_readb(a)
#define	readw(a)		__raw_readw(a)	
#define	readl(a)		__raw_readl(a)	

#define	writeb(v,a)		__raw_writeb(v, a)	
#define	writew(v,a)		__raw_writew(v, a)	
#define	writel(v,a)		__raw_writel(v, a)	

#define	hal_writel(val, reg)		writel(val, reg)
#define	hal_readl(reg)				readl(reg)



#if 0
#define	set_bit(reg, bit)	\
			hal_writel((hal_readl(reg) | (0x01 << bit)), reg)
			
#define	clear_bit(reg, bit)	\
			hal_writel((hal_readl(reg) & ~(0x01 << bit)), reg)
			
#define	set_bit_val(reg, bit, val)		\
			do {						\
				if (val == 0)			\
					clear_bit(reg, bit);\
				else					\
					set_bit(reg, bit);	\
			} while(0)

#define	get_bit_val(reg, bit)	\
			(hal_readl(reg) & (0x01 << bit))

#define	set_nbits_val(reg, bit, num, val)	\
			hal_writel(hal_readl(reg) & ~(((0x01<<num)-1)<<bit) | (val&(0x01<<num)-1)<<bit, reg)

#define	get_nbits_val(reg, bit, num)	\
			((hal_readl(reg)>>bit) & ((0x01<<num)-1))






#define	get_(reg, bit)	\
			(hal_readl(reg) & (0x01 << bit))

#define	set_bit_val(reg, bit, val)	\
			(hal_writel())

#else
void set_bit(unsigned int reg, unsigned int bit)
{
	hal_writel((hal_readl(reg) | (0x01 << bit)), reg);
}

void clear_bit(unsigned int reg, unsigned int bit)
{
	hal_writel((hal_readl(reg) & ~(0x01 << bit)), reg);
}

void set_bit_val(unsigned int reg, unsigned int bit, unsigned val)
{
	if (val == 0) {
		clear_bit(reg, bit);	
	} else {
		set_bit(reg, bit);
	}
}

int get_bit_val(unsigned int reg, unsigned int bit)
{
	return (hal_readl(reg) & (0x01 << bit));
}

void set_nbits_val(unsigned int reg, unsigned int bit, unsigned int num, unsigned val)
{
	hal_writel(hal_readl(reg) & ~(((0x01<<num)-1)<<bit) | (val&(0x01<<num)-1)<<bit, reg);
}

int get_nbits_val(unsigned int reg, unsigned int bit, unsigned int num)
{
	return ((hal_readl(reg)>>bit) & ((0x01<<num)-1))
}

void set_reg_val(unsigned int reg, unsigned val)
{
	hal_writel(val, reg);
}

int get_reg_val(unsigned int reg)
{
	return hal_readl(reg);
}

#endif




#endif

