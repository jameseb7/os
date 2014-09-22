#include <stdint.h>

#include "processes.h"
#include "kutil.h"

struct process{
	uint32_t page_directory;
	uint32_t stack_pointer;
	uint16_t prev;
	uint16_t next;
	uint16_t parent;
	uint16_t children;
	uint16_t prev_child;
	uint16_t next_child;
	uint32_t reserved_1;
	uint32_t reserved_2;
	uint32_t reserved_3;
};

#define MAX_PROCESSES (1 << 16)
#define NULL_PROCESS 0

//keep index 0 as a null value
struct process process_table[MAX_PROCESSES];

uint16_t process_queue_front = NULL_PROCESS;
uint16_t process_queue_back = NULL_PROCESS;
uint16_t current_process = NULL_PROCESS;

void push_to_process_queue(uint16_t);
uint16_t pop_from_process_queue(void);

extern uint32_t kernel_stack_start;


void processes_init(){
	int i;
	uint32_t current_page_table;

	for(i = 0; i < MAX_PROCESSES; i++){
		process_table[i].page_directory = 0;
		process_table[i].stack_pointer = 0;
		process_table[i].prev = 0;
		process_table[i].next = 0;
		process_table[i].parent = 0;
		process_table[i].children = 0;
		process_table[i].prev_child = 0;
		process_table[i].next_child = 0;
	}

	/*make the current process the first process*/
	__asm__("movl %%cr3, %0": "=r"(current_page_table));
	process_table[1].page_directory = current_page_table;
	process_table[1].prev = 0;
	process_table[1].next = 0;
	process_table[1].parent = 0;
	process_table[1].children = 0;
	process_table[1].prev_child = 0;
	process_table[1].next_child = 0;
}

void switch_process(uint16_t, uint16_t);
uint32_t switch_process_asm(uint32_t, uint32_t);

void switch_process(uint16_t old_process, uint16_t new_process){
	process_table[old_process].stack_pointer = 
		switch_process_asm(process_table[new_process].stack_pointer,
						   process_table[new_process].page_directory);
}

void run_next_process(){
	uint16_t old_process = current_process;
	
	push_to_process_queue(current_process);
	current_process = pop_from_process_queue();

	//stop and wait for interrupts if there is no current process
	if(current_process == NULL_PROCESS){
		kprint("idle ");
		__asm__("movl %0, %%esp" : : "r"(kernel_stack_start));
		sti();
		outb(0x20, 0x20); //send the end of interrupt signal to the PIC master
		outb(0xA0, 0x20); //send the end of interrupt signal to the PIC slave
		halt();
	}

	switch_process(old_process, current_process);
}
	

void push_to_process_queue(uint16_t process_id){
	if(process_queue_back == NULL_PROCESS){
		//empty queue so make the process the front and back
		process_queue_back = process_id;
		process_queue_front = process_id;
	}else{
		process_table[process_queue_back].next = process_id;
		process_table[process_id].prev = process_queue_back;
		process_table[process_id].next = NULL_PROCESS;
		process_queue_back = process_id;
	}
}
		
	
uint16_t pop_from_process_queue(){
	uint16_t return_value = 0;

	return_value = process_queue_front;
	process_queue_front = process_table[process_queue_front].next;
	process_table[process_queue_front].prev = NULL_PROCESS;

	if(process_queue_front == NULL_PROCESS){
		//empty queue so remove the back
		process_queue_back = NULL_PROCESS;
	}

	process_table[return_value].next = NULL_PROCESS;
	process_table[return_value].prev = NULL_PROCESS;

	return return_value;
}
