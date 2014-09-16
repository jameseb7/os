
		.lcomm tmp, 4

//uint32_t switch_process_asm(uint32_t stack_pointer, uint32_t page_directory);
		.global switch_process_asm
switch_process_asm:
		//set up the stack frame and push the registers to the stack
		push 	%ebp
		mov		%esp,		%ebp
		pusha

		//put the stack pointer in edx and the page directory in eax
		mov 	8(%ebp),	%edx
		mov		12(%ebp),	%eax

		//load the new page table
		mov		%eax,		%cr4

		//load the new stack pointer and store the old stack pointer
		xchg	%eax,		%esp

		//pop the registers from the stack, preserving the return value in eax
		mov		%eax,		tmp
		popa
		mov		tmp,		%eax

		//end the stack frame and return
		pop 	%ebp
		ret
