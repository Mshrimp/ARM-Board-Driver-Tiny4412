#ifndef	__BOARD_H__
#define	__BOARD_H__

#if 1


#define hal_set_bit(reg, bit)	((*(reg)) = ((*(reg)) | (0x01 << (bit))))
#define hal_clr_bit(reg, bit)	((*(reg)) = ((*(reg)) & ~(0x01 << (bit))))

#if 1
#define	set_bit_val(reg, bit, val)	((*(reg)) = (((*(reg)) & ~(0x01 << (bit))) | ((val) << (bit))))
#else
#define set_bit_val(reg, bit, val)	\
			((*(reg)) = ((val) ? (hal_set_bit((reg), (bit))) : (hal_clr_bit((reg), (bit)))))
#endif

#define	get_bit_val(reg, bit)	((*(reg)) & (0x01<<(bit)))

#define set_nbits_val(reg, bit, num, val)	((*(reg)) = (((*(reg)) & (~(((0x01<<(num))-1)<<(bit)))) | (((val)&((0x01<<(num))-1))<<(bit))))

//#define get_nbits_val(reg, bit, num)	(((*(reg)) >> (bit)) & ((0x01<<(num))-1))
#define get_nbits_val(reg, bit, num)	(((*(reg)) & (((0x01<<(num))-1)<<bit))>>bit)

#define set_reg_val(reg, val)	((*(reg)) = ((*(reg)) & 0 | (val)))

#define get_reg_val(reg)	(*(reg))





#else

#include <asm/io.h>

#define	hal_readb(reg)		__raw_readb(reg)
#define	hal_readw(reg)		__raw_readw(reg)
#define	hal_readl(reg)		__raw_readl(reg)

#define	hal_writeb(val, reg)		__raw_writeb(val, reg)
#define	hal_writew(val, reg)		__raw_writew(val, reg)
#define	hal_writel(val, reg)		__raw_writel(val, reg)



void hal_set_bit(unsigned int reg, unsigned int bit)
{
	hal_writel((hal_readl(reg) | (0x01 << bit)), reg);
}

void hal_clear_bit(unsigned int reg, unsigned int bit)
{
	hal_writel((hal_readl(reg) & ~(0x01 << bit)), reg);
}

void set_bit_val(unsigned int reg, unsigned int bit, unsigned val)
{
	if (val == 0) {
		hal_clear_bit(reg, bit);	
	} else {
		hal_set_bit(reg, bit);
	}
}

unsigned int get_bit_val(unsigned int reg, unsigned int bit)
{
	return (hal_readl(reg) & (0x01 << bit));
}

void set_nbits_val(unsigned int reg, unsigned int bit, unsigned int num, unsigned int val)
{
	printk("set nbits val\n");
	hal_writel(((hal_readl(reg) & (~(((0x01<<num)-1)<<bit))) | ((val&((0x01<<num)-1))<<bit)), reg);
	printk("set nbits val OK\n");
}

unsigned int get_nbits_val(unsigned int reg, unsigned int bit, unsigned int num)
{
	return ((hal_readl(reg)>>bit) & ((0x01<<num)-1));
}

void set_reg_val(unsigned int reg, unsigned int val)
{
	hal_writel(val, reg);
}

unsigned int get_reg_val(unsigned int reg)
{
	return hal_readl(reg);
}

#endif




#endif	/* __BOARD_H__ */

