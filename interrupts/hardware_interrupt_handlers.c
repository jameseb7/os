#include "kutil.h"

void timer_handler(void);

uint32_t counter = 100;
void timer_handler(){
	__asm__("cli");

	if(counter <= 0){
		kprint("tick ");
		counter = 100;
	}else{
		counter--;
	}

	outb(0x20, 0x20);
	__asm__("sti");
}
