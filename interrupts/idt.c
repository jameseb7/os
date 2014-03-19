#include <stdint.h>
#include "kernel.h"
#include "asm_functions.h"
#include "idt.h"

struct IDT_entry{
   uint16_t offset_low;
   uint16_t selector;
   uint8_t  zero;
   uint8_t  type_attributes;
   uint16_t offset_high;
};

struct IDT_entry idt[256];

extern void interrupt_entry(void);
extern void interrupt0x0(void);
extern void interrupt0x8(void);
extern void interrupt0xA(void);
extern void interrupt0xB(void);
extern void interrupt0xD(void);
extern void interrupt0xE(void);

uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);

void add_interrupt_handler(uint8_t interrupt, uint32_t handler_address){
  idt[interrupt].offset_low      = (uint16_t) (handler_address & 0xFFFF);
  idt[interrupt].selector        = 0x0008;
  idt[interrupt].zero            = 0x00;
  idt[interrupt].type_attributes = 0x8E;
  idt[interrupt].offset_high     = (uint16_t) (handler_address >> 16);
}

void setup_idt(){
  add_interrupt_handler(0x00, (uint32_t) interrupt0x0);
  add_interrupt_handler(0x08, (uint32_t) interrupt0x8);
  add_interrupt_handler(0x0A, (uint32_t) interrupt0xA);
  add_interrupt_handler(0x0B, (uint32_t) interrupt0xB);
  add_interrupt_handler(0x0D, (uint32_t) interrupt0xD);
  add_interrupt_handler(0x0E, (uint32_t) interrupt0xE);

  load_idt((uint32_t) &idt, sizeof(idt));
}

void remap_interrupts(){
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
}

uint8_t inb(uint16_t port){
  uint8_t data;
  __asm__("inb %1, %0 \n\t" : "=a"(data) : "Nd"(port) : );
    return data;
}
void outb(uint16_t port, uint8_t data){
  __asm__("outb %0, %1\n\t" : : "a"(data), "Nd"(port) : );
}
