#ifndef KUTIL_H
#define KUTIL_H

#include <stdint.h>

void clear_screen(void);
void kprint(const char * str);
void kprintln(const char * str);
void kprint_uint32(uint32_t input);
void kprintln_uint32(uint32_t input);
void kprint_uint64(uint64_t input);
void kprintln_uint64(uint64_t input);

void sti(void);
void cli(void);
void halt(void);
void error(const char *);

uint8_t inb(uint16_t);
void outb(uint16_t, uint8_t);

#endif /*KUTIL_H*/
