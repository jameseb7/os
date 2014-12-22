
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
		mov		%eax,		%cr3

		//load the new stack pointer and store the old stack pointer
		xchg	%eax,		%esp

		//pop the registers from the stack, preserving the return value in eax
		mov		%eax,		tmp
		popa
		mov		tmp,		%eax

		//end the stack frame and return
		pop 	%ebp
		ret


//void run_idle_process(uint32_t * stack_pointer_store);
		.global run_idle_process
run_idle_process:
		//set up the stack frame and push the registers to the stack
		push 	%ebp
		mov		%esp,					%ebp
		pusha

		//put the stack pointer store in eax
		mov 	8(%ebp),				%eax

		//save the current stack pointer
		mov 	%esp, 					0(%eax)
		
		//set the stack pointer to the kernel bootstrap stack
		mov 	kernel_stack_start(,1),	%esp

		//set the interrupt flag and send the end of interrupt signal
		sti		
		mov  	$0x20, 					%al
		outb 	%al, 					$0x20		
		outb 	%al, 					$0xA0

		//halt and wait for interrupts
halt_loop:		
		hlt
		jmp 	halt_loop

		
//void start_kernel_process(void (*start_function)(), uint32_t page_directory, uint32_t * stack_pointer_store);
		.global start_kernel_process
start_kernel_process:
		//set up the stack frame and push the registers
		push 	%ebp
		mov		%esp,						%ebp
		pusha

		//put the start function address in edx and the page directory in eax and the stack pointer store in ebx
		mov 	8(%ebp),					%edx
		mov		12(%ebp),					%eax
		mov		16(%ebp),					%ebx

		//switch to the process' page table
		mov		%eax, 						%cr3

		//set the stack pointer to the start of the kernel stack and store the old stack pointer
		mov		process_kernel_stack(,1),	%ecx
		xchg	%ecx,						%esp
		mov		%ecx,						0(%ebx)

		//push the stack for the iret instruction
		pushf 							//EFLAGS register
		mov 	$0x08, 						%cx 
		push 	%cx						//CS segment selector
		push	%edx					//EIP value (address of start function)

		//return to the new process
		iret