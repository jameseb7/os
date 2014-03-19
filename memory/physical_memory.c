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

extern uint32_t OS_end;

void index_pages(){
  mmap * m = (mmap*) (mbd->mmap_addr);
  mmap * mmap_end = (mmap*) ((mbd->mmap_addr) + (mbd->mmap_length));
  uint32_t * page = (uint32_t *) 0x00000000;

  if(magic != 0x2BADB002){
    kprint("ERROR: Invalid multiboot magic number: ");
    kprintln_uint32(magic);
    return;
  }
  if((mbd->flags & (1 << 6)) == 0){
    kprintln("ERROR: Memory map not present");
    return;
  }

  for(; m < mmap_end; m = (mmap*) (((uint32_t) m) + m->size + 4)){
    
    /*skip unusable blocks*/
    if(m->type != 1) continue;
    
    /*skip blocks that are below the end of the OS*/
    if((m->base_addr + m->length) < (uint32_t) &OS_end) continue;

    /*skip blocks above 4 GiB (32-bit addressable memory)*/
    if(m->base_addr > ((uint64_t) 1 << 32)) continue;

    /*if a block begins below 2 MiB, consider only the part above 2 MiB,
     otherwise start indexing at the next page boundary after base_addr*/
    if(m->base_addr < (uint32_t) &OS_end){
      page = (uint32_t *) &OS_end;
      if(((uint32_t) page & 0xFFF) != 0){
        page = (uint32_t *) ((uint32_t) page & 0xFFFFF000);
        page += 1 << 12;
      }
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
}

void allocate_physical_page(uint32_t virtual_page_address){
  uint32_t page_directory_index = (virtual_page_address & 0xFFC00000) >> 22;
  uint32_t page_table_index = (virtual_page_address & 0x003FF000) >> 12;
  uint32_t * page_table = (uint32_t *) (((uint32_t) 1023 << 22) | (page_directory_index << 12));
  int i;

  /*check there are pages in the page stack*/
  if(page_stack == 0){
    /*TODO: add code to handle having no pages left*/
    kprintln("ERROR: PAGE STACK EMPTY");
    halt();
    return;
  }

  /*make a page table if there isn't one*/
  if((page_directory[page_directory_index] & PDE_PRESENT) == 0){
    page_directory[page_directory_index] = (uint32_t) page_stack | PDE_PRESENT | PDE_WRITEABLE;
    page_stack = (uint32_t *) *page_table;

    for(i = 0; i < 1024; i++){
      page_table[i] = 0x00000000;
    }
  }

  page_table[page_table_index] = ((uint32_t) page_stack) | PTE_PRESENT | PTE_WRITEABLE;
  page_stack = (uint32_t *) *((uint32_t *) (virtual_page_address & 0xFFFFF000));  
}

void free_physical_page(uint32_t virtual_page_address){
  uint32_t page_directory_index = (virtual_page_address & 0xFFC00000) >> 22;
  uint32_t page_table_index = (virtual_page_address & 0x003FF000) >> 12;
  uint32_t * page_table = (uint32_t *) (((uint32_t) 1023 << 22) | (page_directory_index << 12));

  /*check the page is allocated*/
  if((page_directory[page_directory_index] & PDE_PRESENT) == 0){
    kprintln("PAGE TABLE NOT PRESENT");
    return;
  }
  if((page_table[page_table_index] & PTE_PRESENT) == 0){
    kprintln("PAGE NOT PRESENT");
    return;
  }

  *((uint32_t *) (virtual_page_address & 0xFFFFF000)) = (uint32_t) page_stack;
  page_stack = (uint32_t *) (virtual_page_address & 0xFFFFF000);
  page_table[page_table_index] = 0x00000000;
}

uint8_t * make_page_directory(){
   uint32_t i;

   page_directory[0] = ((uint32_t) &first_page_table) | PDE_PRESENT | PDE_WRITEABLE;
   for(i = 1; i < 1023; i++) page_directory[i] = 0x00000000;
   page_directory[1023] = ((uint32_t) &page_directory) | PDE_PRESENT | PDE_WRITEABLE;

   /*set up identity paging up to the end of the OS*/
   for(i = 0; i*(1 << 12) < (uint32_t) &OS_end; i++) first_page_table[i] = (((uint32_t) i) << 12) | PTE_PRESENT | PTE_WRITEABLE;
   for(; i < 1024; i++) first_page_table[i] = 0x00000000;

   return (uint8_t *) page_directory;
}

 







