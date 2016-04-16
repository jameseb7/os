
	section .text

	extern divide_by_zero_handler
	global interrupt0x0
interrupt0x0:
	pushad
	call divide_by_zero_handler
	popad
	iret
	
	extern debug_handler
	global interrupt0x1
interrupt0x1:
	pushad
	call debug_handler
	popad
	iret

	extern double_fault_handler
	global interrupt0x8	
interrupt0x8:	
	pushad
	call double_fault_handler
	popad
	iret

	extern invalid_tss_handler
	global interrupt0xA
interrupt0xA:
	pushad
	call invalid_tss_handler
	popad
	iret

	extern segment_not_present_handler
	global interrupt0xB
interrupt0xB:
	push ebp
	mov  ebp, esp
	pushad
	mov  ax, [ebp + 4] 
	push ax 		; push error code onto stack
	call segment_not_present_handler
	pop  ax
	popad
	pop ebp
	pop ax			; remove error code since it isn't removed by iret
	iret

	extern general_protection_fault_handler
	global interrupt0xD
interrupt0xD:
	push ebp
	mov  ebp, esp
	pushad
	mov  eax, [ebp+4]
	push eax
	call general_protection_fault_handler
	pop  eax
	popad
	pop  ebp
	pop  eax
	iret

	extern page_fault_handler
	global interrupt0xE
interrupt0xE:
	push ebp
	mov  ebp, esp
	pushad
	mov  eax, [ebp+8]	; instruction address
	push eax
	mov  eax, cr2		; erroneous address
	push eax
	mov  eax, [ebp+4]	; error code
	push eax
	call page_fault_handler
	pop  eax
	pop  eax
	pop  eax
	popad
	pop  ebp
	pop  eax  		; TODO: this clobbers eax, need to fix for a proper page fault handler
	iret

	extern timer_handler
	global interrupt0x20
interrupt0x20:
	cli
	pushad
	call timer_handler
	popad
	sti
	iret

	global	empty__hardware_interrupt_entry
empty_interrupt_entry:
	push eax
	mov  al, 0x20
	out  0x20, al
	out  0xA0, al
	pop  eax
	iret
