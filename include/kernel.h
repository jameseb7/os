#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

void halt(void);

/*output functions*/
unsigned int write_screen(const char * str,
		 char foreclr, char backclr,
		 unsigned int x, unsigned int y);
void clear_screen(void);
void kprint(const char * str);
void kprintln(const char * str);
void kprint_uint32(uint32_t input);
void kprintln_uint32(uint32_t input);
void kprint_uint64(uint64_t input);
void kprintln_uint64(uint64_t input);
#endif /*KERNEL_H*/
