		extern kernel_stack_start
		extern OS_end
		extern make_page_directory_nopaging

		section .bss
		
saved_stack:	resb 4
gdtr:			resb 6

		section .text

		global make_page_directory
		;; uint8_t * make_page_directory
make_page_directory:
		mov [saved_stack], esp
		cmp esp, OS_end
		jl  stack_in_kernel_space
		mov esp, kernel_stack_start
stack_in_kernel_space:
		call disable_paging
		call make_page_directory_nopaging
		call enable_paging
		mov  esp, [saved_stack]
		ret

		global enable_paging
		;; void enable_paging()
enable_paging:
		push eax
		mov  eax, cr0
		or   eax, 0x8000_0000
		mov  cr0, eax
		pop  eax
		ret

		global disable_paging
		;; void disable_paging()
disable_paging:
		push eax
		mov  eax, cr0
		and  eax, 0x7FFF_FFFF
		mov  cr0, eax
		pop  eax
		ret

		global load_page_directory
		;; void load_page_directory(uint8_t * page_directory)
load_page_directory:
		push ebp
		mov  ebp, esp
		push eax
		mov  eax, [ebp + 8]
		mov  cr3, eax
		pop  eax
		pop  ebp
		ret

		global load_gdt
		;; void load_gdt(uint32_t gdt, uint16_t gdt_size)
load_gdt:
		push ebp
		mov  ebp, esp
		push eax
		mov  ax, [ebp + 12]
		mov  [gdtr], ax
		mov  eax, [ebp + 8]
		mov  [gdtr + 2], eax
		lgdt [gdtr]
		mov  ax, 0x10
		mov  ds, ax
		mov  ss, ax
		mov  es, ax
		mov  fs, ax
		mov  gs, ax
		jmp  0x08:gdt_jump
gdt_jump:		
		mov  ax, 0x18
		ltr  ax
		pop  eax
		pop  ebp
		ret
