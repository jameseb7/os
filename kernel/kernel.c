#include <stdint.h>
#include "memory.h"
#include "kutil.h"
#include "interrupts.h"
#include "processes.h"

extern uint32_t mb_magic;

void kmain(void);
void interrupt_handler(void);

void print_A(void);
void print_B(void);
void print_C(void);

void kmain(){
  cli("kmain()");

  clear_screen();
  kprintln("KERNEL STARTED");
    
  if(mb_magic != 0x2BADB002){
    /* Something went not according to specs. Print an error */
    /* message and halt, but do *not* rely on the multiboot */
    /* data structure. */
    kprint("ERROR: Invalid multiboot magic number: ");
    kprintln_uint32(mb_magic);
    return;
  }

  kutil_init();
  kprintln("kutil initialised");

  memory_init();
  kprintln("memory initialised");

  interrupts_init();
  add_interrupt_handler(0x80, (uint32_t) interrupt_handler);  
  kprintln("interrupts initialised");

  processes_init();
  kprintln("processes initialised");

  clear_screen();

  add_process(&print_A);
  add_process(&print_B);
  add_process(&print_C);

  //check_process_stack();

  sti("kmain()");
  halt();
}

void interrupt_handler(void){
   kprintln("Kernel interrupt");
}

uint16_t print_A_process = 1;
uint16_t print_B_process = 2;
uint16_t print_C_process = 3;

void print_A(){
  int i = 0;
  print_A_process = get_current_process_id();
  for(;;){
    kprint("A");
    for(i=0; i < 10000000; i++){
    }
	resume_process(print_B_process);
	suspend_current_process();
  }
}

void print_B(){
  int i = 0;
  print_B_process = get_current_process_id();
  for(;;){
    kprint("B");
    for(i=0; i < 10000000; i++){
    }
	resume_process(print_C_process);
	suspend_current_process();
  }
}

void print_C(){
  int i = 0;
  print_C_process = get_current_process_id();
  for(;;){
    kprint("C");
    for(i=0; i < 10000000; i++){
    }
	resume_process(print_A_process);
	suspend_current_process();
  }
}
