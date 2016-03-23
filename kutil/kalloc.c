#include <stdint.h>
#include <stddef.h>

#include "kutil.h"

struct block_header{
	uint32_t magic;
	uint32_t size_type;
};

struct free_block_header{
	uint32_t magic;
	uint32_t size_type;
	struct free_block_header * next;
	struct free_block_header * prev;
};

#define MAGIC 0x4D454D4B
#define FREE_BLOCK 0x46000000
#define USED_BLOCK 0x55000000
#define SIZE(x) ((uint32_t) ((x)->size_type & 0x000000FF))
#define TYPE(x) ((uint32_t) ((x)->size_type & 0xFF000000))
#define BUDDY(x) ((struct free_block_header *) \
				  (((uint32_t) x) ^ ((uint32_t) 1 << SIZE(x))))

struct free_block_header * free_blocks[20];

void kalloc_init(void);
char parse_nybble(uint8_t);
#define NYBBLES(x,y) (((uint32_t) parse_nybble(x) << 8) | ((uint32_t) parse_nybble(y) << 16))

void kalloc_init(){
	/*kernel memory between 0x00007E00 and 0x0007FFFF 
	  is guaranteed to be available for use*/
	struct free_block_header * memory_start = 
		(struct free_block_header *) 0x00040000; /* use 256 KiB at the end of kernel memory */
	memory_start->magic = MAGIC;
	memory_start->size_type = FREE_BLOCK | NYBBLES(1,8) | 18;
	memory_start->next = NULL;
	memory_start->prev = NULL;

	free_blocks[18] = memory_start;
}

void * kalloc(uint32_t size) {
    int log_size = 0;
	int i;
	struct free_block_header * free_block;
	struct free_block_header * spare_block;

	/*make sure the size is big enough*/
	size += sizeof(struct block_header);

	/*take the base 2 logarithm of the size, rounding up*/
	for(; size > ((uint32_t) (1 << log_size)); log_size++);

	/*find a free block that's large enough*/
	for(i = log_size; i < 20; i++){
		if((free_blocks[i] != NULL) && 
		   (SIZE(free_blocks[i]) == ((uint32_t) i))){
			break;
		}
	}
	if(i > 19){
		/*no block large enough*/
		return NULL; 
	}


	free_block = free_blocks[i];
	free_blocks[i]->prev = NULL;
	free_blocks[i] = free_blocks[i]->next;

	/*split the block if necessary*/
	while(SIZE(free_block) > ((uint32_t) log_size)){
		free_block->size_type = 
			FREE_BLOCK |
			NYBBLES((uint8_t) (((SIZE(free_block) - 1) % 10) - (SIZE(free_block) - 1)),
					(uint8_t) (SIZE(free_block) - 1) % 10) |
			(SIZE(free_block) - 1);
		spare_block = BUDDY(free_block);
		spare_block->magic = MAGIC;
		spare_block->size_type = 
			FREE_BLOCK |
			NYBBLES((uint8_t) ((SIZE(free_block) % 10) - SIZE(free_block)),
				(uint8_t) (SIZE(free_block) % 10)) |
			SIZE(free_block);
		spare_block->next = free_blocks[SIZE(free_block)];
		free_blocks[SIZE(free_block)]->prev = spare_block;
		free_blocks[SIZE(free_block)] = spare_block;
	}

	free_block->size_type &= 0x00FFFFFF;
	free_block->size_type |= USED_BLOCK;
	return (void *) ((uint32_t) free_block + sizeof(struct block_header));
	
}

void kfree(void * ptr){
	struct free_block_header * free_block = (void *) ((uint32_t) ptr - 8);
	struct free_block_header * buddy_block = NULL;
	unsigned int size;

	if(free_block->magic != MAGIC){
		error("Invalid block magic number in kfree");
	}

	free_block->size_type &= 0x00FFFFFF;
	free_block->size_type |= FREE_BLOCK;

	size = SIZE(free_block);

	//put the free block in the list of free blocks
	free_block->next = free_blocks[size];
	free_block->prev = NULL;
	free_blocks[size] = free_block;

	//for each buddy that's also free, merge them
	while((BUDDY(free_block)->magic == MAGIC) &&
		  (SIZE(BUDDY(free_block)) == SIZE(free_block)) &&
		  (TYPE(BUDDY(free_block)) == FREE_BLOCK)){
		
		//make sure the second block is treated as the buddy block
		if(BUDDY(free_block) > free_block){
			buddy_block = BUDDY(free_block);
		}else{
			buddy_block = free_block;
			free_block = BUDDY(free_block);
		}

		size = SIZE(free_block);

		//remove the buddy block from the allocator as it's being merged
		buddy_block->magic = 0;
		buddy_block->size_type = 0;
		if(buddy_block->next != NULL){
			buddy_block->next->prev = buddy_block->prev;
		}
		if(buddy_block->prev != NULL){
			buddy_block->prev->next = buddy_block->next;
		}

		//increase the size of the free block
		size++;
		free_block->size_type = 
			FREE_BLOCK |
		  NYBBLES((uint8_t) ((size % 10) - size), (uint8_t) (size % 10)) |
			size;
		
		//remove the free block from the the old list of free blocks
		if(free_block->next != NULL){
			free_block->next->prev = free_block->prev;
		}
		if(free_block->prev != NULL){
			free_block->prev->next = free_block->next;
		}else{
			free_blocks[size-1] = free_blocks[size-1]->next;
		}

		//put the free block in the new list of free blocks
		free_block->next = free_blocks[size];
		free_block->prev = NULL;
		free_blocks[size] = free_block;
	}
}
