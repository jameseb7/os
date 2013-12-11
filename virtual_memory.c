#include <stdint.h>
#include "kernel.h"

#define FLAG_USED_RECORD 1 << 0
#define FLAG_PAGE_RECORD 1 << 1
#define FLAG_USED_PAGES  1 << 2

#define PAGE_ADDRESS(X) ((uint32_t) (X) & 0xFFFFF000)
#define RECORD_ADDRESS(X) ((uint32_t) (X) & 0xFFFFFFF8)
#define PAGE_SIZE(X) (((uint32_t) (X) & 0x00000FFF) >> (uint32_t) 3)

/*structure for list of unused memory allocator records*/
struct free_record{
  struct free_record * next;
  uint32_t unused1;
  uint32_t unused2;
  uint32_t unused3;
};

/*structure for storing information about a block of virtual pages*/
struct page_record{
  uint32_t flags; /*(log(size)-12) 7-3, flags 0-2*/
  uint32_t address;
  struct page_record * prev;
  struct page_record * next;
};

/* structure for storing page records in a tree */
struct tree_record{
  uint32_t flags; /*tree level 7-3 flags 0-2*/
  struct tree_record * parent;
  struct tree_record * left;
  struct tree_record * right;
};

void add_to_tree(struct page_record *, struct tree_record *);
void free_from_tree(uint32_t, struct tree_record *, int);

struct free_record * free_records = (struct free_record *) 0x00008000;
struct page_record * free_pages_by_size[20];
struct tree_record * pages_by_address;

void setup_virtual_page_allocator(){
  struct free_record * current_free_record = (struct free_record *) 0x00008000;
  struct free_record * next_free_record = (struct free_record *) 0x00008008;
  struct page_record * current_page_record;
  struct page_record * next_page_record;
  struct tree_record * current_tree_record;
  uint32_t address;
  unsigned int i;

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

  for(i = 0; i < 20; i++){
    free_pages_by_size[i] = (struct page_record *) 0x00000000;
  }

  /*pop a record from the free records and record the allocatable space in the second 2MiB*/
  free_pages_by_size[9] = (struct page_record *) free_records;
  free_records = free_records->next;
  free_pages_by_size[9]-flags = (9 << 3) | FLAG_PAGE_RECORD | FLAG_USED_RECORD;
  free_pages_by_size[9]->address_size = 0x00200000;
  free_pages_by_size[9]->prev = 0;
  free_pages_by_size[9]->next = 0;

  /*divide the rest of the memory into 4MiB chunks for allocation*/
  free_pages_by_size[10] = (struct page_record *) free_records;
  free_records = free_records->next;
  current_page_record = free_pages_by_size[10];
  current_page_record->prev = 0;
  for(address = (1 << 22); address < (uint32_t) (1022 << 22); address += (1 << 22)){
    current_page_record->flags = (10 << 3) | FLAG_PAGE_RECORD | FLAG_USED_RECORD;
    current_page_record->address = address;

    next_page_record = (struct page_record *) free_records;
    free_records = free_records->next;
    current_page_record->next = next_page_record;
    next_page_record->prev = current_page_record;

    current_page_record = next_page_record;
  }
  current_page_record->flags = (10 << 3) | FLAG_PAGE_RECORD | FLAG_USED_RECORD;
  current_page_record->address = address;
  current_page_record->next = 0;
  
  /*make root of tree by address*/
  pages_by_address = (struct tree_record *) free_records;
  free_records = free_records->next;
  current_tree_record = pages_by_address;
  current_tree_record->flags = (31 << 3) | FLAG_USED_RECORD;
  current_tree_record->parent = (struct tree_record *) 0;
  current_tree_record->left = (struct tree_record *) 0;
  current_tree_record->right = (struct tree_record *) 0;

  /*put the unallocated record list of size 10 into the tree by address*/
  for(current_page_record = free_pages_by_size[10]; current_page_record != 0; current_page_record = current_page_record->next){
    add_to_tree(current_page_record, pages_by_address);
  }

  /*put the unallocated record list of size 9 into the tree by address*/
  current_page_record = free_pages_by_size[9];
  add_to_tree(current_page_record, pages_by_address);
}

