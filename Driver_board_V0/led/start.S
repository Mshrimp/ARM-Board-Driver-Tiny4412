.text
.globl _start
_start:
	ldr sp, =0x02027800

	bl main

halt_loop:
	b halt_loop
