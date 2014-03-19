#include <stdint.h>
#include "kernel.h"

extern uint32_t OS_end;
static uint32_t low_pages_end;
static uint32_t high_pages_start;

void init_virtual_page_allocator(){
  low_pages_end = (uint32_t) &OS_end;
  if((low_pages_end & 0xFFF) != 0){
    low_pages_end = low_pages_end & 0xFFFFF000;
    low_pages_end += 1 << 12;
  }
  
  high_pages_start = (uint32_t) 1023 << 22;
  high_pages_start -= 1 << 12;
}

//allocate near the start of memory for program code
void * allocate_virtual_pages_low(uint32_t size){
  uint32_t page_address = low_pages_end;
  uint32_t allocation_start = low_pages_end;
  
  for( ; size > 0 ; (page_address += 0x1000), (size -= 0x1000)){
    allocate_physical_page(page_address);
  }

  low_pages_end = page_address;
  return (void *) allocation_start;
}

//allocate near the end of memory for stacks and heap memory
void * allocate_virtual_pages_high(uint32_t size){
  uint32_t page_address = high_pages_start;
  
  for(; size > 0 ; page_address -= 0x1000, size -= 0x1000){
    allocate_physical_page(page_address);
  }

  high_pages_start = page_address;
  page_address += 1 << 12;
  return (void *) page_address;
}
