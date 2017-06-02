#ifndef	__BOARD_H__
#define	__BOARD_H__


typedef	unsigned int	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uchar8;

typedef volatile uint32		vuint32;
typedef volatile uint16		vuint16;
typedef volatile uchar8		vuchar8;


#define	set_bit(reg, bit)	\
			(*(vuint32 *)(reg) |= (1<<(bit)))

#define	clr_bit(reg, bit)	\
			(*(vuint32 *)(reg) &= ~(1<<(bit)))

#define	set_bit_val(reg, bit, val)	\
			(*(vuint32 *)(reg) = (((*(vuint32 *)(reg)) & ~(1<<(bit))) | ((val)<<(bit))))

#define	set_nbit_val(reg, bit, n, val)	\
			(*(vuint32 *)(reg) = (((*(vuint32 *)(reg)) & ~(((1<<(n))-1)<<(bit))) | (((val)&((1<<(n))-1))<<(bit))))

#define	set_reg_val(reg, val)	\
			(*(vuint32 *)(reg) = (val))

#define	get_val(reg)	\
			(*(vuint32 *)(reg))

#define	get_bit(reg, bit)	\
			((*(vuint32 *)(reg) & (1<<(bit))) >> (bit))

#define	get_nbit(reg, bit, n)	\
			((*(vuint32 *)(reg) & (((1<<(n))-1)<<(bit))) >> (bit))


#endif	/* __BOARD_H__ */

