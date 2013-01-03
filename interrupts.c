#include <stdint.h>
#include "kernel.h"

struct IDT_entry{
   uint16_t offset_low;
   uint16_t selector;
   uint8_t  zero;
   uint8_t  type_attr;
   uint16_t offset_high;
}

struct IDT_entry * idtptr; /*pointer to first entry in IDT*/
uint16_t idtsize;
extern uint32_t isr_entry;

void make_idt(){
   int i;
   for(i = 0; i < 256; i++){
      switch(i){
      case 0x80: /*kernel interrupt handler*/
         idtptr[i] = {(uint16_t)(isr_entry & 0xFFFF), 0x0000, 0x00, 0x8E, (uint16_t)(isr_entry >> 16)};
      default: /*default blank IDT entry*/
         idtptr[i] = {0x0000, 0x0000, 0x00, 0x00, 0x0000};
      }
   }
   idtsize = 256*sizeof(IDT_entry);
}

void interrupt_handler(){
   kprint("interrupt 0x80 caught")
}
