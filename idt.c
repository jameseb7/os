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

extern void interrupt_entry(void);
extern void interrupt0x0(void);
extern void interrupt0x8(void);
extern void interrupt0xA(void);
extern void interrupt0xB(void);
extern void interrupt0xC(void);
extern void interrupt0xD(void);

uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);

void make_idt(){
   int i;
   uint8_t tmp1, tmp2;

   /*store the PIC masks from the data ports*/
   tmp1 = inb(0x21);
   tmp2 = inb(0xA1);

   /*send initialisation byte to the PIC command ports*/
   outb(0x20, 0x11); /*PIC master*/
   outb(0xA0, 0x11); /*PIC slave*/
   outb(0x80, 0x00); /*wait extra cycle for IO to complete*/

   /*send interrupt vector offsets to the PIC data ports*/
   outb(0x21, 0x20); /*PIC master*/
   outb(0xA1, 0x28); /*PIC slave*/
   outb(0x80, 0x00); /*wait extra cycle for IO to complete*/

   /*send IRQ mask of slave to master PIC data port*/
   outb(0x21, 0x04);
   /*send IRQ number of slave to the slave PIC data port*/
   outb(0xA1, 0x02);
   outb(0x80, 0x00); /*wait extra cycle for IO to complete*/

   /*send the fourth initialization byte to the PIC data ports*/
   outb(0x21, 0x01); /*PIC master*/
   outb(0xA1, 0x01); /*PIC slave*/
   outb(0x80, 0x00); /*wait extra cycle for IO to complete*/

   /*restore the PIC masks*/
   outb(0x21, tmp1);
   outb(0xA1, tmp2);
	   

   for(i = 0; i < 256; i++){
      switch(i){
      case 0x0:
	idt[i].offset_low      = (uint16_t) ((uint32_t) &interrupt0x0) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t) &interrupt0x0) >> 16);
	break;

      case 0x8:
	idt[i].offset_low      = (uint16_t) ((uint32_t) &interrupt0x8) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t) &interrupt0x8) >> 16);
	break;

      case 0xA:
	idt[i].offset_low      = (uint16_t) ((uint32_t) &interrupt0xA) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t) &interrupt0xA) >> 16);
	break;

      case 0xB:
	idt[i].offset_low      = (uint16_t) ((uint32_t) &interrupt0xB) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t) &interrupt0xB) >> 16);
	break;

      case 0xC:
	idt[i].offset_low      = (uint16_t) ((uint32_t) &interrupt0xC) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t) &interrupt0xC) >> 16);
	break;

      case 0xD:
	idt[i].offset_low      = (uint16_t) ((uint32_t) &interrupt0xD) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t) &interrupt0xD) >> 16);
	break;

      case 0x80:
	idt[i].offset_low      = (uint16_t) ((uint32_t) &interrupt_entry) & 0xFFFF;
	idt[i].selector        = 0x0008;
	idt[i].zero            = 0x00;
	idt[i].type_attributes = 0x8E;
	idt[i].offset_high     = (uint16_t) (((uint32_t) &interrupt_entry) >> 16);
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

uint8_t inb(uint16_t port){
  uint8_t data;
  __asm__("inb %1, %0 \n\t" : "=a"(data) : "Nd"(port) : );
    return data;
}
void outb(uint16_t port, uint8_t data){
  __asm__("outb %0, %1\n\t" : : "a"(data), "Nd"(port) : );
}
