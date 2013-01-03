#include <stdint.h>
#include "multiboot.h"
#include "kernel.h"

#define PDE_PRESENT       1 << 0
#define PDE_WRITEABLE     1 << 1
#define PDE_USER          1 << 2
#define PDE_WRITE_THROUGH 1 << 3
#define PDE_CACHE_DISABLE 1 << 4
#define PDE_ACCESSED      1 << 5
#define PDE_LARGE_PAGES   1 << 7

#define PTE_PRESENT       1 << 0
#define PTE_WRITEABLE     1 << 1
#define PTE_USER          1 << 2
#define PTE_WRITE_THROUGH 1 << 3
#define PTE_CACHE_DISABLE 1 << 4
#define PTE_ACCESSED      1 << 5
#define PTE_DIRTY         1 << 6
#define PTE_GLOBAL        1 << 8

uint32_t * page_directory = (uint32_t *) 0x00001000;

extern uint32_t magic;
extern multiboot_data * mbd;

void index_pages(){
  mmap * m = (mmap*) (mbd->mmap_addr);
  mmap * mmap_end = (mmap*) ((mbd->mmap_addr) + (mbd->mmap_length));

  if(magic != 0x2BADB002){
    kprint("ERROR: Invalid multiboot magic number: ");
    kprintln(uint32_to_hex_string(magic));
    return;
  }
  if((mbd->flags & (1 << 6)) == 0){
    kprintln("ERROR: Memory map not present");
    return;
  }

  clear_screen();

  kprintln(uint32_to_hex_string(mbd->mmap_length));
  kprintln(uint32_to_hex_string(mbd->mmap_addr));
  kprintln(uint32_to_hex_string((uint32_t) m));
  kprintln(uint32_to_hex_string((uint32_t) mmap_end));
  while(m < mmap_end){
    kprint(uint32_to_hex_string((uint32_t) m));
    kprint(": ");
    kprint(uint32_to_hex_string(m->size));
    kprint(" ");
    kprint(uint64_to_hex_string(m->base_addr));
    kprint(" ");
    kprint(uint64_to_hex_string(m->length));
    kprint(" ");
    kprintln(uint32_to_hex_string(m->type));
    m = (mmap*) (((uint32_t) m) + m->size + 4);
  }
  kprintln(uint32_to_hex_string((uint32_t) m));
}
   

void make_page_directory(){
   int i;
   uint32_t * page_table = (uint32_t *) 0x00002000;

   page_directory[0] = ((uint32_t)page_table) | PDE_PRESENT | PDE_WRITEABLE;
   for(i = 1; i < 1023; i++) page_directory[i] = 0x00000000;
   page_directory[1023] = ((uint32_t) page_directory) | PDE_PRESENT | PDE_WRITEABLE;

   /*set up identity paging for the first two megabytes*/
   for(i = 0; i < 512; i++) page_table[i] = (((uint32_t) i) << 12) | PTE_PRESENT | PTE_WRITEABLE;
}

 







