#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

void halt(void);

uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);

#endif /*KERNEL_H*/
