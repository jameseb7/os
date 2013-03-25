#include <stdint.h>
#include "kernel.h"

#define FLAG_USED_RECORD 1 << 0
#define FLAG_PAGE_RECORD 1 << 1
#define FLAG_USED_PAGES  1 << 2

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

struct free_record * free_records = (struct free_record *) 0x00008000;
struct page_record * free_pages_by_size[20];
struct tree_record * allocated_pages_by_address = (struct tree_record *) 0x00000000;
struct tree_record * free_pages_by_address = (struct tree_record *) 0x00000000;

void setup_virtual_page_allocator(){
  struct free_record * current_free_record = (struct free_record *) 0x00008000;
  struct free_record * next_free_record = (struct free_record *) 0x00008008;
  struct page_record * current_page_record;
  struct page_record * next_page_record;
  struct tree_record * current_tree_record;
  uint32_t address;
  unsigned int i;
  
  clear_screen();
  kprintln("VIRTUAL PAGE ALLOCATOR STARTED");

  /*divide two pages starting at 0x00008000 into 64-bit records and push them onto the free record stack*/
  free_records->next = 0x00000000;
  free_records->zero = 0x00000000;
  while((uint32_t) next_free_record < 0x00010000){
    current_free_record->next = next_free_record;
    next_free_record->next = 0x00000000;
    next_free_record->zero = 0x00000000;
    
    current_free_record++;
    next_free_record++;
  }

  kprintln("FREE RECORD STACK MADE");

  for(i = 0; i < 20; i++){
    free_pages_by_size[i] = (struct page_record *) 0x00000000;
  }

  kprintln(uint32_to_hex_string((uint32_t) free_records));
  kprintln(uint32_to_hex_string((uint32_t) free_records->next));

  /*pop a record from the free records and record the allocatable space in the second 2MiB*/
  free_pages_by_size[9] = (struct page_record *) free_records;
  free_records = free_records->next;
  free_pages_by_size[9]->address_size = (uint8_t *) (0x00200000 | (9 << 3) | FLAG_PAGE_RECORD | FLAG_USED_RECORD);
  free_pages_by_size[9]->next = 0x00000000;

  kprintln(uint32_to_hex_string((uint32_t) free_records));
  kprintln(uint32_to_hex_string((uint32_t) free_records->next));

  /*divide the rest of the memory into 4MiB chunks for allocation*/
  free_pages_by_size[10] = (struct page_record *) free_records;
  free_records = free_records->next;
  current_page_record = free_pages_by_size[10]; 
  for(address = (1 << 22); address < (uint32_t) (1022 << 22) ; address += (1 << 22)){
    current_page_record->address_size = (uint8_t *) (address | (10 << 3) | FLAG_PAGE_RECORD | FLAG_USED_RECORD);

    next_page_record = (struct page_record *) free_records;
    free_records = free_records->next;
    current_page_record->next = next_page_record;

    current_page_record = next_page_record;
  }
  current_page_record->address_size = (uint8_t *) (address | (10 << 3) | FLAG_PAGE_RECORD | FLAG_USED_RECORD);
  current_page_record->next = 0x00000000;
  
  /*make root of tree by address*/
  free_pages_by_address = (struct tree_record *) free_records;
  free_records = free_records->next;
  current_tree_record = free_pages_by_address;
  current_tree_record->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
  current_tree_record->right = (struct tree_record *) 0x00000000;

  /*put the unallocated record list of size 10 into a tree by address*/
  for(current_page_record = free_pages_by_size[10]; current_page_record != 0x00000000; current_page_record = current_page_record->next){
    current_tree_record = free_pages_by_address;
    for(i = 31; i > ((((uint32_t) current_page_record->address_size & 0x00000FFF) >> 3) + 12); i--){
      if((((uint32_t) current_page_record->address_size >> i) % 2) == 0){
	/*look at the left subtree*/
	if(((uint32_t) current_tree_record->left & 0xFFFFFFF8) == 0x00000000){
	  /*make nonexistent left branch*/
	  current_tree_record->left = (struct tree_record *) free_records;
	  free_records = free_records->next;
	  current_tree_record = current_tree_record->left;
	  current_tree_record->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
	  current_tree_record->right = (struct tree_record *) 0x00000000;
	}else{
	  current_tree_record = current_tree_record->left;
	}
      }else{
	/*look at the right subtree*/
	if(((uint32_t) current_tree_record->right & 0xFFFFFFF8) == 0x00000000){
	  /*make nonexistent right branch*/
	  current_tree_record->right = (struct tree_record *) free_records;
	  free_records = free_records->next;
	  current_tree_record = current_tree_record->right;
	  current_tree_record->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
	current_tree_record->right = (struct tree_record *) 0x00000000;
	}else{
	  current_tree_record = current_tree_record->right;
	}
      } 
    }
    /*put page record at correct leaf of the tree*/
    if((((uint32_t) current_page_record->address_size >> i) % 2) == 0){
      current_tree_record->left = (struct tree_record *) current_page_record;
    }else{
      current_tree_record->right = (struct tree_record *) current_page_record;
    }
  }

  /*put the unallocated record list of size 9 into a tree by address*/
  current_page_record = free_pages_by_size[9];
  current_tree_record = free_pages_by_address;
  for(i = 31; i > ((((uint32_t) current_page_record->address_size & 0x00000FFF) >> 3) + 12); i--){
    if((((uint32_t) current_page_record->address_size >> i) % 2) == 0){
      /*look at the left subtree*/
      if(((uint32_t) current_tree_record->left & 0xFFFFFFF8) == 0x00000000){
	/*make nonexistent left branch*/
	current_tree_record->left = (struct tree_record *) free_records;
	free_records = free_records->next;
	current_tree_record = current_tree_record->left;
	current_tree_record->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
	current_tree_record->right = (struct tree_record *) 0x00000000;
      }else{
	current_tree_record = current_tree_record->left;
      }
    }else{
      /*look at the right subtree*/
      if(((uint32_t) current_tree_record->right & 0xFFFFFFF8) == 0x00000000){
	/*make nonexistent right branch*/
	  current_tree_record->right = (struct tree_record *) free_records;
	  free_records = free_records->next;
	  current_tree_record = current_tree_record->right;
	  current_tree_record->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
	  current_tree_record->right = (struct tree_record *) 0x00000000;
      }else{
	current_tree_record = current_tree_record->right;
      }
    } 
  }
  /*put page record at correct leaf of the tree*/
  if((((uint32_t) current_page_record->address_size >> i) % 2) == 0){
    current_tree_record->left = (struct tree_record *) current_page_record;
  }else{
    current_tree_record->right = (struct tree_record *) current_page_record;
  }
  
  /*make allocated record tree*/
  allocated_pages_by_address = (struct tree_record *) free_records;
  free_records = free_records->next;
  allocated_pages_by_address->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
  allocated_pages_by_address->right = (struct tree_record *) 0x00000000;
}

