#include <stdint.h>
#include "memory.h"
#include "kutil.h"
#include "interrupts.h"
#include "processes.h"

extern uint32_t mb_magic;

void kmain(void);
void interrupt_handler(void);

void kmain(){

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

  while(1){
	  kprint(".");
  }
}

void interrupt_handler(void){
   kprintln("Kernel interrupt");
}
