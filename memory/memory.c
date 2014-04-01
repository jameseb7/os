#include <stdint.h>

#include "memory.h"


void setup_gdt(void);
void index_pages(void);
void init_virtual_page_allocator(void);
uint8_t * make_page_directory_nopaging(void);
void enable_paging(void);

void memory_init(){
  setup_gdt();
  
  index_pages();
  init_virtual_page_allocator();
  
  load_page_directory(make_page_directory_nopaging());
  enable_paging();  
}
