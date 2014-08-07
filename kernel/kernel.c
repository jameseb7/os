#include <stdint.h>
#include "memory.h"
#include "kutil.h"
#include "interrupts.h"
#include "processes.h"

extern uint32_t mb_magic;

void kmain(void);
void interrupt_handler(void);

void kmain(){
  char * ptr;

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

  ptr = kalloc(2);
  kprint("allocation 1: ");
  kprintln_uint32((uint32_t) ptr);
  *ptr = '1';

  kprintn((char *) ((uint32_t) ptr - 16), 0x100);
  kprintln("");

  ptr = kalloc(10);
  kprint("allocation 2: ");
  kprintln_uint32((uint32_t) ptr);
  *ptr = 'a';
  *(ptr+1) = 'l';
  *(ptr+2) = 'l';
  *(ptr+3) = 'o';
  *(ptr+4) = 'c';
  *(ptr+5) = '-';
  *(ptr+6) = '-';
  *(ptr+7) = '-';
  *(ptr+8) = '2';
  
  kprintn((char *) ((uint32_t) ptr - 16), 0x100);
  kprintln("");

  ptr = kalloc(5);
  kprint("allocation 3: ");
  kprintln_uint32((uint32_t) ptr);
  *ptr = 'a';
  *(ptr+1) = 'l';
  *(ptr+2) = 'c';
  *(ptr+3) = '3';

  kprintn((char *) ((uint32_t) ptr - 16), 0x100);
  kprintln("");

  ptr = kalloc(128);
  kprint("allocation 4: ");
  kprintln_uint32((uint32_t) ptr);
  *ptr = 'a';
  *(ptr+1) = 'l';
  *(ptr+2) = 'l';
  *(ptr+3) = 'o';
  *(ptr+4) = 'c';
  *(ptr+5) = 'a';
  *(ptr+6) = 't';
  *(ptr+7) = 'i';
  *(ptr+8) = 'o';
  *(ptr+9) = 'n';
  *(ptr+10) = '-';
  *(ptr+11) = '-';
  *(ptr+12) = '-';
  *(ptr+13) = '-';
  *(ptr+14) = '-';
  *(ptr+15) = '-';
  *(ptr+16) = '4';

  kprintn((char *) ((uint32_t) ptr - 16), 0x100);
  kprintln("");

  cli();
  halt();
}

void interrupt_handler(void){
   kprintln("Kernel interrupt");
}
