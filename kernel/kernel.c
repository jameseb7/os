#include <stdint.h>
#include "kernel.h"
#include "memory.h"
#include "koutput.h"
#include "interrupts.h"

extern uint32_t mb_magic;

void kmain(void);
void interrupt_handler(void);

void kmain(){
  uint32_t * ptr;

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

  memory_init();

  interrupts_init();
  add_interrupt_handler(0x80, (uint32_t) interrupt_handler);
  
  clear_screen();

  kprintln("TEST");
  __asm__("int $0x80");

  ptr = allocate_virtual_pages_high(0x1000);
  kprint("allocated (1): ");
  kprintln_uint32((uint32_t) ptr);
  *ptr = 3;
  kprintln_uint32(*ptr);

  ptr = allocate_virtual_pages_high(0x4000);
  kprint("allocated (2): ");
  kprintln_uint32((uint32_t) ptr);
  ptr += 0x3FF0 >> 2;
  *ptr = 10;
  kprintln_uint32(*ptr);

  ptr = allocate_virtual_pages_low(0x4000);
  kprint("allocated (3): ");
  kprintln_uint32((uint32_t) ptr);
  ptr += 0x3FF0 >> 2;
  *ptr = 14;
  kprintln_uint32(*ptr);
}

void halt(){
  __asm__("cli");
  for(;;){
    __asm__("hlt");
  }
}

void interrupt_handler(void){
   kprintln("Kernel interrupt");
}
