#ifndef MEMORY_H
#define MEMORY_H

void memory_init(void);

uint8_t * make_page_directory(void);
void load_page_directory(uint8_t *);

void allocate_physical_page(uint32_t);
void free_physical_page(uint32_t);
void * allocate_virtual_pages_high(uint32_t);
void * allocate_virtual_pages_low(uint32_t);

#endif /* MEMORY_H */
