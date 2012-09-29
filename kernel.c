#include <stdint.h>

extern uint32_t magic;

extern kprint(char *);
 
void kmain(void)
{
   int i;
   char blank_str[80];

   /*extern void *mbd;*/
 
   if ( magic != 0x2BADB002 )
   {
      /* Something went not according to specs. Print an error */
      /* message and halt, but do *not* rely on the multiboot */
      /* data structure. */
      return;
   }
 
   for(i = 0; i < 80; i++){
      blank_str[i] = ' ';
   }
   for(i = 0; i < 25; i++){
      kprint(blank_str);
   }
   
   kprint("Hello, world!");
   kprint("Testing...");

   __asm__("int $0x0080");
   kprint("interrupt finished");
}

void interrupt_handler(void){
   kprint("Kernel interrupt");
}
