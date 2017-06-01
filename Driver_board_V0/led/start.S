
	.section	.text
	.align	2
	.global	_start
_start:
	stmfd sp!, {lr}
	
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x11
	msr cpsr, r0

	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x13
	msr cpsr, r0

	bl	main

	ldmfd sp!, {pc}
	

