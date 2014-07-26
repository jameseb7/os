#include "processes.h"
#include "kutil.h"

struct process{
	uint32_t page_table;
	uint16_t prev;
	uint16_t next;
	uint16_t parent;
	uint16_t children;
	uint16_t prev_child;
	uint16_t next_child;
	uint32_t reserved_1;
	uint32_t reserved_2;
	uint32_t reserved_3;
	uint32_t reserved_4;
};

#define MAX_PROCESSES (1 << 16)

struct process process_table[MAX_PROCESSES];

void processes_init(){
	int i;
	uint32_t current_page_table;

	for(i = 0; i < MAX_PROCESSES; i++){
		process_table[i].page_table = 0;
		process_table[i].prev = 0;
		process_table[i].next = 0;
		process_table[i].parent = 0;
		process_table[i].children = 0;
		process_table[i].prev_child = 0;
		process_table[i].next_child = 0;
	}

	/*make the current process the first process*/
	__asm__("movl %%cr4, %0": "=r"(current_page_table));
	process_table[0].page_table = current_page_table;
	process_table[0].prev = 0;
	process_table[0].next = 0;
	process_table[0].parent = 0;
	process_table[0].children = 0;
	process_table[0].prev_child = 0;
	process_table[0].next_child = 0;
}
