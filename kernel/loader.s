.global loader                                  # making entry point visible to linker

#.global error_handler

# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,    1<<0                             # align loaded modules on page boundaries
.set MEMINFO,  1<<1                             # provide memory map
.set FLAGS,    ALIGN | MEMINFO                  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002                       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)                 # checksum required

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# reserve initial kernel stack space
.set STACKSIZE, 0x4000                          # that is, 16k.
.comm kernel_stack, STACKSIZE                   # reserve 16k stack on a doubleword boundary
.comm mb_data, 4                                # space to store pointer to Multiboot data structure
.comm mb_magic, 4                               # space to store pointer to Multiboot magic number

.global kernel_stack_start
kernel_stack_start:
		.long kernel_stack + STACKSIZE
        
loader:
    cli	
    movl  kernel_stack_start(,1), %esp     # set up the stack
    movl  %eax, mb_magic                        # Multiboot magic number
    movl  %ebx, mb_data                         # Multiboot data structure
        
    call kmain                                  # call kernel proper

    cli
hang:
    hlt                                         # halt machine should kernel return
    jmp   hang

.global interrupt_entry
interrupt_entry:
    pusha
    call interrupt_handler
    popa
    iret
