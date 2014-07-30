#include <stdint.h>
#include <stddef.h>

#include "kutil.h"

struct block_header{
	uint32_t magic_word;
	uint32_t type;
	uint32_t size;
	struct block_header * prev_block;
};

struct free_block_header{
	uint32_t magic_word;
	uint32_t type;
	uint32_t size;
	struct block_header * prev_block;
	struct free_block_header * left;
	struct free_block_header * right;
};

#define MAGIC 0x4D454D4B
#define FREE_BLOCK 0x46000000
#define USED_BLOCK 0x55000000

struct free_block_header * free_blocks_by_size;
struct block_header * blocks_by_address;

struct free_block_header * allocate_in_tree(struct free_block_header *,
											uint32_t,
											struct block_header **,
											struct free_block_header **);
void insert_to_tree(struct free_block_header **, struct free_block_header *);
void kalloc_init(void);

void kalloc_init(){
	/*kernel memory between 0x00007E00 and 0x0007FFFF 
	  is guaranteed to be available for use*/
	struct free_block_header * memory_start = 
		(struct free_block_header *) 0x00007E00;
	memory_start->magic_word = MAGIC;
	memory_start->type = FREE_BLOCK;
	memory_start->size = 0x0007FFFF - 0x00007E00;
	memory_start->prev_block = NULL;
	memory_start->left = NULL;
	memory_start->right = NULL;

	free_blocks_by_size = memory_start;
	blocks_by_address = (struct block_header *) memory_start;
}

void * kalloc(uint32_t size) {
	struct block_header * allocated_block = NULL;
	struct free_block_header * spare_block = NULL;
	
	size += sizeof(struct block_header);
	if(size < sizeof(struct free_block_header)){
		size = sizeof(struct free_block_header);
	}

	free_blocks_by_size = allocate_in_tree
		(free_blocks_by_size, size, &allocated_block, &spare_block);

	if(spare_block != NULL){
		insert_to_tree(&free_blocks_by_size, spare_block);
	}
	if(allocated_block == NULL){
		return NULL;
	}

	if(allocated_block->magic_word != MAGIC){
		error("kalloc: bad magic number on allocated block");
	}

	return (uint8_t *) allocated_block + sizeof(struct block_header);
}

struct free_block_header * allocate_in_tree(
	struct free_block_header * root,
	uint32_t size,
	struct block_header ** allocated_block,
	struct free_block_header ** spare_block){

	if(root == NULL){
		*allocated_block = NULL;
		return NULL;
	}
	
	if(root->magic_word != MAGIC){
		error("kalloc: bad magic number in free block tree");
	}

	if(root->size > size){ 
		if(root->left != NULL){
			/*recurse down the left branch of the tree*/
			root->left = allocate_in_tree
				(root->left, size, allocated_block, spare_block);
		}else{
			/*need a smaller block so split the current block*/
			*allocated_block = 
				(struct block_header *) ((uint8_t *) root + root->size - size);
			root->size -= size;

			(*allocated_block)->magic_word = MAGIC;
			(*allocated_block)->type = USED_BLOCK;
			(*allocated_block)->size = size;
			(*allocated_block)->prev_block = (struct block_header *) root;
			
			/*remove root as it might be in the wrong place now*/
			*spare_block = root;
			root = root->right;
			/*no left branch to handle*/
		}
	}else if(root->size < size){
		if(root->right != NULL){
			/*recurse down the left branch of the tree*/
			root->right = allocate_in_tree
				(root->right, size, allocated_block, spare_block);
		}else{
			/*need a bigger block but there's no right branch so can't allocate*/
			*allocated_block = NULL;
		}
	}else{
		/*the current block is the right size so use that*/
		
		*allocated_block = (struct block_header *) root;
		if(root->left == NULL){
			root = root->right;
		}else if(root->right == NULL){
			root = root->left;
		}else{
			insert_to_tree(&(root->right), root->left);
			root = root->right;
		}

		(*allocated_block)->type |= USED_BLOCK;
	}

	return root;
}

void insert_to_tree(struct free_block_header ** root_ptr, 
					struct free_block_header * to_insert){
	struct free_block_header * root;
	
	if(*root_ptr == NULL){
		*root_ptr = to_insert;
		return;
	}
	root = *root_ptr;

	if(root->size > to_insert->size){ 
		if(root->left != NULL){
			insert_to_tree(&(root->left), to_insert);
		}else{
			root->left = to_insert;
		}
	}else{
		if(root->right != NULL){
			insert_to_tree(&(root->right), to_insert);
		}else{
			root->right = to_insert;
		}
	}
}