void * allocate_virtual_pages(int number_of_pages){
  int i, j;
  struct page_record * page_block;
  struct tree_record * current_tree_record;

  /*can't allocate bigger pages than the number of lists allows*/
  if(number_of_pages > (1 << 19)) return 0;

  /*can't allocate non-positive numbers of pages*/
  if(number_of_pages <= 0) return 0;

  for(i = 0; ((number_of_pages >> i) % 2) == 1; i++);

  /*check if number_of_pages is a power of two*/
  if((number_of_pages >> i) == 1){
    /*if it is then check if there is a free page block of that size*/
    if(free_pages_by_size[i] != 0){
      /*if there is, pop it from the list*/
      page_block = free_pages_by_size[i];
      free_pages_by_size[i] = free_pages_by_size[i]->next;
      
      /*mark the page block as used*/
      page_block->address_size = (uint8_t *) ((uint32_t) page_block->address_size | FLAG_USED_PAGES);

      /*remove the page block record from the free pages tree*/
      current_tree_record = free_pages_by_address;
      for(j = 31; j > (i + 12); j--){
	if((((uint32_t) page_block->address_size >> j) % 2) == 0){
	  /*look at the left subtree*/
	  if(((uint32_t) current_tree_record->left & 0xFFFFFFF8) == 0x00000000){
	    /*nonexistent left branch - error*/
	    return 0;
	  }else{
	    current_tree_record = current_tree_record->left;
	  }
	}else{
	  /*look at the right subtree*/
	  if(((uint32_t) current_tree_record->right & 0xFFFFFFF8) == 0x00000000){
	    /*nonexistent right branch - error*/
	    return 0;
	  }else{
	    current_tree_record = current_tree_record->right;
	  }
	}
      }
      if((((uint32_t) page_block->address_size >> (i + 12)) % 2) == 0){
	current_tree_record->left = (struct tree_record *) (0 | FLAG_USED_RECORD);
      }else{
	current_tree_record->right = (struct tree_record *) 0;
      }

      /*add the page block record to the allocated pages tree*/
      current_tree_record = allocated_pages_by_address;
      for(j = 31; j > (i + 12); j--){
	if((((uint32_t) page_block->address_size >> j) % 2) == 0){
	  /*look at the left subtree*/
	  if(((uint32_t) current_tree_record->left & 0xFFFFFFF8) == 0x00000000){
	    /*make nonexistent left branch*/
	    current_tree_record->left = (struct tree_record *) free_records;
	    free_records = free_records->next;
	    current_tree_record = current_tree_record->left;
	    current_tree_record->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
	    current_tree_record->right = (struct tree_record *) 0x00000000;
	  }else{
	    current_tree_record = current_tree_record->left;
	  }
	}else{
	  /*look at the right subtree*/
	  if(((uint32_t) current_tree_record->right & 0xFFFFFFF8) == 0x00000000){
	    /*make nonexistent right branch*/
	    current_tree_record->right = (struct tree_record *) free_records;
	    free_records = free_records->next;
	    current_tree_record = current_tree_record->right;
	    current_tree_record->left = (struct tree_record *) (0x00000000 | FLAG_USED_RECORD);
	    current_tree_record->right = (struct tree_record *) 0x00000000;
	  }else{
	    current_tree_record = current_tree_record->right;
	  }
	}
      }
      if((((uint32_t) page_block->address_size >> (i + 12)) % 2) == 0){
	current_tree_record->left = (struct tree_record *) ((uint32_t) page_block | FLAG_USED_RECORD);
      }else{
	current_tree_record->right = (struct tree_record *) page_block;
      }

      /*return the address of the block*/
      return (void *) ((uint32_t) page_block->address_size & 0xFFFFF000);

    }
  }
  
  /*TODO: add code to deal with
    having no page blocks of the correct size
    and page numbers not powers of 2*/
  return 0;
}
