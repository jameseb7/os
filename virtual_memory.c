#include <stdint.h>
#include "kernel.h"

/* structure for list of unused memory allocator records */
struct free_record{
  struct free_record * next;
  struct free_record * zero;
};

/* structure for storing information about a block of virtual pages */
struct page_record{
  uint8_t * address_size;
  struct page_record * next;
};

/* structure for storing page records in a tree */
struct tree_record{
    struct tree_record * left;
    struct tree_record * right;
};

struct free_record * free_allocator_records = (struct free_record *) 0x00008000;
struct page_record * free_page_list_by_size[32];
struct tree_record * allocated_pages_by_address = (struct tree_record *) 0x00000000;
struct tree_record * free_pages_by_address = (struct tree_record *) 0x00000000;

void setup_virtual_page_allocator(){
  struct free_record * current_record = (struct free_record *) 0x00008000;
  struct free_record * next_record = (struct free_record *) 0x00008008;
  int i;
  
  free_allocator_records->next = 0x00000000;
  free_allocator_records->zero = 0x00000000;

  for(; next_record < (struct free_record *) 0x00010000; next_record += 8, current_record += 8){
    current_record->next = next_record;
    next_record->next = 0x00000000;
    next_record->zero = 0x00000000;
  }

  for(i = 0; i < 32; i++){
    free_page_list_by_size[i] = 0x00000000;
  }


  /* TODO: Add code to create intial page blocks */
}
  
