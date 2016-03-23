#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void interrupts_init(void);

void add_interrupt_handler(uint8_t interrupt, uint32_t handler_address);

#endif /*INTERRUPTS_H*/
