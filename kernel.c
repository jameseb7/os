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
 
  allocate_physical_page(0x00300000);

  ptr = (char *) 0x00300000;
  *ptr = 'A';
  ptr = (char *) 0x00300001;
  *ptr = '\0';

  ptr = (char *) 0x00300000;
  kprintln(ptr);

  ptr = (char *) 0x003000FF;
  *ptr = 'B';
  ptr = (char *) 0x00300100;
  *ptr = 'C';
  ptr = (char *) 0x00300101;
  *ptr = 'D';
  ptr = (char *) 0x00300102;
  *ptr = '\0';

  ptr = (char *) 0x003000FF;
  kprintln(ptr);
  
}

void interrupt_handler(void){
   kprintln("Kernel interrupt");
}
