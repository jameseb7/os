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

uint32_t page_directory[1024] __attribute__ ((aligned(4096)));
uint32_t first_page_table[1024] __attribute__ ((aligned(4096)));
uint32_t * page_stack = (uint32_t *) 0x00000000;

extern uint32_t magic;
extern multiboot_data * mbd;

void index_pages(){
  mmap * m = (mmap*) (mbd->mmap_addr);
  mmap * mmap_end = (mmap*) ((mbd->mmap_addr) + (mbd->mmap_length));
  uint32_t * page = (uint32_t *) 0x00000000;

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
  for(; m < mmap_end; m = (mmap*) (((uint32_t) m) + m->size + 4)){
    kprint(uint32_to_hex_string((uint32_t) m));
    kprint(": ");
    kprint(uint32_to_hex_string(m->size));
    kprint(" ");
    kprint(uint64_to_hex_string(m->base_addr));
    kprint(" ");
    kprint(uint64_to_hex_string(m->length));
    kprint(" ");
    kprintln(uint32_to_hex_string(m->type));
    
    /*skip unusable blocks*/
    if(m->type != 1) continue;
    
    /*skip blocks entirely below 2 MiB*/
    if((m->base_addr + m->length) < (2 << 20)) continue;

    /*skip blocks above 4 GiB (32-bit addressable memory)*/
    if(m->base_addr > ((uint64_t) 1 << 32)) continue;

    /*if a block begins below 2 MiB, consider only the part above 2 MiB,
     otherwise start indexing at the next page boundary after base_addr*/
    if(m->base_addr < (2 << 10)){
      page = (uint32_t *) (2 << 20);
    }else{
      if((m->base_addr & 0x00000FFF) == 0){
	page = (uint32_t *) ((uint32_t) m->base_addr & 0xFFFFFFFF);
      }else{
	page = (uint32_t *) (((uint32_t)  m->base_addr & 0xFFFFF000) + (1 << 12));
      }
    }

    /*push each page of the memory block onto the page stack*/
    for(; ((uint32_t) page < (m->base_addr + m->length)) && (page != 0) /*avoid 32-bit wraparound*/; page += (1 << 12)){
      *page = (uint32_t) page_stack;
      page_stack = page;
    }

  }
  kprintln(uint32_to_hex_string((uint32_t) m));
}
   

void make_page_directory(){
   int i;

   page_directory[0] = ((uint32_t) first_page_table) | PDE_PRESENT | PDE_WRITEABLE;
   for(i = 1; i < 1023; i++) page_directory[i] = 0x00000000;
   page_directory[1023] = ((uint32_t) page_directory) | PDE_PRESENT | PDE_WRITEABLE;

   /*set up identity paging for the first two megabytes*/
   for(i = 0; i < 512; i++) first_page_table[i] = (((uint32_t) i) << 12) | PTE_PRESENT | PTE_WRITEABLE;
}

 







