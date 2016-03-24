#include <stdint.h>
#include "kutil.h"
#include "interrupts.h"

struct IDT_entry{
   uint16_t offset_low;
   uint16_t selector;
   uint8_t  zero;
   uint8_t  type_attributes;
   uint16_t offset_high;
};

struct IDT_entry idt[256];

extern void interrupt0x0(void);
extern void interrupt0x1(void);
extern void interrupt0x8(void);
extern void interrupt0xA(void);
extern void interrupt0xB(void);
extern void interrupt0xD(void);
extern void interrupt0xE(void);
extern void interrupt0x20(void);
extern void empty_interrupt_entry(void);

void remap_interrupts(void);
void init_timer(void);

void add_interrupt_handler(uint8_t interrupt, uint32_t handler_address){
  idt[interrupt].offset_low      = (uint16_t) (handler_address & 0xFFFF);
  idt[interrupt].selector        = 0x0008;
  idt[interrupt].zero            = 0x00;
  idt[interrupt].type_attributes = 0x8E;
  idt[interrupt].offset_high     = (uint16_t) (handler_address >> 16);
}

void interrupts_init(){
	uint8_t i;

	remap_interrupts();
	init_timer();

	/*add exception handlers*/
	add_interrupt_handler(0x00, (uint32_t) interrupt0x0);
	add_interrupt_handler(0x01, (uint32_t) interrupt0x1);
	add_interrupt_handler(0x08, (uint32_t) interrupt0x8);
	add_interrupt_handler(0x0A, (uint32_t) interrupt0xA);
	add_interrupt_handler(0x0B, (uint32_t) interrupt0xB);
	add_interrupt_handler(0x0D, (uint32_t) interrupt0xD);
	add_interrupt_handler(0x0E, (uint32_t) interrupt0xE);
	
	/*put an empty handler in for hardware interrupts until something can be found to handle them*/
	for(i = 0x20; i <= 0x28; i++){
		add_interrupt_handler(i, (uint32_t) interrupt0x20);
	}

	/*add hardware interrupt handlers*/
	add_interrupt_handler(0x20, (uint32_t) interrupt0x20);
	
	__asm__(".lcomm  idtr, 6 \n\t"
			"movw %1, idtr   \n\t"
			"movl %0, idtr+2 \n\t"
			"lidt idtr"
			: /*outputs*/
			: "r"(&idt), "r"((uint16_t) sizeof(idt)) /*inputs*/
			: "%eax" /*clobbered registers*/);
}

void remap_interrupts(){
	//uint8_t tmp1, tmp2;

   /*store the PIC masks from the data ports*/
   //tmp1 = inb(0x21);
   //tmp2 = inb(0xA1);

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
   outb(0x21, 0x00);
   outb(0xA1, 0x00);	   
}

#define CLOCK_FREQ 100
void init_timer(){
	int divisor = 1193180/CLOCK_FREQ;

	/*channel 0 -- lobyte/hibyte -- mode 3 -- binary*/
	/*    00    --       11      --  011   --   0   */
	outb(0x43, 0x36); /*set the PIT command register*/

	/*write the divisor to the PIT data register for channel 0*/ 
	outb(0x40, (uint8_t) (divisor & 0xFF));
	outb(0x40, (uint8_t) ((divisor >> 8) & 0xFF));
}


