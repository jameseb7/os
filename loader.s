.global loader                          # making entry point visible to linker

#.global error_handler

# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,    1<<0                     # align loaded modules on page boundaries
.set MEMINFO,  1<<1                     # provide memory map
.set FLAGS,    ALIGN | MEMINFO          # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002               # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)         # checksum required

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# reserve initial kernel stack space
.set STACKSIZE, 0x4000                  # that is, 16k.
.comm stack, STACKSIZE                  # reserve 16k stack on a doubleword boundary
.comm  mbd, 4                           # we will use this in kmain
.comm  magic, 4                         # we will use this in kmain

.comm  gdtr, 6
.comm  idtr, 6

loader:
    movl  $(stack + STACKSIZE), %esp    # set up the stack
    movl  %eax, magic                   # Multiboot magic number
    movl  %ebx, mbd                     # Multiboot data structure

    cli
    call make_gdt
    movw gdt_size, %ax
    movw %ax, gdtr
    movl gdt, %eax
    movl %eax, gdtr+2
    lgdt gdtr
    ljmp $0x08, $gdt_jump
gdt_jump:
    movw $0x18, %ax
    ltr  %ax

    call make_idt
    movw idt_size, %ax
    movw %ax, idtr
    movl idt, %eax
    movl %eax, idtr+2
    lidt idtr
    sti

    call index_pages
	
    call make_page_directory
    movl $page_directory, %eax
    movl %eax, %cr3
    movl %cr0, %eax
    orl  $0x80000000, %eax
    movl %eax, %cr0

    call setup_virtual_page_allocator

    call kmain                         # call kernel proper

    cli
hang:
    hlt                                 # halt machine should kernel return
    jmp   hang

.global interrupt_entry
interrupt_entry:
    pusha
    call interrupt_handler
    popa
    iret
