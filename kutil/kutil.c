#include "kutil.h"

void kalloc_init(void);
void kutil_init(){
	kalloc_init();
}

void sti(void){
	__asm__("sti");
}
void cli(void){
	__asm__("cli");
}

void halt(){
  for(;;){
    __asm__("hlt");
  }
}

void error(const char * str){
	kprintln("");
	kprintln(str);
	cli();
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
