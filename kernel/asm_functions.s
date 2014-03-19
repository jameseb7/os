        .global load_gdt
        #void load_gdt(uint32_t gdt, uint16_t gdt_size)
load_gdt:       
        push %ebp
        movl %esp, %ebp

        .lcomm  gdtr, 6
        movw 12(%ebp), %ax
        movw %ax, gdtr
        movl 8(%ebp), %eax
        movl %eax, gdtr+2
	lgdt gdtr
        
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %ss
	movw %ax, %es
	movw %ax, %fs
        ljmp $0x08, $gdt_jump
gdt_jump:
        movw $0x18, %ax
        ltr  %ax
        
        pop  %ebp
        ret


        .global load_idt
        #void load_idt(uint32_t idt, uint16_t idt_size)
load_idt:     
        push %ebp
        movl %esp, %ebp

        push %eax
        
        .lcomm  idtr, 6
        movw 12(%ebp), %ax
        movw %ax, idtr
        movl 8(%ebp), %eax
        movl %eax, idtr+2
        lidt idtr

        pop  %eax
        pop  %ebp
        ret

        
        .global load_page_directory
        #void load_page_directory(uint8_t * page_directory)
load_page_directory:   
        push %ebp
        movl %esp, %ebp

        movl 8(%ebp), %eax
        movl %eax, %cr3

        pop %ebp
        ret


        .global enable_paging
        #void enable_paging()
enable_paging:
        push %eax
        movl %cr0, %eax
        orl  $0x80000000, %eax
        movl %eax, %cr0
        pop  %eax
        ret
        
