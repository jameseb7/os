	.global interrupt0x0
interrupt0x0:
	pusha
	call divide_by_zero_handler
	popa
	iret

	.global interrupt0x8	
interrupt0x8:	
	pusha
	call double_fault_handler
	popa
	iret

	.global interrupt0xA
interrupt0xA:
	pusha
	call invalid_tss_handler
	popa
	iret

	.global interrupt0xB
interrupt0xB:
	mov %eax, %cr2
	pop %ax
	pusha
	push %ax
	call segment_not_present_handler
	popa
	mov %cr2, %eax
	iret

	.global interrupt0xC
interrupt0xC:
	pusha
	call general_protection_fault_handler
	popa
	iret

	.global interrupt0xD
interrupt0xD:	
	pusha
	call page_fault_handler
	popa
	iret
	