#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void add_interrupt_handler(uint8_t interrupt, uint32_t handler_address);
void setup_idt(void);
void remap_interrupts(void);

#endif
