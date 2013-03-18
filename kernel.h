#ifndef KERNEL_H
#define KERNEL_H

/*output functions*/
unsigned int write_screen(const char * str,
		 char foreclr, char backclr,
		 unsigned int x, unsigned int y);
void clear_screen(void);
void kprint(const char * str);
void kprintln(const char * str);
char * uint32_to_hex_string(uint32_t input);
char * uint64_to_hex_string(uint64_t input);

/*memory setup functions*/
void make_gdt(void);
void make_idt(void);
void index_pages(void);
void make_page_directory(void);

/*memory allocation functions*/
void allocate_physical_page(uint32_t);

#endif /*KERNEL_H*/