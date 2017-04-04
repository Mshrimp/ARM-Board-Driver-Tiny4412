#ifndef	__HAL_H__
#define	__HAL_H__



#define	GPIO1_BASE_ADDR	0x11400000

#define GPA0_OFFSET	0x0000
#define	GPA1_OFFSET	0x0020
#define	GPB_OFFSET	0x0040
#define	GPC0_OFFSET	0x0060
#define	GPC1_OFFSET	0x0080
#define	GPD0_OFFSET	0x00A0
#define	GPD1_OFFSET	0x00C0
#define	GPF0_OFFSET	0x0180
#define	GPF1_OFFSET	0x01A0
#define	GPF2_OFFSET	0x01C0
#define	GPF3_OFFSET	0x01E0
#define	GPJ0_OFFSET	0x0240
#define	GPJ1_OFFSET	0x0260

	
#define	GPIO2_BASE_ADDR	0x11000000

#define GPK0_OFFSET	0x0040
#define GPK1_OFFSET	0x0060
#define GPK2_OFFSET	0x0080
#define GPK3_OFFSET	0x00A0
#define GPL0_OFFSET	0x00C0
#define GPL1_OFFSET	0x00E0
#define GPL2_OFFSET	0x0100
#define GPY0_OFFSET	0x0120
#define GPY1_OFFSET	0x0140
#define GPY2_OFFSET	0x0160
#define GPY3_OFFSET	0x0180
#define GPY4_OFFSET	0x01A0
#define GPY5_OFFSET	0x01C0
#define GPY6_OFFSET	0x01E0
#define GPM0_OFFSET	0x0260
#define GPM1_OFFSET	0x0280
#define GPM2_OFFSET	0x02A0
#define GPM3_OFFSET	0x02C0
#define GPM4_OFFSET	0x02E0




#define	GPIO_CON_OFFSET		0x0000
#define	GPIO_DAT_OFFSET		0x0004
#define	GPIO_PUD_OFFSET		0x0008
#define	GPIO_DRV_OFFSET		0x000C
#define	GPIO_CONPDN_OFFSET	0x0010
#define	GPIO_PUDPDN_OFFSET	0x0014



	
//====================================================
#define	GPM4CON_ADDR	(GPIO2_BASE_ADDR + GPM4_OFFSET + GPIO_CON_OFFSET)
#define	GPM4DAT_ADDR	(GPIO2_BASE_ADDR + GPM4_OFFSET + GPIO_DAT_OFFSET)
#define	GPM4PUD_ADDR	(GPIO2_BASE_ADDR + GPM4_OFFSET + GPIO_PUD_OFFSET)
#define	GPM4DRV_ADDR	(GPIO2_BASE_ADDR + GPM4_OFFSET + GPIO_DRV_OFFSET)
#define	GPM4CONPDN_ADDR	(GPIO2_BASE_ADDR + GPM4_OFFSET + GPIO_CONPDN_OFFSET)
#define	GPM4PUDPDN_ADDR	(GPIO2_BASE_ADDR + GPM4_OFFSET + GPIO_PUDPDN_OFFSET)

//====================================================
typedef struct {
	uint32	CON;
	uint32	DAT;
	uint32	PUD;
	uint32	DRV;
	uint32	CONPDN;
	uint32	PUDPDN;
}sGPIO;

#define	GPM4	(sGPIO *)(GPIO2_BASE_ADDR + GPM4_OFFSET)

//====================================================





static void GpioSetPinDirection(sGPIO *spGPIO, uint32 bit, uint8 InOut)
{
	uint32	val;

	val = readl(spGPIO->CON);
	if (InOut & PIN_DIR_INPUT) {
		val &= ~(0x0F << (bit*4));
	} else if (InOut & PIN_DIR_OUTPUT) {
		val &= ~(0x0F << (bit*4));
		val |= 0x01 << (bit*4);
	}
	writel(val, spGPIO->CON);
}

static void GpioSetPinDirection_Addr(uint32 addr, uint32 bit, uint8 InOut)
{
	uint32	val;

	val = readl(addr);
	if (InOut & PIN_DIR_INPUT) {
		val &= ~(0x0F << (bit*4));
	} else if (InOut & PIN_DIR_OUTPUT) {
		val &= ~(0x0F << (bit*4));
		val |= 0x01 << (bit*4);
	}
	writel(val, addr);
}

static void GpioSetPinExport(sGPIO *spGPIO, uint32 bit, uint8 HighLow)
{
	uint32 val;
	val = readl(spGPIO->DAT);
	if (HighLow & PIN_OUTPUT_LOW) {
		val &= ~(0x01 << bit);
	} else {
		val |= 0x01 << bit;
	}
	writel(val, spGPIO->DAT);
}

static void GpioSetPinExport_Addr(uint32 addr, uint32 bit, uint8 HighLow)
{
	uint32 val;
	val = readl(addr);
	if (HighLow & PIN_OUTPUT_LOW) {
		val &= ~(0x01 << bit);
	} else {
		val |= 0x01 << bit;
	}
	writel(val, addr);
}

static int32 GpioGetPinValue(sGPIO *spGPIO, uint32 bit)
{
	uint32 val;	
	val = readl(spGPIO->DAT);

	return (val & (0x01 << bit)) ? 1 : 0;
}

static int32 GpioGetPinValue_Addr(uint32 addr, uint32 bit)
{
	uint32 val;	
	val = readl(addr);

	return (val & (0x01 << bit)) ? 1 : 0;
}



#endif	//__HAL_H__

