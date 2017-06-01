ENTRY(_start);
/*OUTPUT("b.out");*/
/*STARTUP("start.o");*/
/*OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm");*/
OUTPUT_FORMAT("elf32-littlearm");
OUTPUT_ARCH(arm);

SECTIONS
{
	. = 0x50000000;
	.text : {
		start.o(.text);	
		main.o(.text);
		*(.text);
	}

	. = ALIGN(4);
	.rodata : {
		*(.rodata);
	}

	. = ALIGN(4);
	.data : {
		*(.data);
	}

	. = ALIGN(4);
	.mdata : {
		*(.mdata);
	}

	. = ALIGN(4);
	__bss_start = .;	/*int __bss_start X*/	/*__bss_start: */
	.bss : {
		*(.bss);
	}
	__bss_end = .;		/*__bss_end: */

}
