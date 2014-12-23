		global loader
		extern kmain

		section .text

ALIGN_FLAG   equ 1 << 0
MEMINFO_FLAG equ 1 << 1
FLAGS        equ ALIGN_FLAG | MEMINFO_FLAG
MAGIC        equ 0x1BADB002
CHECKSUM     equ -(MAGIC + FLAGS)

multiboot_header:		
		align 4
		dd MAGIC
		dd FLAGS
		dd CHECKSUM

		section .bss

STACKSIZE equ 0x4000
kernel_stack:	resb STACKSIZE
		global kernel_stack_start
kernel_stack_start equ kernel_stack + STACKSIZE

		global mb_data
		global mb_magic
mb_data:		resb 4
mb_magic:		resb 4

		section .text

loader:
		cli
		mov esp, kernel_stack_start
		mov [mb_magic], eax
		mov [mb_data], ebx

		call kmain

halt:	hlt
		jmp halt

		extern interrupt_handler
		global interrupt_entry
interrupt_entry:
		pushad
		call interrupt_handler
		popad
		iret
