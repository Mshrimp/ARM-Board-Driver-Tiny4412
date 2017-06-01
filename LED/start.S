
	.section	.text
	.align	2
	.global	_start

_start:
	stmfd	sp!, {lr}

	bl	main

	ldmfd	sp!, {pc}

