.lcomm tmp 4

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
	mov %eax, tmp
	pop %ax
	pusha
	push %ax
	call segment_not_present_handler
	popa
	mov tmp, %eax
	iret

	.global interrupt0xD
interrupt0xD:
	mov %eax, tmp
	pop %eax
	pusha
	push %eax
	call general_protection_fault_handler
	popa
	mov tmp, %eax
	iret

	.global interrupt0xE
interrupt0xE:	
	pusha
	call page_fault_handler
	popa
	iret

	.global	empty_interrupt_entry
empty_interrupt_entry:
	iret
