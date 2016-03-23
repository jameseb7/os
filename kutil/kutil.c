#include "kutil.h"

void kalloc_init(void);
void kutil_init(){
	kalloc_init();
}

void sti(const char * caller){
	__asm__("sti");
	// kprint("sti: ");
	// kprintln(caller);
	caller = caller;
}
void cli(const char * caller){
	__asm__("cli");
	// kprint("cli: ");
	// kprintln(caller);
	caller = caller;
}

void halt(){
  for(;;){
    __asm__("hlt");
  }
}

void error(const char * str){
	kprintln("");
	kprintln(str);
	cli("error()");
	halt();
}

uint8_t inb(uint16_t port){
  uint8_t data;
  __asm__("inb %1, %0 \n\t" : "=a"(data) : "Nd"(port) : );
    return data;
}
void outb(uint16_t port, uint8_t data){
  __asm__("outb %0, %1\n\t" : : "a"(data), "Nd"(port) : );
}