void add_to_tree(struct page_record * page_block, struct tree_record * tree_root){
  struct tree_record * current_tree_record = tree_root;
  uint32_t address = PAGE_ADDRESS(page_block->address_size);
  uint32_t size = PAGE_SIZE(page_block->address_size) + 12;
  unsigned int level;

  for(level = 31; level > size; level--){
    if(((address >> level) % 2) == 0){
      /* kprint("left "); */
      /*look at the left subtree*/
      if((uint32_t) current_tree_record->left == 0){
	/*make nonexistent left branch*/
	if(free_records == 0){
	  kprintln("ERROR: NO FREE RECORDS LEFT");
	  halt();
	}
	current_tree_record->left = (struct tree_record *) free_records;
	free_records = free_records->next;

	current_tree_record->left->flags = ((level-1) << 3) | FLAG_USED_RECORD;
	current_tree_record->left->parent = current_tree_record;
	current_tree_record->left->left = (struct tree_record *) 0;
	current_tree_record->left->right = (struct tree_record *) 0;
	
	current_tree_record = (struct tree_record *) current_tree_record->left;	
      }else{
	current_tree_record = (struct tree_record *) current_tree_record->left;
      }
    }else{
      /* kprint("right "); */
      /*look at the right subtree*/
      if(current_tree_record->right == 0){
	/*make nonexistent right branch*/
	if(free_records == 0){
	  kprintln("ERROR: NO FREE RECORDS LEFT");
	  halt();
	}
	current_tree_record->right = (struct tree_record *) free_records;
	free_records = free_records->next;

	current_tree_record->right->flags = ((level-1) << 3) | FLAG_USED_RECORD;
	current_tree_record->right->parent = current_tree_record;
	current_tree_record->right->left = (struct tree_record *) 0;
	current_tree_record->right->right = (struct tree_record *) 0;

	current_tree_record = current_tree_record->right;
      }else{
	current_tree_record = current_tree_record->right;
      }
    }
  }
  
  if(((address >> level) % 2) == 0){
    current_tree_record->left = (struct tree_record *) page_block;
  }else{
    current_tree_record->right = (struct tree_record *) page_block;
  }
}

