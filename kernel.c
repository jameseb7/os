#include <stdint.h>
#include "kernel.h"

extern uint32_t magic;

void kmain(void);
void interrupt_handler(void);

void kmain(){
  char * ptr;
    /*extern void *mbd;*/

  kprintln("KERNEL STARTED");
    
  if(magic != 0x2BADB002){
    /* Something went not according to specs. Print an error */
    /* message and halt, but do *not* rely on the multiboot */
    /* data structure. */
    kprint("ERROR: Invalid multiboot magic number: ");
    kprintln(uint32_to_hex_string(magic));
    return;
  }
  
  clear_screen();

  ptr = allocate_virtual_pages(1);

  /*return;*/

  kprintln(uint32_to_hex_string((uint32_t) ptr));
  ptr[0] = 'A';
  ptr[1] = 'B';
  ptr[2] = 'C';
  ptr[3] = '\0';
  kprintln(ptr);

  ptr = allocate_virtual_pages(8);
  kprintln(uint32_to_hex_string((uint32_t) ptr));
  ptr[4096*8 - 3] = 'D';
  ptr[4096*8 - 2] = 'E';
  ptr[4096*8 - 1] = '\0';
  kprintln(ptr + 4096*8 - 3);

  ptr = allocate_virtual_pages(6);
  kprintln(uint32_to_hex_string((uint32_t) ptr));
  ptr[4096*8 - 3] = 'F';
  ptr[4096*8 - 2] = 'G';
  ptr[4096*8 - 1] = '\0';
  kprintln(ptr + 4096*8 - 3);
}

void halt(){
  asm("cli");
  for(;;){
    asm("hlt");
  }
}

void interrupt_handler(void){
   kprintln("Kernel interrupt");
}
