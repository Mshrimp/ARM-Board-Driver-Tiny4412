#ifndef	__PIN_H__
#define	__PIN_H__






typedef	struct	{
#define	PIN_DIR_INPUT	0x00
#define	PIN_DIR_OUTPUT	0x01
	void (*SetDirection)(void *PinInfo, uint8 InOut);
#define	PIN_OUTPUT_LOW	0x00
#define	PIN_OUTPUT_HIGH	0x01
	void (*SetExport)(void *PinInfo, uint8	HighLow);
	void (*GetPinValue)(void *PinInfo);


}ClassPin;





#endif	//__PIN_H__

