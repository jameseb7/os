#include <stdint.h>
#include "kernel.h"

extern uint32_t magic;

void kmain(void);
void interrupt_handler(void);

 
void kmain(){

   /*extern void *mbd;*/
 
   if ( magic != 0x2BADB002 )
   {
      /* Something went not according to specs. Print an error */
      /* message and halt, but do *not* rely on the multiboot */
      /* data structure. */
      return;
   }
 
   clear_screen();
   
   kprintln("Hello, world!");

   __asm__("int $0x0080");

   kprint("hex: ");
   kprint(uint32_to_hex_string(0x123ABC0F));
}

void interrupt_handler(void){
   kprintln("Kernel interrupt");
}