void * allocate_virtual_pages(int number_of_pages){
  int i;
  int shift_number, larger_shift;
  uint32_t address;
  struct page_record * page_block;
  struct page_record * spare_page_block;
  struct tree_record * tree_node;

  /*can't allocate bigger pages than the number of lists allows*/
  if(number_of_pages > (1 << 19)){
    kprintln("ERROR: NUMBER OF PAGES TOO BIG");
    return 0;
  }

  /*can't allocate non-positive numbers of pages*/
  if(number_of_pages <= 0){
    kprintln("ERROR: NUMBER OF PAGES MUST BE POSITIVE");
    return 0;
  }

  for(shift_number = 0; ((number_of_pages >> shift_number) % 2) == 0; shift_number++);

  /*if number_of_pages isn't a power of two, go to a page block of larger size*/
  if((number_of_pages >> shift_number) != 1){
    for(; (number_of_pages >> shift_number) != 1; shift_number++);
    shift_number++;
  }

  /*check if there is a free page block of the correct size*/
  if(free_pages_by_size[shift_number] != 0){
    /*if there is, pop it from the list*/
    page_block = free_pages_by_size[shift_number];
    free_pages_by_size[shift_number] = free_pages_by_size[shift_number]->next;
    free_pages_by_size[shift_number]->prev = 0;

    /*mark the page block as used*/
    page_block->flags = page_block->flags | FLAG_USED_PAGES;
  }else{
    /*if there isn't a block of the correct size, search for a larger block that could be broken up*/
    for(larger_shift = shift_number+1; larger_shift < 20; larger_shift++){
      if(free_pages_by_size[larger_shift] != 0) break;
    }
    if(larger_shift == 20) return 0; /*return a null pointer if no such block exists*/

    /*remove this block from the free block list*/
    page_block = free_pages_by_size[larger_shift];
    free_pages_by_size[larger_shift] = free_pages_by_size[larger_shift]->next;
    free_pages_by_size[larger_shift]->prev = 0;

    /*break up the block and put the spare blocks in the tree*/
    for(i = larger_shift; i > shift_number; i--){
      /*change the page block record into a tree record and store the address_size*/
      tree_node = (struct tree_record *) page_block;
      address = page_block->address_size;

      /*put new records on each branch of the tree node*/
      if(free_records->next == 0){
	kprintln("ERROR: NO FREE RECORDS LEFT");
	halt();
      }
      tree_node->left = (struct tree_record *) ((uint32_t) free_records | FLAG_USED_RECORD);
      free_records = free_records->next;
      tree_node->right = (struct tree_record *) free_records;
      free_records = free_records->next;

      /*check which branch the page bock needs to be on*/
      if(((PAGE_ADDRESS(address) >> (12+i-1)) % 2) == 0){
	/*put the page block on the left branch and spare page block on the right*/
	page_block = (struct page_record *) RECORD_ADDRESS(tree_node->left);
	page_block->address_size = address;
	spare_page_block = (struct page_record *) RECORD_ADDRESS(tree_node->right);
      }else{
	/*put the page block on the right branch and spare page block on the left*/
	page_block = (struct page_record *) RECORD_ADDRESS(tree_node->right);
	page_block->address_size = address;
	spare_page_block = (struct page_record *) RECORD_ADDRESS(tree_node->left);
      }
	
      /*set up the spare page block as the second half of the current page block*/
      spare_page_block->address_size = (PAGE_ADDRESS(page_block->address_size) + (uint32_t) (1 << (i-1+12))) 
	| (uint32_t) ((i-1) << 3) | FLAG_PAGE_RECORD | FLAG_USED_RECORD;
      spare_page_block->next = free_pages_by_size[i-1];
      free_pages_by_size[i-1] = spare_page_block;

      /*reduce the page block's size by half*/
      page_block->address_size = (page_block->address_size & 0xFFFFF007) | (uint32_t) ((i-1) << 3);
    }

    /*mark the page block as used*/
    page_block->address_size = page_block->address_size | FLAG_USED_PAGES;
  }
  
  /*allocate the physical pages to the virtual pages*/
  for(address = PAGE_ADDRESS(page_block->address_size);
      address < (PAGE_ADDRESS(page_block->address_size) + ((uint32_t) 1 << (PAGE_SIZE(page_block->address_size)+12)));
      address += (1 << 12)){
    allocate_physical_page(address);
  }

  /*return the address of the block*/
  return (void *) PAGE_ADDRESS(page_block->address_size);
}

void free_virtual_pages(void * address){
  free_from_tree((uint32_t) address, pages_by_address, 31);
}

void free_from_tree(uint32_t address, struct tree_record * tree, int level){
  struct page_record * pages = (struct page_record *) tree;
  /*check if a page record has been found*/
  if(tree->left & FLAG_PAGE_RECORD){
    /*if it has, mark it as free*/
    pages->address_size = pages->address_size & (!FLAG_USED_PAGES);

    /*push it onto the correct stack*/
    pages->next = free_pages_by_size[level-12];
    free_pages_by_size[level-12] = pages;

    return;
  }

  if(((address >> level) % 2) == 0){
    /*check left subtree*/
    free_from_tree(address, RECORD_ADDRESS(tree->left), level-1);
  }else{
    /*check right subtree*/
    free_from_tree(address, tree->right, level-1);
  }

  /*try to combine the pages if there are two adjacent free pages*/
  if((RECORD_ADDRESS(tree->left)->left & (FLAG_PAGE_RECORD | FLAG_USED_PAGES))
     && ((tree->right)->left & (FLAG_PAGE_RECORD | FLAG_USED_PAGES))){
    /**/
  }
}
