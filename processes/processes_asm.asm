
                section .bss
tmp:    resb 4

                extern kernel_stack_start
                extern process_kernel_stack
                extern sti

                section .rodata
run_idle_process_str:           db "run_idle_process()", 0
start_kernel_process_str:       db "start_kernel_process()", 0
                
                section .text

;;; uint32_t switch_process_asm(uint32_t stack_pointer, uint32_t page_directory)
                global switch_process_asm
switch_process_asm:
                ;; set up the stack frame and push the registers to the stack
                push    ebp
                mov     ebp, esp
                pushf
                pushad

                ;; put the stack pointer in edx and the page directory in eax
                mov     edx, [ebp + 8]
                mov     eax, [ebp + 12]

                ;; load the new page table
                mov             cr3, eax

                ;; load the new stack pointer and store the old stack pointer
                xchg    esp, eax

                ;; pop the registers from the stack, preserving the return value in eax
                mov     [tmp], eax
                popad
                mov     eax, [tmp]
                popf

                ;; end the stack frame and return
                pop     ebp
                ret


;;; void run_idle_process(uint32_t * stack_pointer_store)
                global run_idle_process
run_idle_process:
                ;; set up the stack frame and push the registers to the stack
                push    ebp
                mov     ebp, esp
                pushad

                ;; put the stack pointer store in eax
                mov     eax, [ebp + 8]

                ;; save the current stack pointer
                mov     [eax], esp
                
                ;; set the stack pointer to the kernel bootstrap stack
                mov     esp, kernel_stack_start

                ;; send the end of interrupt signal     and set the interrupt flag
                mov     al, 0x20
                out     0x20, al                
                out     0xA0, al
                sti

                ;; halt and wait for interrupts
halt_loop:              
                hlt
                jmp     halt_loop

                
;;; void start_kernel_process(void (*start_function)(), uint32_t page_directory, uint32_t * stack_pointer_store)
                global start_kernel_process
start_kernel_process:
                ;; set up the stack frame and push the registers
                push    ebp
                mov     ebp, esp
                pushad

                ;; put the start function address in edx and the page directory in eax and the stack pointer store in ebx
                mov     edx, [ebp + 8]
                mov     eax, [ebp + 12]
                mov     ebx, [ebp + 16]
                
                ;; switch to the process' page table
                mov             cr3, eax

                ;; set the stack pointer to the start of the kernel stack and store the old stack pointer
                mov     ecx, [process_kernel_stack]
                xchg    esp, ecx
                mov     [ebx], ecx

                ;; send the end of interrupt signal
                mov     al, 0x20
                out     0x20, al                
                out     0xA0, al

                ;; push the stack for the iret instruction
                pushf                           ;EFLAGS register
                pop     eax
                or      eax, (1 << 9)   ;set the interrupt flag
                push    eax
                mov     cx, 0x08        
                push    cx                      ;CS segment selector
                push    edx                     ;EIP value (address of start function)

                ;; return to the new process
                sti
                iret
