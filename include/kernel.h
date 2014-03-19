#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

void halt(void);

/*output functions*/
unsigned int write_screen(const char * str,
		 char foreclr, char backclr,
		 unsigned int x, unsigned int y);
void clear_screen(void);
void kprint(const char * str);
void kprintln(const char * str);
void kprint_uint32(uint32_t input);
void kprintln_uint32(uint32_t input);
void kprint_uint64(uint64_t input);
void kprintln_uint64(uint64_t input);

/*memory setup functions*/
/*void make_gdt(void);
void make_idt(void);*/
void index_pages(void);
uint8_t * make_page_directory(void);
void setup_virtual_page_allocator(void);
void init_virtual_page_allocator(void);

/*memory allocation functions*/
void allocate_physical_page(uint32_t);
void free_physical_page(uint32_t);
void * allocate_virtual_pages_high(uint32_t);
void * allocate_virtual_pages_low(uint32_t);

#endif /*KERNEL_H*/