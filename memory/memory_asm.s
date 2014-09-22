        .global make_page_directory
        #uint8_t * make_page_directory()
make_page_directory:    
        cmpl OS_end, %esp
        jl   stack_in_kernel_space

        .lcomm  saved_stack, 4
        movl %esp, saved_stack
        movl kernel_stack_start(,1), %esp

stack_in_kernel_space:
        call disable_paging
        
        call make_page_directory_nopaging

        call enable_paging
        movl saved_stack, %esp

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

        .global disable_paging
        #void disable_paging()
disable_paging:
        push %eax
        movl %cr0, %eax
        andl $0x7FFFFFFF, %eax
        movl %eax, %cr0
        pop  %eax
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
        