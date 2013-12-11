#ifndef ASM_FUNCTIONS_H
#define ASM_FUNCTIONS_H

#include <stdint.h>

void load_gdt(uint32_t gdt, uint16_t gdt_size);

void load_idt(uint32_t idt, uint16_t idt_size);

void load_page_directory(uint8_t * page_directory);
void enable_paging(void);

/*uint8_t inb(uint8_t port);*/
/*void outb(uint8_t data, uint8_t port);*/

#endif
