#include <stdint.h>

#include "processes.h"
#include "kutil.h"
#include "memory.h"

struct process{
	uint32_t page_directory;
	uint32_t stack_pointer;
	uint16_t prev;
	uint16_t next;
	uint16_t parent;
	uint16_t children;
	uint16_t prev_child;
	uint16_t next_child;
	uint32_t flags;
	void (*start_function)(void);
	uint32_t reserved_3;
};

#define MAX_PROCESSES (1 << 16)
#define NULL_PROCESS 0

#define STARTED 0x00000001

//keep index 0 as a null value
struct process process_table[MAX_PROCESSES];

uint16_t process_queue_front = NULL_PROCESS;
uint16_t process_queue_back = NULL_PROCESS;
uint16_t current_process = NULL_PROCESS;

void switch_process(uint16_t, uint16_t);
uint32_t switch_process_asm(uint32_t, uint32_t);
void run_idle_process(uint32_t *);
void start_kernel_process(void (*start_function)(void),
						  uint32_t page_directory, 
						  uint32_t * stack_pointer_store);

void push_to_process_queue(uint16_t);
uint16_t pop_from_process_queue(void);

extern uint32_t kernel_stack_start;


void processes_init(){
	int i;

	for(i = 0; i < MAX_PROCESSES; i++){
		process_table[i].page_directory = 0;
		process_table[i].stack_pointer = 0;
		process_table[i].prev = 0;
		process_table[i].next = 0;
		process_table[i].parent = 0;
		process_table[i].children = 0;
		process_table[i].prev_child = 0;
		process_table[i].next_child = 0;
		process_table[i].flags = 0;
	}
}

void switch_process(uint16_t old_process, uint16_t new_process){
	process_table[old_process].stack_pointer = 
		switch_process_asm(process_table[new_process].stack_pointer,
						   process_table[new_process].page_directory);
}

void run_next_process(){
	uint16_t old_process = current_process;
	
	if(current_process != NULL_PROCESS){ //don't push the null process
		push_to_process_queue(current_process);
	}
	current_process = pop_from_process_queue();
	kprint_uint32(current_process);

	if(current_process == NULL_PROCESS){
		//stop and wait for interrupts if there is no current process
		kprint("idle ");
		run_idle_process(&process_table[old_process].stack_pointer);
	}else{
		if(process_table[current_process].flags & STARTED){
			kprint("process-switching-");
			kprint_uint32(current_process);
			kprint(" ");
			halt();
			switch_process(old_process, current_process);
		}else{
			kprint("process-starting-");
			kprint_uint32(current_process);
			kprint(" ");
			halt();
			process_table[current_process].flags |= STARTED;
			start_kernel_process(process_table[current_process].start_function,
								 process_table[current_process].page_directory, 
								 &process_table[old_process].stack_pointer);
		}
	}
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

static uint16_t process_counter = 1;

void add_process(void (*start_function)(void)){
	process_table[process_counter].page_directory = (uint32_t) make_page_directory();
	process_table[process_counter].stack_pointer = 0;
	process_table[process_counter].flags = 0;
	process_table[process_counter].start_function = start_function;
	
	
	push_to_process_queue(process_counter);

	process_counter++;
}

void check_process_stack(){
	int i;
	for(i = 0; i < 3; i++){
		kprint_uint32(process_table[i].page_directory);
		kprint_uint32(process_table[i].stack_pointer);
		kprint_uint32(process_table[i].prev);
		kprint_uint32(process_table[i].next);
		kprint_uint32(process_table[i].parent);
		kprint_uint32(process_table[i].children);
		kprint_uint32(process_table[i].prev_child);
		kprint_uint32(process_table[i].next_child);
		kprint_uint32(process_table[i].flags);
		kprintln("");
	}
	cli();
	halt();
}
