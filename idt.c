#include <stdint.h>
#include "kernel.h"

struct IDT_entry{
   uint16_t offset_low;
   uint16_t selector;
   uint8_t  zero;
   uint8_t  type_attributes;
   uint16_t offset_high;
};

struct IDT_entry * idt = (struct IDT_entry *) 0x00000700;
uint16_t idt_size;

extern void * interrupt_entry;

void make_idt(){
   int i;
   for(i = 0; i < 256; i++){
      switch(i){
      case 0x80:
	idt[i].offset_low      = (uint16_t) ((uint32_t)&interrupt_entry) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t)&interrupt_entry) >> 16);
	break;
      default:
	idt[i].offset_low      = 0x0000;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x0E;
	idt[i].offset_high     = 0x0000;
      }
   }
   
   idt_size = 256 * sizeof(struct IDT_entry);
}