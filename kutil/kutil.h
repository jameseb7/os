#ifndef KUTIL_H
#define KUTIL_H

#include <stdint.h>

void kutil_init(void);

void * kalloc(uint32_t size);

void clear_screen(void);
void kprint(const char * str);
void kprintln(const char * str);
void kprintn(const char * str, unsigned int n);
void kprint_uint32(uint32_t input);
void kprintln_uint32(uint32_t input);
void kprint_uint64(uint64_t input);
void kprintln_uint64(uint64_t input);

void sti(void);
void cli(void);
void halt(void);
void error(const char * str);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);

#endif /*KUTIL_H*/
