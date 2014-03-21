#ifndef MEMORY_H
#define MEMORY_H

void setup_gdt(void);
void index_pages(void);
uint8_t * make_page_directory(void);
void init_virtual_page_allocator(void);

void allocate_physical_page(uint32_t);
void free_physical_page(uint32_t);
void * allocate_virtual_pages_high(uint32_t);
void * allocate_virtual_pages_low(uint32_t);

#endif
